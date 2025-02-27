/*  Audio Worker
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "AudioConstants.h"
#include "AudioInfo.h"
#include "AudioWorker.h"
#include "AudioFileLoader.h"
#include "AudioFormatUtils.h"
#include "CommonFramework/Logging/LoggerQt.h"

#include <QIODevice>
#include <QThread>

#if QT_VERSION_MAJOR == 5
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioOutput>
#include <QtEndian>
using AudioSource = QAudioInput;
using AudioSink = QAudioOutput;
#elif QT_VERSION_MAJOR == 6
#include <QMediaDevices>
#include <QAudioSource>
#include <QAudioSink>
#include <QAudioDevice>
using AudioSource = QAudioSource;
using AudioSink = QAudioSink;
#endif


#include <iomanip>
#include <cfloat>
#include <chrono>
#include <memory>
#include <cassert>
#include <cstring>
#include <set>

#include <iostream>
using std::cout;
using std::endl;

//#define DEBUG_AUDIO_IO

namespace PokemonAutomation{



AudioWorker::AudioWorker(
    LoggerQt& logger,
    const AudioDeviceInfo& inputInfo,
    AudioFormat inputFormat,
    const QString& inputAbsoluteFilepath,
    const AudioDeviceInfo& outputInfo,
    float outputVolume
)
    : m_logger(logger)
    , m_inputInfo(inputInfo)
    , m_inputFormat(inputFormat)
    , m_inputAbsoluteFilepath(inputAbsoluteFilepath)
    , m_outputInfo(outputInfo)
    , m_volume(std::max(std::min(outputVolume, 1.0f), 0.0f))
{
    qRegisterMetaType<std::shared_ptr<AlignedVector<float>>>("std::shared_ptr<AlignedVector<float>>");
    qRegisterMetaType<std::shared_ptr<const AlignedVector<float>>>("std::shared_ptr<const AlignedVector<float>>");
}


void AudioWorker::startAudio(){
#ifdef DEBUG_AUDIO_IO
    std::cout << "T" << QThread::currentThread() << " AudioWorker::startAudio()" << std::endl;
#endif

    bool foundAudioInputInfo = false;
    bool foundAudioOutputInfo = false;
    QAudioFormat inputAudioFormat, outputAudioFormat;

    NativeAudioInfo chosenAudioInputDevice = m_inputInfo.native_info();
    foundAudioInputInfo = m_inputInfo;

    NativeAudioInfo chosenAudioOutputDevice = m_outputInfo.native_info();
    foundAudioOutputInfo = m_outputInfo;

    // If input filename is not empty, load audio from file:
    if (m_inputAbsoluteFilepath.size() > 0){
        // We hard code file audio format to be mono channel 48KHz.
        inputAudioFormat.setChannelCount(2);
#if QT_VERSION_MAJOR == 5
        inputAudioFormat.setCodec("audio/pcm");
#endif
        inputAudioFormat.setSampleRate(48000);
        setSampleFormatToFloat(inputAudioFormat);
        m_channelMode = ChannelMode::Stereo;

        // Note: m_inputAbsoluteFilepath must be an absolute file path. Otherwise it may trigger a bug
        // in QAudioDecoder used in AudioFileLoader, which will either stops the audio stream
        // halfway or crash the program when deleting QAudioDecoder.
        m_FileLoader = new AudioFileLoader(this, m_inputAbsoluteFilepath, inputAudioFormat);
        if (m_FileLoader->start() == false){
            return;
        }
        outputAudioFormat = m_FileLoader->audioFormat();
        m_logger.log("Set output audio format to: " + dumpAudioFormat(outputAudioFormat));

        connect(m_FileLoader, &AudioFileLoader::bufferReady, this, [&](const char* data, size_t len){
            if (m_audioIODevice){
                m_audioIODevice->writeData(data, len);
            }
        });

    }else{
        // Load from audio input device:

        if (foundAudioInputInfo == false){
            // std::cout << "Cannot build Qt6VideoWidget: cannot found audio device name matching: " << inputInfo.device_name() << std::endl;
            return;
        }

        inputAudioFormat = chosenAudioInputDevice.preferredFormat();
        m_logger.log("Default input audio format: " + dumpAudioFormat(inputAudioFormat));

        set_format(inputAudioFormat, m_inputFormat);
        outputAudioFormat = inputAudioFormat;

        switch (m_inputFormat){
        case AudioFormat::MONO_48000:
        case AudioFormat::MONO_96000:
        case AudioFormat::DUAL_44100:
        case AudioFormat::DUAL_48000:
            break;
        case AudioFormat::INTERLEAVE_LR_96000:
        case AudioFormat::INTERLEAVE_RL_96000:
#if QT_VERSION_MAJOR == 5
            outputAudioFormat.setChannelCount(2);
#elif QT_VERSION_MAJOR == 6
            outputAudioFormat.setChannelConfig(QAudioFormat::ChannelConfig::ChannelConfigStereo);
#endif
            outputAudioFormat.setSampleRate(48000);
            break;
        default:
            m_logger.log(std::string("Invalid AudioFormat: ") + AUDIO_FORMAT_LABELS[(size_t)m_inputFormat]);
            return;
        }


        m_logger.log("Set input audio format to: " + dumpAudioFormat(inputAudioFormat));
        m_logger.log("Set output audio format to: " + dumpAudioFormat(outputAudioFormat));
        
        if (!chosenAudioInputDevice.isFormatSupported(inputAudioFormat)){
            std::cout << "Error: audio input device cannot support desired audio format" << std::endl;
            return;
        }

        const int bytesPerSample = inputAudioFormat.bytesPerFrame() / inputAudioFormat.channelCount();
        if (bytesPerSample != sizeof(float)){
            std::cout << "Error: audio format is wrong. Set its sample format to float but the bytesPerSample is "
                << bytesPerSample << ", different from float size " << sizeof(float) << std::endl;
            return;
        }

        m_audioSource = new AudioSource(chosenAudioInputDevice, inputAudioFormat, this);

        connect(
            m_audioSource, &AudioSource::stateChanged,
            this, [&](QAudio::State newState){
                this->handleDeviceErrorState(newState, m_audioSource->error(), "AudioSource");
            }
        );
    } // end if load audio from input audio device
    
    m_audioIODevice = new AudioIODevice(m_inputFormat);
    m_audioIODevice->open(QIODevice::ReadWrite | QIODevice::Unbuffered);
    connect(m_audioIODevice, &AudioIODevice::fftInputReady, this, &AudioWorker::fftInputReady);
    
    if (m_audioSource){
        m_audioSource->start(m_audioIODevice);
    }

    if (foundAudioOutputInfo){
        bool outputSupported = chosenAudioOutputDevice.isFormatSupported(outputAudioFormat);
        if (!outputSupported){
            std::cout << "Error the audio output device does not support the requested audio format" << std::endl;
        } else{
            m_audioSink = new AudioSink(chosenAudioOutputDevice, outputAudioFormat, this);
            m_audioSink->setBufferSize(32768);
            m_audioSink->setVolume(m_volume);
            m_audioIODevice->setAudioSinkDevice(m_audioSink->start());

            connect(
                m_audioSink, &AudioSink::stateChanged,
                this, [&](QAudio::State newState){
                    this->handleDeviceErrorState(newState, m_audioSink->error(), "AudioSink");
                }
            );
        }
    }
}

AudioWorker::~AudioWorker(){
    if (m_audioIODevice){
        // Close the connection between m_audioIODevice and 
        // m_audioSink.
        m_audioIODevice->setAudioSinkDevice(nullptr);
    }

    if (m_audioSink){
        m_audioSink->stop();
        delete m_audioSink;
        m_audioSink = nullptr;
    }

    if (m_audioSource){
        m_audioSource->stop();
        delete m_audioSource;
        m_audioSource = nullptr;
    }

    if (m_FileLoader){
        delete m_FileLoader;
        m_FileLoader = nullptr;
    }

    if (m_audioIODevice){
        delete m_audioIODevice;
        m_audioIODevice = nullptr;
    }

    m_logger.log("AudioWorker destroyed");
}

void AudioWorker::setVolume(float volume){
    volume = std::max(std::min(volume, 1.0f), 0.0f);
    m_volume = volume;
    if (m_audioSink){
        m_audioSink->setVolume(volume);
    }
}

void AudioWorker::handleDeviceErrorState(QAudio::State newState, QAudio::Error error, const char* deviceType){
    // TODO connect logger output to it
    switch (newState) {
    case QAudio::StoppedState:
        switch (error) {
        case QAudio::NoError:
            m_logger.log(std::string(deviceType) + " stopped normally");
            break;
        case QAudio::OpenError:
            m_logger.log(std::string(deviceType) + " OpenError");
            break;
        case QAudio::IOError:
            m_logger.log(std::string(deviceType) + " IOError");
            break;
        case QAudio::UnderrunError:
            // Underrun error happens on audio sink when the audio thread is closing.
            // So we don't print this error if it's on audio sink.
            if (strcmp(deviceType, "AudioSink") != 0){
                m_logger.log(std::string(deviceType) + " UnderrunError");
            }
            break;
        case QAudio::FatalError:
            m_logger.log(std::string(deviceType) + " FatalError");
            break;
        }
        break;

    case QAudio::ActiveState:
        // Started recording - read from IO device
//        m_logger.log("Audio started");
        break;
    
    case QAudio::SuspendedState:
        m_logger.log(std::string(deviceType) + " suspended");
        break;

    case QAudio::IdleState:
//        m_logger.log("AudioSource is idle, no input");
        break;
#if QT_VERSION_MAJOR == 5
    case QAudio::InterruptedState:
        m_logger.log("AudioSource is interrupted, no input");
        break;
#endif
    } // end switch newState
}


}

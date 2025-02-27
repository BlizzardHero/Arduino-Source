/*  Audio Template
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_AudioPipeline_AudioTemplate_H
#define PokemonAutomation_AudioPipeline_AudioTemplate_H

#include <cstddef>
#include <vector>

#include <QString>

namespace PokemonAutomation{

// Hold the sepctrogram of an audio, used as a template to
// match sounds from an audio stream.
class AudioTemplate{
public:
    AudioTemplate() {}
    AudioTemplate(std::vector<float>&& spectrogram, size_t numWindows);
    
    size_t numWindows() const { return m_numWindows; }
    size_t numFrequencies() const { return m_numFrequencies; }
    const std::vector<float>& spectrogram() const { return m_spectrogram; }

    const float* getWindow(size_t windowIndex) const { return m_spectrogram.data() + windowIndex * m_numFrequencies; }

    void scale(float s) { for(auto& v: m_spectrogram) v *= s; }

private:
    size_t m_numWindows = 0;
    size_t m_numFrequencies = 0;
    std::vector<float> m_spectrogram;
};

AudioTemplate loadAudioTemplate(const QString& filename, size_t sampleRate = 48000);









}

#endif

/*  Bot-Base Wrapper Handle
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include <emmintrin.h>
#include <QtGlobal>
#include <QMessageBox>
#include "Common/Cpp/PrettyPrint.h"
#include "Common/Cpp/Exceptions.h"
#include "Common/Cpp/PanicDump.h"
#include "Common/Microcontroller/DeviceRoutines.h"
#include "Common/NintendoSwitch/NintendoSwitch_Tools.h"
#include "ClientSource/Libraries/MessageConverter.h"
#include "ClientSource/Connection/SerialConnection.h"
#include "ClientSource/Connection/PABotBase.h"
#include "CommonFramework/Globals.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_Device.h"
#include "BotBaseHandle.h"

namespace PokemonAutomation{


BotBaseHandle::BotBaseHandle(
    SerialLogger& logger,
    const QSerialPortInfo& port,
    PABotBaseLevel minimum_pabotbase
)
    : m_logger(logger)
    , m_port(port)
    , m_minimum_pabotbase(minimum_pabotbase)
    , m_current_pabotbase(PABotBaseLevel::NOT_PABOTBASE)
    , m_state(State::NOT_CONNECTED)
    , m_allow_user_commands(true)
{
    reset(port);
}
BotBaseHandle::~BotBaseHandle(){
    stop();
    m_botbase.reset();
    m_state.store(State::NOT_CONNECTED, std::memory_order_release);
    emit on_not_connected("");
}

BotBase* BotBaseHandle::botbase(){
    return m_botbase.get();
}

BotBaseHandle::State BotBaseHandle::state() const{
    return m_state.load(std::memory_order_acquire);
}
bool BotBaseHandle::accepting_commands() const{
    return state() == State::READY &&
        m_current_pabotbase.load(std::memory_order_acquire) > PABotBaseLevel::NOT_PABOTBASE;
}

void BotBaseHandle::set_allow_user_commands(bool allow){
    m_allow_user_commands.store(allow, std::memory_order_release);
}

const char* BotBaseHandle::try_reset(){
    std::unique_lock<std::mutex> lg(m_lock, std::defer_lock);
    if (!lg.try_lock()){
        return "Console is busy.";
    }
    if (state() != State::READY){
        return "Console is not accepting commands right now.";
    }
    if (!m_allow_user_commands.load(std::memory_order_acquire)){
        return "Cannot reset while a program is running.";
    }
    reset_unprotected(m_port);
    return nullptr;
}
const char* BotBaseHandle::try_send_request(const BotBaseRequest& request){
    std::unique_lock<std::mutex> lg(m_lock, std::defer_lock);
    if (!lg.try_lock()){
        return "Console is busy.";
    }
    if (state() != State::READY){
        return "Console is not accepting commands right now.";
    }
    if (m_current_pabotbase.load(std::memory_order_acquire) <= PABotBaseLevel::NOT_PABOTBASE){
        return "Device is not running PABotBase.";
    }
    if (!m_allow_user_commands.load(std::memory_order_acquire)){
        return "Handle is not accepting commands right now.";
    }
    if (!botbase()->try_issue_request(request)){
        return "Command dropped.";
    }
    return nullptr;
}

void BotBaseHandle::stop_unprotected(){
    {
        State state = m_state.load(std::memory_order_acquire);
        if (state == State::NOT_CONNECTED){
            return;
        }
        if (state == State::SHUTDOWN){
            while (m_state.load(std::memory_order_acquire) != State::NOT_CONNECTED){
                _mm_pause();
            }
            return;
        }

        m_state.store(State::SHUTDOWN, std::memory_order_release);
        emit on_stopped("");
        m_botbase->stop();

        std::lock_guard<std::mutex> lg(m_cv_lock);
        m_cv.notify_all();
    }

    if (m_status_thread.joinable()){
        m_status_thread.join();
    }

    m_state.store(State::NOT_CONNECTED, std::memory_order_release);
    emit on_not_connected("");
}
void BotBaseHandle::reset_unprotected(const QSerialPortInfo& port){
    using namespace PokemonAutomation;

    stop_unprotected();
    if (port.isNull()){
        return;
    }

    std::string name = port.systemLocation().toUtf8().data();
    std::string error;


    if (port.description().indexOf("Prolific") != -1){
        QMessageBox box;
        box.critical(
            nullptr,
            "Error",
            "Cannot select Prolific controller.<br><br>"
            "Prolific controllers do not work for Arduino and similar microntrollers.<br>"
            "You were warned of this in the setup instructions. Please buy a CP210x controller instead."
        );
        emit on_not_connected("<font color=\"red\">Cannot connect to Prolific controller.</font>");
        m_logger.log("Unable to connect due to Prolific controller.");
        return;
    }

    try{
        std::unique_ptr<SerialConnection> connection(new SerialConnection(name, PABB_BAUD_RATE));
        m_botbase.reset(new PABotBase(m_logger, std::move(connection), nullptr));
        m_current_pabotbase.store(PABotBaseLevel::NOT_PABOTBASE, std::memory_order_release);
    }catch (const ConnectionException& e){
        error = e.message();
    }catch (const SerialProtocolException& e){
        error = e.message();
    }
    if (error.empty()){
        m_state.store(State::CONNECTING, std::memory_order_release);
        emit on_connecting();
    }else{
        emit on_not_connected("<font color=\"red\">Unable to open port.</font>");
//        m_logger.log(error, Color());
        return;
    }

    m_status_thread = std::thread(run_with_catch, "BotBaseHandle::thread_body()", [=]{ thread_body(); });
}

void BotBaseHandle::stop(){
    std::lock_guard<std::mutex> lg(m_lock);
    stop_unprotected();
}
void BotBaseHandle::reset(const QSerialPortInfo& port){
    std::lock_guard<std::mutex> lg(m_lock);
    reset_unprotected(port);
}


void BotBaseHandle::verify_protocol(){
    uint32_t protocol = Microcontroller::protocol_version(*m_botbase);
    uint32_t version_hi = protocol / 100;
    uint32_t version_lo = protocol % 100;
    if (version_hi != PABB_PROTOCOL_VERSION / 100 || version_lo < PABB_PROTOCOL_VERSION % 100){
        throw SerialProtocolException(
            m_logger, PA_CURRENT_FUNCTION,
            "Incompatible version. Client: " + std::to_string(PABB_PROTOCOL_VERSION) + ", Device: " + std::to_string(protocol) + "<br>"
            "Please install the .hex that came with this version of the program."
        );
    }
}
uint8_t BotBaseHandle::verify_pabotbase(){
    using namespace PokemonAutomation;

    uint8_t program_id = Microcontroller::program_id(*m_botbase);
    PABotBaseLevel type = program_id_to_botbase_level(program_id);
    m_current_pabotbase.store(type, std::memory_order_release);
    if (type < m_minimum_pabotbase){
        throw SerialProtocolException(
            m_logger, PA_CURRENT_FUNCTION,
            "PABotBase level not met. (" + program_name(program_id) + ")"
        );
    }
    return program_id;
}
void BotBaseHandle::thread_body(){
    using namespace PokemonAutomation;

    m_botbase->set_sniffer(&m_logger);

    //  Connect
    {
        QString error;
        try{
            m_botbase->connect();
        }catch (InvalidConnectionStateException&){
            m_botbase->stop();
            emit on_stopped("");
            return;
        }catch (SerialProtocolException& e){
            error = QString::fromStdString(e.message());
        }
        if (!error.isEmpty()){
            m_botbase->stop();
            emit on_stopped("<font color=\"red\">" + error + "</font>");
            return;
        }
    }

    //  Check protocol and version.
    {
        uint8_t program_id = 0;
        uint32_t version = 0;
        QString error;
        try{
            verify_protocol();
            program_id = verify_pabotbase();
            version = Microcontroller::program_version(*m_botbase);
        }catch (InvalidConnectionStateException&){
            return;
        }catch (SerialProtocolException& e){
            error = QString::fromStdString(e.message());
        }
        if (error.isEmpty()){
            m_state.store(State::READY, std::memory_order_release);
            emit on_ready(QString::fromStdString(
                "<font color=\"blue\">Program: " +
                program_name(program_id) +
                " (" + std::to_string(version) + ")</font>"
            ));
        }else{
            m_state.store(State::STOPPED, std::memory_order_release);
            emit on_stopped("<font color=\"red\">" + error + "</font>");
            m_botbase->stop();
            return;
        }
    }

    std::thread watchdog([=]{
        while (true){
            if (m_state.load(std::memory_order_acquire) != State::READY){
                break;
            }

            auto last = std::chrono::system_clock::now() - m_botbase->last_ack();
            std::chrono::duration<double> seconds = last;
            if (last > 2 * SERIAL_REFRESH_RATE){
                emit uptime_status(
                    QString("<font color=\"red\">Last Ack: ") +
                    QString::fromStdString(tostr_fixed(seconds.count(), 3)) + " seconds ago</font>"
                );
//                m_logger.log("Connection issue detected. Turning on all logging...");
//                settings.log_everything.store(true, std::memory_order_release);
            }

            std::unique_lock<std::mutex> lg(m_cv_lock);
            if (m_state.load(std::memory_order_acquire) != State::READY){
                break;
            }
            m_cv.wait_for(lg, SERIAL_REFRESH_RATE);
        }
    });

    BotBaseContext context(*m_botbase);
    while (true){
        if (m_state.load(std::memory_order_acquire) != State::READY){
            break;
        }

        std::string str;
        QString error;
        try{
//            cout << "system_clock()" << endl;
            uint32_t wallclock = NintendoSwitch::system_clock(context);
//            cout << "system_clock() - done" << endl;
            str = NintendoSwitch::ticks_to_time(wallclock);
        }catch (InvalidConnectionStateException&){
            break;
        }catch (SerialProtocolException& e){
            error = QString::fromStdString(e.message());
        }
        if (error.isEmpty()){
            emit uptime_status("<font color=\"blue\">Up Time: " + QString::fromStdString(str) + "</font>");
        }else{
            emit uptime_status("<font color=\"red\">Up Time: " + error + "</font>");
            error.clear();
        }

//        cout << "lock()" << endl;
        std::unique_lock<std::mutex> lg(m_cv_lock);
//        cout << "lock() - done" << endl;
        if (m_state.load(std::memory_order_acquire) != State::READY){
            break;
        }
        m_cv.wait_for(lg, SERIAL_REFRESH_RATE);
    }

    {
        std::unique_lock<std::mutex> lg(m_cv_lock);
        m_cv.notify_all();
    }
    watchdog.join();
}



}

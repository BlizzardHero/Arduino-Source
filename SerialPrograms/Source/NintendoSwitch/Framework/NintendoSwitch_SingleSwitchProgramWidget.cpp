/*  Single Switch Program Template
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "CommonFramework/Tools/BlackBorderCheck.h"
#include "NintendoSwitch_SingleSwitchProgramWidget.h"

namespace PokemonAutomation{
namespace NintendoSwitch{


SingleSwitchProgramWidget::~SingleSwitchProgramWidget(){
    on_destruct_stop();
}
SingleSwitchProgramWidget* SingleSwitchProgramWidget::make(
    QWidget& parent,
    SingleSwitchProgramInstance& instance,
    PanelListener& listener
){
    SingleSwitchProgramWidget* widget = new SingleSwitchProgramWidget(parent, instance, listener);
    widget->construct();
    return widget;
}
void SingleSwitchProgramWidget::run_program(
    StatsTracker* current_stats,
    const StatsTracker* historical_stats
){
    SingleSwitchProgramInstance& instance = static_cast<SingleSwitchProgramInstance&>(m_instance);
    SingleSwitchProgramEnvironment env(
        ProgramInfo(
            instance.descriptor().identifier(),
            instance.descriptor().category(),
            instance.descriptor().display_name(),
            timestamp()
        ),
        m_logger,
        current_stats, historical_stats,
        system().logger(),
        sanitize_botbase(system().botbase()),
        system().camera(),
        system().overlay(),
        system().audio()
    );
    connect(
        this, &RunnableSwitchProgramWidget::signal_cancel,
        &env, [&]{
            m_state.store(ProgramState::STOPPING, std::memory_order_release);
            env.signal_stop();
        },
        Qt::DirectConnection
    );
    connect(
        &env, &ProgramEnvironment::set_status,
        this, &SingleSwitchProgramWidget::status_update
    );

    try{
        start_program_video_check(env.console, instance.descriptor().feedback());
        instance.program(env);
    }catch (...){
        env.update_stats();
        throw;
    }
}





}
}

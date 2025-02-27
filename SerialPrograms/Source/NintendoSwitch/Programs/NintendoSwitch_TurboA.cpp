/*  Turbo A
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "NintendoSwitch/Commands/NintendoSwitch_Commands_PushButtons.h"
#include "NintendoSwitch/NintendoSwitch_Settings.h"
#include "PokemonSwSh/Commands/PokemonSwSh_Commands_GameEntry.h"
#include "NintendoSwitch_TurboA.h"

namespace PokemonAutomation{
namespace NintendoSwitch{


TurboA_Descriptor::TurboA_Descriptor()
    : RunnableSwitchProgramDescriptor(
        "PokemonSwSh:TurboA",
        "Nintendo Switch", "Turbo A",
        "ComputerControl/blob/master/Wiki/Programs/NintendoSwitch/TurboA.md",
        "Endlessly mash A.",
        FeedbackType::NONE, false,
        PABotBaseLevel::PABOTBASE_12KB
    )
{}



TurboA::TurboA(const TurboA_Descriptor& descriptor)
    : SingleSwitchProgramInstance(descriptor)
{
    PA_ADD_OPTION(START_IN_GRIP_MENU);
}

void TurboA::program(SingleSwitchProgramEnvironment& env){
    if (START_IN_GRIP_MENU){
        grip_menu_connect_go_home(env.console);
        PokemonSwSh::resume_game_back_out(env.console, ConsoleSettings::instance().TOLERATE_SYSTEM_UPDATE_MENU_FAST, 200);
    }

    while (true){
        pbf_press_button(env.console, BUTTON_A, 5, 5);
    }
}


}
}


/*  Loto Farmer
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "Common/Cpp/PrettyPrint.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_Device.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_PushButtons.h"
#include "PokemonSwSh/PokemonSwSh_Settings.h"
#include "PokemonSwSh/Commands/PokemonSwSh_Commands_DateSpam.h"
#include "PokemonSwSh_DateSpam-LotoFarmer.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSwSh{


LotoFarmer_Descriptor::LotoFarmer_Descriptor()
    : RunnableSwitchProgramDescriptor(
        "PokemonSwSh:LotoFarmer",
        STRING_POKEMON + " SwSh", "Date Spam - Loto Farmer",
        "ComputerControl/blob/master/Wiki/Programs/PokemonSwSh/DateSpam-LotoFarmer.md",
        "Farm the Loto ID.",
        FeedbackType::NONE, false,
        PABotBaseLevel::PABOTBASE_12KB
    )
{}



LotoFarmer::LotoFarmer(const LotoFarmer_Descriptor& descriptor)
    : SingleSwitchProgramInstance(descriptor)
    , SKIPS(
        "<b>Number of Loto Attempts:</b>",
        100000
    )
    , MASH_B_DURATION(
        "<b>Mash B for this long to exit the dialog:</b><br>(Some languages like German need to increase this.)",
        "9 * TICKS_PER_SECOND"
    )
{
    PA_ADD_OPTION(START_IN_GRIP_MENU);
    PA_ADD_OPTION(SKIPS);
    PA_ADD_OPTION(MASH_B_DURATION);
}

void LotoFarmer::program(SingleSwitchProgramEnvironment& env){
    if (START_IN_GRIP_MENU){
        grip_menu_connect_go_home(env.console);
    }else{
        pbf_press_button(env.console, BUTTON_B, 5, 5);
        pbf_press_button(env.console, BUTTON_HOME, 10, GameSettings::instance().GAME_TO_HOME_DELAY_FAST);
    }

    uint8_t year = MAX_YEAR;
    for (uint32_t c = 0; c < SKIPS; c++){
        env.log("Fetch Attempts: " + tostr_u_commas(c));
        home_roll_date_enter_game_autorollback(env.console, &year);
        pbf_mash_button(env.console, BUTTON_B, 90);

        pbf_press_button(env.console, BUTTON_A, 10, 70);
        pbf_press_button(env.console, BUTTON_B, 10, 70);
        pbf_press_dpad(env.console, DPAD_DOWN, 10, 5);
        pbf_mash_button(env.console, BUTTON_ZL, 490);
        pbf_mash_button(env.console, BUTTON_B, MASH_B_DURATION);

        //  Tap HOME and quickly spam B. The B spamming ensures that we don't
        //  accidentally update the system if the system update window pops up.
        pbf_press_button(env.console, BUTTON_HOME, 10, 5);
        pbf_mash_button(env.console, BUTTON_B, GameSettings::instance().GAME_TO_HOME_DELAY_FAST - 15);
    }
}


}
}
}

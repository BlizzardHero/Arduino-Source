/*  Daily Highlight Farmer
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "Common/Cpp/PrettyPrint.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_Device.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_PushButtons.h"
#include "PokemonSwSh/PokemonSwSh_Settings.h"
#include "PokemonSwSh/Commands/PokemonSwSh_Commands_DateSpam.h"
#include "PokemonSwSh_DateSpam-DailyHighlightFarmer.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSwSh{


DailyHighlightFarmer_Descriptor::DailyHighlightFarmer_Descriptor()
    : RunnableSwitchProgramDescriptor(
        "PokemonSwSh:DailyHighlightFarmer",
        STRING_POKEMON + " SwSh", "Date Spam - Daily Highlight Farmer",
        "ComputerControl/blob/master/Wiki/Programs/PokemonSwSh/DateSpam-DailyHighlightFarmer.md",
        "Farm the daily highlight watt trader in Crown Tundra.",
        FeedbackType::NONE, false,
        PABotBaseLevel::PABOTBASE_12KB
    )
{}



DailyHighlightFarmer::DailyHighlightFarmer(const DailyHighlightFarmer_Descriptor& descriptor)
    : SingleSwitchProgramInstance(descriptor)
    , SKIPS(
        "<b>Number of Purchase Attempts:</b>",
        100000
    )
    , SAVE_ITERATIONS(
        "<b>Save Every this Many Fetches:</b><br>(zero disables saving): ",
        0
    )
{
    PA_ADD_OPTION(START_IN_GRIP_MENU);
    PA_ADD_OPTION(SKIPS);
    PA_ADD_OPTION(SAVE_ITERATIONS);
}

void DailyHighlightFarmer::program(SingleSwitchProgramEnvironment& env){
    if (START_IN_GRIP_MENU){
        grip_menu_connect_go_home(env.console);
    }else{
        pbf_press_button(env.console, BUTTON_B, 5, 5);
        pbf_press_button(env.console, BUTTON_HOME, 10, GameSettings::instance().GAME_TO_HOME_DELAY_FAST);
    }

    uint8_t year = MAX_YEAR;
    uint16_t save_count = 0;
    for (uint32_t c = 0; c < SKIPS; c++){
        env.log("Fetch Attempts: " + tostr_u_commas(c));
        home_roll_date_enter_game_autorollback(env.console, &year);
        pbf_mash_button(env.console, BUTTON_B, 90);

        pbf_press_button(env.console, BUTTON_A, 10, 110);
        pbf_press_button(env.console, BUTTON_ZL, 10, 40);
        pbf_press_dpad(env.console, DPAD_DOWN, 10, 10);
        pbf_mash_button(env.console, BUTTON_ZL, 400);
        pbf_mash_button(env.console, BUTTON_B, 700);

        if (SAVE_ITERATIONS != 0){
            save_count++;
            if (save_count >= SAVE_ITERATIONS){
                save_count = 0;
                pbf_mash_button(env.console, BUTTON_B, 2 * TICKS_PER_SECOND);
                pbf_press_button(env.console, BUTTON_X, 20, GameSettings::instance().OVERWORLD_TO_MENU_DELAY);
                pbf_press_button(env.console, BUTTON_R, 20, 2 * TICKS_PER_SECOND);
                pbf_press_button(env.console, BUTTON_ZL, 20, 3 * TICKS_PER_SECOND);
            }
        }

        //  Tap HOME and quickly spam B. The B spamming ensures that we don't
        //  accidentally update the system if the system update window pops up.
        pbf_press_button(env.console, BUTTON_HOME, 10, 5);
        pbf_mash_button(env.console, BUTTON_B, GameSettings::instance().GAME_TO_HOME_DELAY_FAST - 15);
    }
}


}
}
}


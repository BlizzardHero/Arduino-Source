/*  God Egg Duplication
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "Common/Cpp/PrettyPrint.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_Device.h"
#include "NintendoSwitch/NintendoSwitch_Settings.h"
#include "PokemonSwSh/Commands/PokemonSwSh_Commands_GameEntry.h"
#include "PokemonSwSh/Commands/PokemonSwSh_Commands_EggRoutines.h"
#include "PokemonSwSh_GodEggDuplication.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSwSh{


GodEggDuplication_Descriptor::GodEggDuplication_Descriptor()
    : RunnableSwitchProgramDescriptor(
        "PokemonSwSh:GodEggDuplication",
        STRING_POKEMON + " SwSh", "God Egg Duplication",
        "ComputerControl/blob/master/Wiki/Programs/PokemonSwSh/GodEggDuplication.md",
        "Mass duplicate " + STRING_POKEMON + " with the God Egg.",
        FeedbackType::NONE, false,
        PABotBaseLevel::PABOTBASE_31KB
    )
{}



GodEggDuplication::GodEggDuplication(const GodEggDuplication_Descriptor& descriptor)
    : SingleSwitchProgramInstance(descriptor)
    , MAX_FETCH_ATTEMPTS(
        "<b>Fetch this many times:</b><br>This puts a limit on how many eggs you can get so you don't make a mess of your boxes for fetching too many.",
        2000
    )
    , PARTY_ROUND_ROBIN(
        "<b>Party Round Robin:</b><br>Cycle through this many party members.",
        6, 1, 6
    )
{
    PA_ADD_OPTION(START_IN_GRIP_MENU);
    PA_ADD_OPTION(MAX_FETCH_ATTEMPTS);
    PA_ADD_OPTION(PARTY_ROUND_ROBIN);
}


void GodEggDuplication::collect_godegg(const BotBaseContext& context, uint8_t party_slot) const{
    pbf_wait(context, 50);
    ssf_press_button1(context, BUTTON_B, 100);
    ssf_press_button1(context, BUTTON_B, 100);
    pbf_wait(context, 225);

    //  "You received an Egg from the Nursery worker!"
    ssf_press_button1(context, BUTTON_B, 300);

    //  "Where do you want to send the Egg to?"
    ssf_press_button1(context, BUTTON_A, 100);

    //  (extra line of text for French)
    ssf_press_button1(context, BUTTON_B, 100);

    //  "Please select a Pokemon to swap from your party."
    ssf_press_button1(context, BUTTON_B, GameSettings::instance().MENU_TO_POKEMON_DELAY);

    //  Select the party member.
    for (uint8_t c = 0; c < party_slot; c++){
        ssf_press_dpad1(context, DPAD_DOWN, 10);
    }
    ssf_press_button1(context, BUTTON_A, 300);
    pbf_mash_button(context, BUTTON_B, 500);
}
void GodEggDuplication::run_program(SingleSwitchProgramEnvironment& env, uint16_t attempts) const{
    if (attempts == 0){
        return;
    }

    uint8_t items = PARTY_ROUND_ROBIN > 6 ? 6 : PARTY_ROUND_ROBIN;
    uint8_t party_slot = 0;

    uint16_t c = 0;

    //  1st Fetch: Get into position.
    {
        env.log("Fetch Attempts: " + tostr_u_commas(c));
        fly_home_collect_egg(env.console, true);
        collect_godegg(env.console, party_slot++);
        if (party_slot >= items){
            party_slot = 0;
        }

        c++;
        if (c >= attempts){
            return;
        }
    }

    //  Now we are in steady state.
    for (; c < attempts; c++){
        env.log("Fetch Attempts: " + tostr_u_commas(c));
        eggfetcher_loop(env.console);
        collect_egg(env.console);
        collect_godegg(env.console, party_slot++);
        if (party_slot >= items){
            party_slot = 0;
        }
    }
}

void GodEggDuplication::program(SingleSwitchProgramEnvironment& env){
    if (START_IN_GRIP_MENU){
        grip_menu_connect_go_home(env.console);
        resume_game_back_out(env.console, ConsoleSettings::instance().TOLERATE_SYSTEM_UPDATE_MENU_FAST, 400);
    }else{
        pbf_press_button(env.console, BUTTON_B, 5, 5);
    }

    run_program(env, MAX_FETCH_ATTEMPTS);
    ssf_press_button2(env.console, BUTTON_HOME, GameSettings::instance().GAME_TO_HOME_DELAY_SAFE, 10);
}



}
}
}


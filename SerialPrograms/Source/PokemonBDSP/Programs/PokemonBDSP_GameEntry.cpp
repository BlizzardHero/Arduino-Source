/*  Game Entry
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "CommonFramework/Tools/ErrorDumper.h"
#include "CommonFramework/InferenceInfra/InferenceRoutines.h"
#include "CommonFramework/Inference/BlackScreenDetector.h"
#include "NintendoSwitch/NintendoSwitch_Settings.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_PushButtons.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_Routines.h"
#include "NintendoSwitch/Programs/NintendoSwitch_GameEntry.h"
#include "PokemonBDSP/PokemonBDSP_Settings.h"
#include "PokemonBDSP_GameEntry.h"

#include <iostream>
using std::cout;
using std::endl;

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonBDSP{


bool gamemenu_to_ingame(
    ProgramEnvironment& env, ConsoleHandle& console,
    uint16_t mash_duration, uint16_t enter_game_timeout
){
    console.log("Mashing A to enter game...");
    BlackScreenOverWatcher detector(COLOR_RED, {0.2, 0.2, 0.6, 0.6});
    pbf_mash_button(console, BUTTON_ZL, mash_duration);
    console.botbase().wait_for_all_requests();
    console.log("Waiting to enter game...");
    int ret = wait_until(
        env, console,
        std::chrono::milliseconds(enter_game_timeout * (1000 / TICKS_PER_SECOND)),
        { &detector }
    );
    if (ret == 0){
        console.log("Entered game!");
        return true;
    }else{
        console.log("Timed out waiting to enter game.", COLOR_RED);
        return false;
    }
}
bool openedgame_to_ingame(
    ProgramEnvironment& env, ConsoleHandle& console,
    uint16_t load_game_timeout,
    uint16_t mash_duration, uint16_t enter_game_timeout,
    uint16_t post_wait_time
){
    bool ok = true;
    ok &= openedgame_to_gamemenu(env, console, load_game_timeout);
    ok &= gamemenu_to_ingame(env, console, mash_duration, enter_game_timeout);
    if (!ok){
        dump_image(env.logger(), env.program_info(), "StartGame", console.video().snapshot());
    }
    console.log("Entered game! Waiting out grace period.");
    pbf_wait(console, post_wait_time);
    console.botbase().wait_for_all_requests();
    return ok;
}




bool reset_game_from_home(
    ProgramEnvironment& env, ConsoleHandle& console,
    bool tolerate_update_menu,
    uint16_t post_wait_time
){
    if (ConsoleSettings::instance().START_GAME_REQUIRES_INTERNET || tolerate_update_menu){
        close_game(console);
        open_game_from_home(
            env, console,
            tolerate_update_menu,
            0, 0,
            GameSettings::instance().START_GAME_MASH
        );
    }else{
        pbf_press_button(console, BUTTON_X, 50, 0);
        pbf_mash_button(console, BUTTON_A, GameSettings::instance().START_GAME_MASH);
    }
    bool ret = openedgame_to_ingame(
        env, console,
        GameSettings::instance().START_GAME_WAIT,
        GameSettings::instance().ENTER_GAME_MASH,
        GameSettings::instance().ENTER_GAME_WAIT,
        post_wait_time
    );
    console.botbase().wait_for_all_requests();
    return ret;
}



}
}
}

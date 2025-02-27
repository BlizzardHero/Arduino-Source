/*  EggHelpers
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_PokemonSwSh_EggHelpers_H
#define PokemonAutomation_PokemonSwSh_EggHelpers_H

#include "Common/Compiler.h"
#include "PokemonSwSh/PokemonSwSh_Settings.h"
#include "NintendoSwitch/FixedInterval.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSwSh{

#define GO_TO_LADY_DURATION         51
#define TRAVEL_RIGHT_DURATION       300
#define END_BATCH_MASH_B_DURATION   (20 * TICKS_PER_SECOND)


//  Collect egg.
static void collect_egg(const BotBaseContext& context){
    ssf_press_button1(context, BUTTON_A, 120);
    if (GameSettings::instance().EGG_FETCH_EXTRA_LINE){
        ssf_press_button1(context, BUTTON_A, 120);
    }
    ssf_press_button1(context, BUTTON_A, 10);
}
static void collect_egg_mash_out(const BotBaseContext& context, bool deposit_automatically){
    uint16_t FETCH_EGG_MASH_DELAY = GameSettings::instance().FETCH_EGG_MASH_DELAY;
    pbf_mash_button(
        context,
        BUTTON_B,
        deposit_automatically
            ? FETCH_EGG_MASH_DELAY
            : FETCH_EGG_MASH_DELAY + 240
    );
}


//  Fly Home: Used by everything.
static void fly_home(const BotBaseContext& context, char from_overworld){
    if (from_overworld){
        ssf_press_button2(context, BUTTON_X, GameSettings::instance().OVERWORLD_TO_MENU_DELAY, 20);
    }
    ssf_press_button2(context, BUTTON_A, 350, 10);
    ssf_press_dpad2(context, DPAD_UP_RIGHT, 25, 5);
    pbf_mash_button(context, BUTTON_A, 480);
}
static void fly_home_goto_lady(const BotBaseContext& context, char from_overworld){
    fly_home(context, from_overworld);

    //  Go to lady.
    //  If you change this, you MUST update "GO_TO_LADY_DURATION".
    ssf_press_joystick2(context, true, STICK_MIN, STICK_CENTER, 16, 6);
    ssf_press_joystick2(context, true, STICK_CENTER, STICK_MIN, 90, 45);
}
static void fly_home_collect_egg(const BotBaseContext& context, char from_overworld){
    fly_home_goto_lady(context, from_overworld);
    collect_egg(context);
}



//  EggHatcher+EggCombined Helpers

#define EGG_BUTTON_HOLD_DELAY   10

static void menu_to_box(const BotBaseContext& context, bool from_map){
    uint16_t BOX_SCROLL_DELAY = GameSettings::instance().BOX_SCROLL_DELAY;
    if (from_map){
        ssf_press_dpad2(context, DPAD_UP, BOX_SCROLL_DELAY, EGG_BUTTON_HOLD_DELAY);
        ssf_press_dpad2(context, DPAD_RIGHT, BOX_SCROLL_DELAY, EGG_BUTTON_HOLD_DELAY);
    }
    ssf_press_button2(context, BUTTON_A, GameSettings::instance().MENU_TO_POKEMON_DELAY, EGG_BUTTON_HOLD_DELAY);
    ssf_press_button2(context, BUTTON_R, GameSettings::instance().POKEMON_TO_BOX_DELAY, EGG_BUTTON_HOLD_DELAY);
    ssf_press_dpad2(context, DPAD_LEFT, BOX_SCROLL_DELAY, EGG_BUTTON_HOLD_DELAY);
    ssf_press_dpad2(context, DPAD_DOWN, BOX_SCROLL_DELAY, EGG_BUTTON_HOLD_DELAY);
    ssf_press_button2(context, BUTTON_Y, 30, EGG_BUTTON_HOLD_DELAY);
    ssf_press_button2(context, BUTTON_Y, 30, EGG_BUTTON_HOLD_DELAY);
}
static void box_to_menu(const BotBaseContext& context){
    //  There are two states here which need to be merged:
    //      1.  The depositing column was empty. The party has been swapped and
    //          it's sitting in the box with no held pokemon.
    //      2.  The depositing column was not empty. The party swap failed and
    //          it's sitting in the box holding on the party pokemon.
    //
    //  Double press B quickly here to back out of the box.
    //  In state (1):   The 1st B will begin back out of the box. The 2nd B will
    //                  be swallowed by the animation.
    //  In state (2):   The 1st B will drop the party pokemon. The 2nd B will
    //                  back out of the box.

    uint16_t BOX_SCROLL_DELAY = GameSettings::instance().BOX_SCROLL_DELAY;

    ssf_press_button2(context, BUTTON_B, 20, EGG_BUTTON_HOLD_DELAY);
    ssf_press_button2(context, BUTTON_B, GameSettings::instance().BOX_TO_POKEMON_DELAY, EGG_BUTTON_HOLD_DELAY);

    //  Back out to menu.
    ssf_press_button2(context, BUTTON_B, GameSettings::instance().POKEMON_TO_MENU_DELAY, EGG_BUTTON_HOLD_DELAY);

    ssf_press_dpad2(context, DPAD_LEFT, BOX_SCROLL_DELAY, EGG_BUTTON_HOLD_DELAY);
    ssf_press_dpad2(context, DPAD_DOWN, BOX_SCROLL_DELAY, EGG_BUTTON_HOLD_DELAY);
}

static void party_to_column(const BotBaseContext& context, uint8_t column){
    uint16_t BOX_SCROLL_DELAY = GameSettings::instance().BOX_SCROLL_DELAY;

    ssf_press_dpad2(context, DPAD_UP, BOX_SCROLL_DELAY, EGG_BUTTON_HOLD_DELAY);
    column++;
    if (column <= 3){
        for (uint8_t c = 0; c != column; c++){
            ssf_press_dpad2(context, DPAD_RIGHT, BOX_SCROLL_DELAY, EGG_BUTTON_HOLD_DELAY);
        }
    }else{
        for (uint8_t c = 7; c != column; c--){
            ssf_press_dpad2(context, DPAD_LEFT, BOX_SCROLL_DELAY, EGG_BUTTON_HOLD_DELAY);
        }
    }
}
static void column_to_party(const BotBaseContext& context, uint8_t column){
    uint16_t BOX_SCROLL_DELAY = GameSettings::instance().BOX_SCROLL_DELAY;

    column++;
    if (column <= 3){
        for (uint8_t c = column; c != 0; c--){
            ssf_press_dpad2(context, DPAD_LEFT, BOX_SCROLL_DELAY, EGG_BUTTON_HOLD_DELAY);
        }
    }else{
        for (uint8_t c = column; c != 7; c++){
            ssf_press_dpad2(context, DPAD_RIGHT, BOX_SCROLL_DELAY, EGG_BUTTON_HOLD_DELAY);
        }
    }
    ssf_press_dpad2(context, DPAD_DOWN, BOX_SCROLL_DELAY, EGG_BUTTON_HOLD_DELAY);
}

static void pickup_column(const BotBaseContext& context, char party){
    uint16_t BOX_SCROLL_DELAY = GameSettings::instance().BOX_SCROLL_DELAY;

    ssf_press_button2(context, BUTTON_A, 20, EGG_BUTTON_HOLD_DELAY);
    if (party){
        ssf_press_dpad2(context, DPAD_UP, BOX_SCROLL_DELAY, EGG_BUTTON_HOLD_DELAY);
    }
    ssf_press_dpad2(context, DPAD_UP, BOX_SCROLL_DELAY, EGG_BUTTON_HOLD_DELAY);
    ssf_press_button2(context, BUTTON_A, GameSettings::instance().BOX_PICKUP_DROP_DELAY, EGG_BUTTON_HOLD_DELAY);
}




}
}
}
#endif

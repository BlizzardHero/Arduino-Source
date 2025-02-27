/*  Egg Super-Combined 2
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "NintendoSwitch/Commands/NintendoSwitch_Commands_Device.h"
#include "PokemonSwSh/Programs/ReleaseHelpers.h"
#include "PokemonSwSh_EggHelpers.h"
#include "PokemonSwSh_EggCombinedShared.h"
#include "PokemonSwSh_EggSuperCombined2.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSwSh{


EggSuperCombined2_Descriptor::EggSuperCombined2_Descriptor()
    : RunnableSwitchProgramDescriptor(
        "PokemonSwSh:EggSuperCombined2",
        STRING_POKEMON + " SwSh", "Egg Super-Combined 2",
        "ComputerControl/blob/master/Wiki/Programs/PokemonSwSh/EggSuperCombined2.md",
        "Fetch and hatch eggs at the same time. (Fastest - 1700 eggs/day for 5120-step)",
        FeedbackType::NONE, false,
        PABotBaseLevel::PABOTBASE_31KB
    )
{}



EggSuperCombined2::EggSuperCombined2(const EggSuperCombined2_Descriptor& descriptor)
    : SingleSwitchProgramInstance(descriptor)
    , BOXES_TO_RELEASE(
        "<b>Boxes to Release:</b><br>Start by releasing this many boxes.",
        2, 0, 32
    )
    , BOXES_TO_SKIP(
        "<b>Boxes to Skip:</b><br>Then skip this many boxes.",
        1, 0, 32
    )
    , BOXES_TO_HATCH(
        "<b>Boxes to Hatch:</b>",
        31, 0, 32
    )
    , FETCHES_PER_BATCH(
        "<b>Fetches per Batch:</b><br>For each batch of eggs, attempt this many egg fetches.",
        6.0, 0, 7
    )
    , m_advanced_options(
        "<font size=4><b>Advanced Options:</b> You should not need to touch anything below here.</font>"
    )
    , SAFETY_TIME(
        "<b>Safety Time:</b><br>Additional time added to the spinning.",
        "8 * TICKS_PER_SECOND"
    )
    , EARLY_HATCH_SAFETY(
        "<b>Safety Time:</b><br>Additional time added to the spinning.",
        "5 * TICKS_PER_SECOND"
    )
    , HATCH_DELAY(
        "<b>Hatch Delay:</b><br>Total animation time for hatching 5 eggs when there are no shinies.",
        "88 * TICKS_PER_SECOND"
    )
{
    PA_ADD_OPTION(START_IN_GRIP_MENU);
    PA_ADD_OPTION(TOUCH_DATE_INTERVAL);

    PA_ADD_OPTION(BOXES_TO_RELEASE);
    PA_ADD_OPTION(BOXES_TO_SKIP);
    PA_ADD_OPTION(BOXES_TO_HATCH);
    PA_ADD_OPTION(STEPS_TO_HATCH);
    PA_ADD_OPTION(FETCHES_PER_BATCH);
    PA_ADD_STATIC(m_advanced_options);
    PA_ADD_OPTION(SAFETY_TIME);
    PA_ADD_OPTION(EARLY_HATCH_SAFETY);
    PA_ADD_OPTION(HATCH_DELAY);
}

void EggSuperCombined2::program(SingleSwitchProgramEnvironment& env){
    EggCombinedSession session{
        .BOXES_TO_HATCH = BOXES_TO_HATCH,
        .STEPS_TO_HATCH = STEPS_TO_HATCH,
        .FETCHES_PER_BATCH = (float)FETCHES_PER_BATCH,
        .SAFETY_TIME = SAFETY_TIME,
        .EARLY_HATCH_SAFETY = EARLY_HATCH_SAFETY,
        .HATCH_DELAY = HATCH_DELAY,
        .TOUCH_DATE_INTERVAL = TOUCH_DATE_INTERVAL,
    };

    if (START_IN_GRIP_MENU){
        grip_menu_connect_go_home(env.console);
        resume_game_back_out(env.console, ConsoleSettings::instance().TOLERATE_SYSTEM_UPDATE_MENU_FAST, 400);
    }else{
        pbf_press_button(env.console, BUTTON_B, 5, 5);
    }

    //  Mass Release
    ssf_press_button2(env.console, BUTTON_X, GameSettings::instance().OVERWORLD_TO_MENU_DELAY, 20);
    ssf_press_button1(env.console, BUTTON_A, 200);
    ssf_press_button1(env.console, BUTTON_R, 250);
    release_boxes(env.console, BOXES_TO_RELEASE, GameSettings::instance().BOX_SCROLL_DELAY, GameSettings::instance().BOX_CHANGE_DELAY);

    //  Skip Boxes
    for (uint8_t c = 0; c <= BOXES_TO_SKIP; c++){
        ssf_press_button1(env.console, BUTTON_R, 60);
    }
    pbf_mash_button(env.console, BUTTON_B, 600);

    session.eggcombined2_body(env);
}



}
}
}


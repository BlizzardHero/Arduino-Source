/*  Basic Pokemon Catcher
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "Common/Cpp/Exceptions.h"
#include "CommonFramework/Globals.h"
#include "CommonFramework/Tools/InterruptableCommands.h"
#include "CommonFramework/InferenceInfra/InferenceRoutines.h"
// #include "CommonFramework/Inference/BlackScreenDetector.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_PushButtons.h"
#include "PokemonBDSP/Inference/PokemonBDSP_ReceivePokemonDetector.h"
#include "PokemonBDSP/Inference/Battles/PokemonBDSP_ExperienceGainDetector.h"
#include "PokemonBDSP/Inference/Battles/PokemonBDSP_BattleMenuDetector.h"
#include "PokemonBDSP/Inference/Battles/PokemonBDSP_EndBattleDetector.h"
#include "PokemonBDSP_BasicCatcher.h"


namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonBDSP{


//  Returns the # of slots scrolled. Returns -1 if not found.
int move_to_ball(
    const BattleBallReader& reader,
    ConsoleHandle& console,
    const std::string& ball_slug,
    bool forward, int attempts, uint16_t delay
){
    QImage frame = console.video().snapshot();
    std::string first_ball = reader.read_ball(frame);
    if (first_ball == ball_slug){
        return 0;
    }

    size_t repeat_counter = 0;
    for (int c = 1; c < attempts; c++){
        pbf_press_dpad(console, forward ? DPAD_RIGHT : DPAD_LEFT, 10, delay);
        console.botbase().wait_for_all_requests();
        frame = console.video().snapshot();
        std::string current_ball = reader.read_ball(frame);
        if (current_ball == ball_slug){
            return c;
        }
        if (current_ball == first_ball){
            repeat_counter++;
            if (repeat_counter == 3){
                return -1;
            }
        }
    }
    return -1;
}


//  Returns the quantity of the ball.
//  Returns -1 if unable to read.
int16_t move_to_ball(
    const BattleBallReader& reader,
    ConsoleHandle& console,
    const std::string& ball_slug
){
    //  Search forward at high speed.
    int ret = move_to_ball(reader, console, ball_slug, true, 50, 30);
    if (ret < 0){
        return 0;
    }
    if (ret == 0){
        uint16_t quantity = reader.read_quantity(console.video().snapshot());
        return quantity == 0 ? -1 : quantity;
    }

    //  Wait a second to let the video catch up.
    pbf_wait(console, TICKS_PER_SECOND);
    console.botbase().wait_for_all_requests();

    //  Now try again in reverse at a lower speed in case we overshot.
    //  This will return immediately if we got it right the first time.
    ret = move_to_ball(reader, console, ball_slug, false, 5, TICKS_PER_SECOND);
    if (ret < 0){
        return 0;
    }
    if (ret > 0){
        console.log("BasicCatcher: Fast ball scrolling overshot by " +
            std::to_string(ret) + " slot(s).", COLOR_RED);
    }
    uint16_t quantity = reader.read_quantity(console.video().snapshot());
    return quantity == 0 ? -1 : quantity;
}


CatchResults throw_balls(
    ProgramEnvironment& env,
    ConsoleHandle& console,
    Language language,
    const std::string& ball_slug
){
    uint16_t balls_used = 0;
    while (true){
        // Test code for checking catch outcome handling: if the wild pokemon fainted:
// #define TEST_WILD_POKEMON_FAINTED
#ifdef TEST_WILD_POKEMON_FAINTED
        pbf_mash_button(console, BUTTON_ZL, TICKS_PER_SECOND);
        console.botbase().wait_for_all_requests();
        if (0)
#endif
        {
            BattleBallReader reader(console, language);

            pbf_press_button(console, BUTTON_X, 20, 105);
            console.botbase().wait_for_all_requests();

            const int16_t num_balls = move_to_ball(reader, console, ball_slug);
            if (num_balls < 0){
                console.log("BasicCatcher: Unable to read quantity of ball " + ball_slug + ".");
            }
            if (num_balls == 0){
                console.log("BasicCatcher: No ball " + ball_slug +
                    " found in bag or used them all during catching.");
                return {CatchResult::OUT_OF_BALLS, balls_used};
            }

            console.log("BasicCatcher: Found " + ball_slug + " with amount " + 
                std::to_string(num_balls));
            pbf_mash_button(console, BUTTON_ZL, 125);
            console.botbase().wait_for_all_requests();
        }
        balls_used++;

        auto start = std::chrono::system_clock::now();

        BattleMenuWatcher menu_detector(BattleType::STANDARD);
        ExperienceGainWatcher experience_detector;
        SelectionArrowFinder own_fainted_detector(console, {0.18, 0.64, 0.46, 0.3}, COLOR_YELLOW);
        int result = wait_until(
            env, console,
            std::chrono::seconds(60),
            {
                &menu_detector,
                &experience_detector,
                &own_fainted_detector,
            }
        );
        switch (result){
        case 0:
            if (std::chrono::system_clock::now() < start + std::chrono::seconds(5)){
                return {CatchResult::CANNOT_THROW_BALL, balls_used};
            }
            env.log("BasicCatcher: Failed to catch.", COLOR_ORANGE);
            continue;
        case 1:
            env.log("BasicCatcher: End of battle detected.", COLOR_PURPLE);
            // It's actually fainted or caught. The logic to find out which one
            // is in basic_catcher().
            return {CatchResult::POKEMON_FAINTED, balls_used};
        case 2:
            return {CatchResult::OWN_FAINTED, balls_used};
        default:
            return {CatchResult::TIMEOUT, balls_used};
        }
    }
}


CatchResults basic_catcher(
    ProgramEnvironment& env,
    ConsoleHandle& console,
    Language language,
    const std::string& ball_slug
){
    console.botbase().wait_for_all_requests();
    env.log("Attempting to catch with: " + ball_slug);

    CatchResults results = throw_balls(env, console, language, ball_slug);
    const QString s = (results.balls_used <= 1 ? "" : "s");
    const QString pokeball_str = QString::number(results.balls_used) + " " +
        QString(ball_slug.c_str()) + s;

    switch (results.result){
    case CatchResult::OUT_OF_BALLS:
        env.log("BasicCatcher: Out of balls after throwing " + pokeball_str, COLOR_RED);
        return results;
    case CatchResult::CANNOT_THROW_BALL:
        env.log("BasicCatcher: cannot throw ball for some reason.", COLOR_RED);
        return results;
    case CatchResult::OWN_FAINTED:
        env.log("BasicCatcher: own pokemon fainted after throwing " + pokeball_str, COLOR_RED);
        return results;
    case CatchResult::TIMEOUT:
        env.log("BasicCatcher: time out.", COLOR_RED);
        return results;
    default:;
    }

    //  Need to distinguish between caught or faint.
    //  Where there is no pokemon evolving, the order of events in BDSP is:
    //  exp screen -> lvl up and learn new move dialog -> new pokemon received screen if caught
    //  -> black screen -> return to overworld
    //  Wthere there is pokemon evolving, the order becomes:
    //  exp screen -> lvl up and learn new move dialog -> black screen -> pokemon evolving
    //  -> new pokemon received screen if caught.
    //  In this basic_catcher() we don't handle pokemon evolving.

    //  First, default the result to be fainted.
    results.result = CatchResult::POKEMON_FAINTED;
    size_t num_learned_moves = 0;
    while (true){
        console.botbase().wait_for_all_requests();
        //  Wait for end of battle.
        // BlackScreenOverWatcher black_screen_detector;
        EndBattleWatcher end_battle;
        //  Look for a pokemon learning a new move.
        SelectionArrowFinder learn_move(console, {0.50, 0.62, 0.40, 0.18}, COLOR_YELLOW);
        //  Look for the pokemon caught screen.
        ReceivePokemonDetector caught_detector;
        int ret = run_until(
            env, console,
            [=](const BotBaseContext& context){
                pbf_mash_button(context, BUTTON_B, 120 * TICKS_PER_SECOND);
            },
            {
                &end_battle,
                &caught_detector,
                &learn_move,
            }
        );
        switch (ret){
        case 0:
            if (results.result == CatchResult::POKEMON_FAINTED){
                env.log("BasicCatcher: The wild " + STRING_POKEMON + " fainted after " +
                    pokeball_str, COLOR_RED);
            }
            env.log("BasicCatcher: Battle finished!", COLOR_BLUE);
            pbf_wait(console, TICKS_PER_SECOND);
            console.botbase().wait_for_all_requests();
            return results;
        case 1:
            if (results.result == CatchResult::POKEMON_CAUGHT){
                throw OperationFailedException(console, "BasicCatcher: Found receive pokemon screen two times.");
            }
            env.log("BasicCatcher: The wild " + STRING_POKEMON + " was caught by " + pokeball_str, COLOR_BLUE);
            pbf_wait(console, 50);
            results.result = CatchResult::POKEMON_CAUGHT;
            break; //  Continue the loop.
        case 2:
            env.log("BasicCatcher: Detected move learn! Don't learn the new move.", COLOR_BLUE);
            num_learned_moves++;
            if (num_learned_moves == 100){
                throw OperationFailedException(console, "BasicCatcher: Learn new move attempts reach 100.");
            }
            pbf_move_right_joystick(console, 128, 255, 20, 105);
            pbf_press_button(console, BUTTON_ZL, 20, 105);
            break; //  Continue the loop.

        default:
            env.log("BasicCatcher: Timed out.", COLOR_RED);
            results.result = CatchResult::TIMEOUT;
            return results;
        }
    }
}


}
}
}

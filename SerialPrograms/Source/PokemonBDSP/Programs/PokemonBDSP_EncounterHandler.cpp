/*  Encounter Handler
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include <QtGlobal>
#include "Common/Cpp/Exceptions.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_PushButtons.h"
#include "Pokemon/Pokemon_Types.h"
#include "PokemonBDSP_BasicCatcher.h"
#include "PokemonBDSP_EncounterHandler.h"
#include "PokemonBDSP_GameNavigation.h"
#include "PokemonBDSP_RunFromBattle.h"

#include <iostream>
using std::cout;
using std::endl;

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonBDSP{



void take_video(const BotBaseContext& context){
    pbf_wait(context, 5 * TICKS_PER_SECOND);
    pbf_press_button(context, BUTTON_CAPTURE, 2 * TICKS_PER_SECOND, 5 * TICKS_PER_SECOND);
//    context->wait_for_all_requests();
}


StandardEncounterHandler::StandardEncounterHandler(
    ProgramEnvironment& env,
    ConsoleHandle& console,
    Language language,
    EncounterBotCommonOptions& settings,
    PokemonSwSh::ShinyHuntTracker& session_stats
)
    : m_env(env)
    , m_console(console)
    , m_language(language)
    , m_settings(settings)
    , m_session_stats(session_stats)
{}



std::vector<PokemonDetection> get_mon_list(StandardEncounterDetection& encounter){
    std::vector<PokemonDetection> mon_list;
    const PokemonDetection& left = encounter.pokemon_left();
    const PokemonDetection& right = encounter.pokemon_right();
    if (left.exists){
        mon_list.emplace_back(left);
    }
    if (right.exists){
        mon_list.emplace_back(right);
    }
    return mon_list;
}

void StandardEncounterHandler::run_away_due_to_error(uint16_t exit_battle_time){
    pbf_mash_button(m_console, BUTTON_B, 3 * TICKS_PER_SECOND);
    pbf_press_dpad(m_console, DPAD_DOWN, 3 * TICKS_PER_SECOND, 0);
    m_console.botbase().wait_for_all_requests();

    run_from_battle(m_env, m_console, exit_battle_time);
}

std::vector<EncounterResult> StandardEncounterHandler::results(StandardEncounterDetection& encounter){
    std::vector<EncounterResult> ret;
    const PokemonDetection& left = encounter.pokemon_left();
    const PokemonDetection& right = encounter.pokemon_right();
    if (left.exists){
        ret.emplace_back(EncounterResult{left.slugs, encounter.left_shininess()});
    }
    if (right.exists){
        ret.emplace_back(EncounterResult{right.slugs, encounter.right_shininess()});
    }
    return ret;
}
void StandardEncounterHandler::update_frequencies(StandardEncounterDetection& encounter){
    const PokemonDetection& left = encounter.pokemon_left();
    const PokemonDetection& right = encounter.pokemon_right();
    if (!left.detection_enabled && !right.detection_enabled){
        return;
    }
    if (left.exists){
        m_frequencies += left.slugs;
    }
    if (right.exists){
        m_frequencies += right.slugs;
    }
    if (left.exists || right.exists){
        m_env.log(m_frequencies.dump_sorted_map("Encounter Stats:\n"));
    }
}


bool StandardEncounterHandler::handle_standard_encounter(const DoublesShinyDetection& result){
    if (result.shiny_type == ShinyType::UNKNOWN){
        m_console.log("Unable to determine result of battle.", COLOR_RED);
        m_session_stats.add_error();
        m_consecutive_failures++;
        if (m_consecutive_failures >= 3){
            throw OperationFailedException(m_console, "3 consecutive failed encounter detections.");
        }
        return false;
    }
    m_consecutive_failures = 0;

    StandardEncounterDetection encounter(
        m_env, m_console,
        m_language,
        m_settings.FILTER,
        result
    );

    m_session_stats += result.shiny_type;
    if (encounter.is_double_battle()){
        bool left = is_confirmed_shiny(encounter.left_shininess());
        bool right = is_confirmed_shiny(encounter.right_shininess());
        if (left && right){
            m_session_stats += ShinyType::UNKNOWN_SHINY;
        }else{
            m_session_stats += ShinyType::NOT_SHINY;
        }
    }
    m_env.update_stats();

    if (result.shiny_type == ShinyType::UNKNOWN){
        pbf_mash_button(m_console, BUTTON_B, TICKS_PER_SECOND);
        return false;
    }

    bool enable_names = m_language != Language::None;
    std::vector<EncounterResult> encounter_results = results(encounter);

    update_frequencies(encounter);
    send_encounter_notification(
        m_console,
        m_settings.NOTIFICATION_NONSHINY,
        m_settings.NOTIFICATION_SHINY,
        m_env.program_info(),
        m_language != Language::None, is_likely_shiny(result.shiny_type),
        encounter_results,
        result.best_screenshot,
        &m_session_stats,
        enable_names ? &m_frequencies : nullptr
    );

    if (m_settings.VIDEO_ON_SHINY && encounter.has_shiny()){
        take_video(m_console);
    }

    return encounter.get_action().action == EncounterAction::StopProgram;
}
bool StandardEncounterHandler::handle_standard_encounter_end_battle(
    const DoublesShinyDetection& result,
    uint16_t exit_battle_time
){
    if (result.shiny_type == ShinyType::UNKNOWN){
        m_console.log("Unable to determine result of battle.", COLOR_RED);
        m_session_stats.add_error();
        m_consecutive_failures++;
        if (m_consecutive_failures >= 3){
            throw OperationFailedException(m_console, "3 consecutive failed encounter detections.");
        }
        return false;
    }
    m_consecutive_failures = 0;

    StandardEncounterDetection encounter(
        m_env, m_console,
        m_language,
        m_settings.FILTER,
        result
    );

    m_session_stats += result.shiny_type;
    if (encounter.is_double_battle()){
        bool left = is_confirmed_shiny(encounter.left_shininess());
        bool right = is_confirmed_shiny(encounter.right_shininess());
        if (left && right){
            m_session_stats += ShinyType::UNKNOWN_SHINY;
        }else{
            m_session_stats += ShinyType::NOT_SHINY;
        }
    }
    m_env.update_stats();

    if (m_settings.VIDEO_ON_SHINY && encounter.has_shiny()){
        take_video(m_console);
    }

    bool enable_names = m_language != Language::None;
    std::vector<EncounterResult> encounter_results = results(encounter);

    update_frequencies(encounter);
    send_encounter_notification(
        m_console,
        m_settings.NOTIFICATION_NONSHINY,
        m_settings.NOTIFICATION_SHINY,
        m_env.program_info(),
        enable_names, is_likely_shiny(result.shiny_type),
        encounter_results,
        result.best_screenshot,
        &m_session_stats,
        enable_names ? &m_frequencies : nullptr
    );

    EncounterActionFull action = encounter.get_action();
    switch (action.action){
    case EncounterAction::StopProgram:
        return true;
    case EncounterAction::RunAway:
        //  Fast run-away sequence to save time.
        pbf_press_dpad(m_console, DPAD_UP, 10, 0);
        m_console.botbase().wait_for_all_requests();

        run_from_battle(m_env, m_console, exit_battle_time);
        return false;

    case EncounterAction::ThrowBalls:
    case EncounterAction::ThrowBallsAndSave:{
        CatchResults catch_result = basic_catcher(m_env, m_console, m_language, action.pokeball_slug);
        switch (catch_result.result){
        case CatchResult::POKEMON_CAUGHT:
            m_session_stats.add_caught();
            m_env.update_stats();
            if (action.action == EncounterAction::ThrowBallsAndSave){
                //  Save the game
                save_game(m_env, m_console);
            }
            break;
        case CatchResult::POKEMON_FAINTED:
            pbf_mash_button(m_console, BUTTON_B, 2 * TICKS_PER_SECOND);
            break;
        case CatchResult::OWN_FAINTED:
            throw OperationFailedException(
                m_console,
                "Your " + STRING_POKEMON.toStdString() + " fainted after " + std::to_string(catch_result.balls_used) + " balls."
            );
        case CatchResult::OUT_OF_BALLS:
            throw OperationFailedException(
                m_console,
                "Unable to find the desired ball after throwing " + std::to_string(catch_result.balls_used) + " of them. Did you run out?"
            );
        case CatchResult::CANNOT_THROW_BALL:
            throw OperationFailedException(
                m_console,
                "Unable to throw ball. Is the " + STRING_POKEMON.toStdString() + " semi-invulnerable?"
            );
        case CatchResult::TIMEOUT:
            throw OperationFailedException(
                m_console,
                "Program has timed out. Did your lead " + STRING_POKEMON.toStdString() + " faint?"
            );
        }
        send_catch_notification(
            m_console,
            m_settings.NOTIFICATION_CATCH_SUCCESS,
            m_settings.NOTIFICATION_CATCH_FAILED,
            m_env.program_info(),
            &encounter_results[0].slug_candidates,
            action.pokeball_slug,
            catch_result.balls_used,
            catch_result.result == CatchResult::POKEMON_CAUGHT
        );
        return false;
    }
    default:
        return true;
    }

    return false;
}




}
}
}

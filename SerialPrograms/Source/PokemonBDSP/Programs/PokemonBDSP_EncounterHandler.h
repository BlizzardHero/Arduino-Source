/*  Encounter Handler
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_PokemonBDSP_EncounterHandler_H
#define PokemonAutomation_PokemonBDSP_EncounterHandler_H

#include "CommonFramework/Tools/ProgramEnvironment.h"
#include "CommonFramework/Tools/ConsoleHandle.h"
#include "Pokemon/Pokemon_Notification.h"
#include "Pokemon/Options/Pokemon_EncounterBotOptions.h"
#include "PokemonSwSh/ShinyHuntTracker.h"
#include "PokemonBDSP/Options/PokemonBDSP_EncounterBotCommon.h"
#include "PokemonBDSP/Inference/ShinyDetection/PokemonBDSP_ShinyEncounterDetector.h"
#include "PokemonBDSP/Programs/PokemonBDSP_EncounterDetection.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonBDSP{

//  Handle wild encounters using feedback.
class StandardEncounterHandler{
public:
    StandardEncounterHandler(
        ProgramEnvironment& env,
        ConsoleHandle& console,
        Language language,
        EncounterBotCommonOptions& settings,
        PokemonSwSh::ShinyHuntTracker& session_stats
    );

    //  Run away sequence for unexpected battle.
    //  Must be called during a battle. Hit "Run" and go through the dialogue until
    //  end of battle is detected.
    //  The pokemon must be able to run successfully with no trapping or run away failure.
    //
    //  exit_battle_time: number of ticks to wait for battle ends after pressing "Run" button.
    //    If end of battle not detected in time, log the error but don't throw exception.
    void run_away_due_to_error(uint16_t exit_battle_time);

    //  Use shiny detection result and inference of pokemon species to determine whether
    //  to stop the program according to the user setting.
    //  Return true if program should stop.
    bool handle_standard_encounter(const DoublesShinyDetection& result);
    //  Use shiny detection result and inference of pokemon species to determine whether
    //  to stop the program, run away from battle or catch the pokemon according to the user
    //  setting.
    //  Return true if program should stop.
    bool handle_standard_encounter_end_battle(const DoublesShinyDetection& result, uint16_t exit_battle_time);


private:
    std::vector<EncounterResult> results(StandardEncounterDetection& encounter);
    // Record the types of pokemon encountered into encounter stats.
    // The stats are then logged.
    void update_frequencies(StandardEncounterDetection& encounter);

private:
    ProgramEnvironment& m_env;
    ConsoleHandle& m_console;
    const Language m_language;
    EncounterBotCommonOptions& m_settings;

    EncounterFrequencies m_frequencies;
    PokemonSwSh::ShinyHuntTracker& m_session_stats;
    size_t m_consecutive_failures = 0;
};


void take_video(const BotBaseContext& context);
void run_away(
    ProgramEnvironment& env,
    ConsoleHandle& console,
    uint16_t exit_battle_time
);



}
}
}
#endif

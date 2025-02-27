/*  Encounter Detection
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_PokemonBDSP_EncounterTracker_H
#define PokemonAutomation_PokemonBDSP_EncounterTracker_H

#include "CommonFramework/Language.h"
#include "CommonFramework/Tools/ConsoleHandle.h"
#include "CommonFramework/Tools/ProgramEnvironment.h"
#include "Pokemon/Pokemon_EncounterStats.h"
#include "PokemonSwSh/ShinyHuntTracker.h"
#include "PokemonBDSP/Options/EncounterFilter/PokemonBDSP_EncounterFilterOption.h"
#include "PokemonBDSP/Inference/ShinyDetection/PokemonBDSP_ShinyEncounterDetector.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonBDSP{


struct PokemonDetection{
    bool exists = false;
    bool detection_enabled = false;
    std::set<std::string> slugs;
};


class StandardEncounterDetection{
public:
    StandardEncounterDetection(
        ProgramEnvironment& env,
        ConsoleHandle& console,
        Language language,
        const EncounterFilterOption& filter,
        const DoublesShinyDetection& shininess,
        std::chrono::milliseconds read_name_delay = std::chrono::milliseconds(500)
    );

    bool is_double_battle() const{ return m_double_battle; }
    const PokemonDetection& pokemon_left() const;
    const PokemonDetection& pokemon_right() const;

    bool has_shiny() const;
    ShinyType overall_shininess() const{ return m_shininess.shiny_type; }
    ShinyType left_shininess() const{ return m_shininess_left; }
    ShinyType right_shininess() const{ return m_shininess_right; }

    EncounterActionFull get_action();

private:
    std::set<std::string> read_name(const QImage& screen, const ImageFloatBox& box);
#if 0
    void run_overrides(
        std::vector<std::pair<EncounterAction, std::string>>& actions,
        const std::vector<EncounterFilterOverride>& overrides,
        const PokemonDetection& pokemon, ShinyType side_shiny
    ) const;
#endif
    bool run_overrides(
        EncounterActionFull& action,
        const std::vector<EncounterFilterOverride>& overrides,
        const PokemonDetection& pokemon, ShinyType side_shiny
    ) const;
    EncounterActionFull get_action_singles();
    EncounterActionFull get_action_doubles();

private:
    ConsoleHandle& m_console;

    const Language m_language;

    const EncounterFilterOption& m_filter;
    const DoublesShinyDetection& m_shininess;

    bool m_double_battle = false;
    ShinyType m_shininess_left;
    ShinyType m_shininess_right;

    PokemonDetection m_pokemon_left;
    PokemonDetection m_pokemon_right;
};







}
}
}
#endif

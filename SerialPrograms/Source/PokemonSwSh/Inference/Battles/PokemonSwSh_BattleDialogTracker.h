/*  Encounter Dialog Tracker
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_PokemonSwSh_EncounterDialogTracker_H
#define PokemonAutomation_PokemonSwSh_EncounterDialogTracker_H

#include <chrono>
#include "CommonFramework/Logging/LoggerQt.h"
#include "CommonFramework/Tools/VideoFeed.h"
#include "CommonFramework/Tools/VideoOverlaySet.h"
#include "CommonFramework/Inference/VisualDetector.h"
#include "CommonFramework/InferenceInfra/VisualInferenceCallback.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSwSh{

enum class EncounterState{
    BEFORE_ANYTHING,
    WILD_ANIMATION,
    YOUR_ANIMATION,
    POST_ENTRY,
};


class EncounterDialogTracker : public VisualInferenceCallback{
public:
    EncounterDialogTracker(
        LoggerQt& logger,
        StaticScreenDetector& dialog_detector
    );

    bool dialog_on() const{ return m_dialog_on; }
    EncounterState encounter_state() const{ return m_state; }
    std::chrono::milliseconds wild_animation_duration() const{ return m_wild_animation_duration; }
    std::chrono::milliseconds your_animation_duration() const{ return m_your_animation_duration; }

    virtual void make_overlays(VideoOverlaySet& items) const override;
    virtual bool process_frame(
        const QImage& screen,
        std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now()
    ) override;
    void push_end(std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now());

private:
    LoggerQt& m_logger;
    StaticScreenDetector& m_dialog_detector;
    std::chrono::system_clock::time_point m_end_dialog;
    bool m_dialog_on;

    EncounterState m_state;
    std::chrono::milliseconds m_wild_animation_duration;
    std::chrono::milliseconds m_your_animation_duration;
};


}
}
}
#endif

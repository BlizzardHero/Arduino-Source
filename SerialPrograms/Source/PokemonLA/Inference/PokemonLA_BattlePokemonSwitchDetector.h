/*  Battle Pokemon Switch Detector
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 *  Detect the screen of switching to another pokemon during battle.
 */

#ifndef PokemonAutomation_PokemonLA_BattlePokemonSwitchDetector_H
#define PokemonAutomation_PokemonLA_BattlePokemonSwitchDetector_H

#include "CommonFramework/ImageTools/ImageBoxes.h"
#include "CommonFramework/InferenceInfra/VisualInferenceCallback.h"
#include "CommonFramework/Logging/LoggerQt.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonLA{

class BattlePokemonSwitchDetector : public VisualInferenceCallback{
public:
    BattlePokemonSwitchDetector(LoggerQt& logger, VideoOverlay& overlay, bool stop_on_detected);

    bool detected() const{
        return m_detected.load(std::memory_order_acquire);
    }

    virtual void make_overlays(VideoOverlaySet& items) const override;
    virtual bool process_frame(
        const QImage& frame,
        std::chrono::system_clock::time_point timestamp
    ) override;

private:
    bool m_stop_on_detected;
    std::atomic<bool> m_detected;

    // The white background of the current selected pokemon's name and types:
    ImageFloatBox m_white_bg_1;
    ImageFloatBox m_white_bg_2;
    ImageFloatBox m_white_bg_3;
    ImageFloatBox m_white_bg_4;

    // The dark blue background of "Ready to battle":
    ImageFloatBox m_ready_to_battle_bg_1;
    ImageFloatBox m_ready_to_battle_bg_2;
};


}
}
}
#endif

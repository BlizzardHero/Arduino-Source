/*  Dialog Detector
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *  
 *  Detect the battle menu, which has the option (A) battle, (B) run, 
 *  DPAD_UP for items and DPAD_DOWN to change pokemon.
 */

#ifndef PokemonAutomation_PokemonLA_BattleMenuDetector_H
#define PokemonAutomation_PokemonLA_BattleMenuDetector_H

#include "CommonFramework/ImageTools/ImageBoxes.h"
#include "CommonFramework/InferenceInfra/VisualInferenceCallback.h"
#include "PokemonLA/Inference/Objects/PokemonLA_ButtonDetector.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonLA{

class BattleMenuDetector : public VisualInferenceCallback{
public:
    BattleMenuDetector(LoggerQt& logger, VideoOverlay& overlay, bool stop_on_detected);

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
    // m_pokemon_stroke_bg_*: match the deep sky blue background of the player's pokemon stats
    // in the lower left part of the screen.
    ImageFloatBox m_pokemon_stroke_bg_left;
    ImageFloatBox m_pokemon_stroke_bg_right;

    ButtonDetector m_button_A_detector;
    // ButtonDetector m_button_B_detector;
};


}
}
}
#endif

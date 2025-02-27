/*  Dialog Detector
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_PokemonBDSP_BattleDialogDetector_H
#define PokemonAutomation_PokemonBDSP_BattleDialogDetector_H

#include "CommonFramework/Inference/VisualDetector.h"
#include "CommonFramework/InferenceInfra/VisualInferenceCallback.h"
#include "PokemonBDSP_SelectionArrow.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonBDSP{

// Detect short dialog boxes that are used in all kinds of ingame situations where texts are displayed.
// The only place the long dialog boxes appear is during pokemon battles. But after battle, the exp gain
// text is in a short dialog box.
class ShortDialogDetector : public StaticScreenDetector{
public:
    ShortDialogDetector(Color color = COLOR_RED);

    virtual void make_overlays(VideoOverlaySet& items) const override;
    virtual bool detect(const QImage& screen) const override;

private:
    Color m_color;
//    ImageFloatBox m_bottom;
    ImageFloatBox m_left_white;
    ImageFloatBox m_left;
    ImageFloatBox m_right_white;
    ImageFloatBox m_right;
};
class ShortDialogWatcher : public ShortDialogDetector, public VisualInferenceCallback{
public:
    ShortDialogWatcher(Color color = COLOR_RED);

    virtual void make_overlays(VideoOverlaySet& items) const override;
    virtual bool process_frame(
        const QImage& frame,
        std::chrono::system_clock::time_point timestamp
    ) override;
};



// Detect the long dialog boxes that only appear during pokemon battles.
// Note after battle, the exp gain text is in a short dialog box.
class BattleDialogDetector : public StaticScreenDetector{
public:
    BattleDialogDetector(Color color = COLOR_RED);

    virtual void make_overlays(VideoOverlaySet& items) const override;
    virtual bool detect(const QImage& screen) const override;

private:
    Color m_color;
    ImageFloatBox m_bottom;
    ImageFloatBox m_left_white;
    ImageFloatBox m_left;
    ImageFloatBox m_right;
};


class ShortDialogPromptDetector : public VisualInferenceCallback{
public:
    ShortDialogPromptDetector(
        VideoOverlay& overlay,
        const ImageFloatBox& box,
        Color color = COLOR_RED
    );

    virtual void make_overlays(VideoOverlaySet& items) const override;
    virtual bool process_frame(
        const QImage& frame,
        std::chrono::system_clock::time_point timestamp
    ) override;

private:
    ShortDialogDetector m_dialog;
    SelectionArrowFinder m_arrow;
};




}
}
}
#endif

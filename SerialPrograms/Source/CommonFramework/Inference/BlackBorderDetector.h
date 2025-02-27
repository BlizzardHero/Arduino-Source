/*  Black Screen Detector
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_CommonFramework_BlackBorderDetector_H
#define PokemonAutomation_CommonFramework_BlackBorderDetector_H

#include "CommonFramework/ImageTools/ImageBoxes.h"
#include "CommonFramework/Inference/VisualDetector.h"

namespace PokemonAutomation{


class BlackBorderDetector : public StaticScreenDetector{
public:
    BlackBorderDetector();

    virtual void make_overlays(VideoOverlaySet& items) const;
    virtual bool detect(const QImage& screen) const;

private:
    ImageFloatBox m_top;
    ImageFloatBox m_bottom;
    ImageFloatBox m_left;
    ImageFloatBox m_right;
//    ImageFloatBox m_body;
};



}
#endif

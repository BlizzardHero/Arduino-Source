/*  Battle Move Selection Detector
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include <QImage>
#include "CommonFramework/ImageTools/ImageStats.h"
#include "CommonFramework/ImageTools/SolidColorTest.h"
#include "CommonFramework/Tools/VideoOverlaySet.h"
#include "PokemonLA_BattleMoveSelectionDetector.h"

#include <iostream>
using std::cout;
using std::endl;

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonLA{


BattleMoveSelectionDetector::BattleMoveSelectionDetector(LoggerQt& logger, VideoOverlay& overlay, bool stop_on_detected)
    : VisualInferenceCallback("BattleMoveSelectionDetector")
    , m_stop_on_detected(stop_on_detected)
    , m_detected(false)
    , m_move_1_highlight(0.800, 0.6220, 0.02, 0.032)
    , m_move_2_highlight(0.779, 0.6875, 0.02, 0.032)
    , m_move_3_highlight(0.758, 0.7530, 0.02, 0.032)
    , m_move_4_highlight(0.737, 0.8185, 0.02, 0.032)
{}

void BattleMoveSelectionDetector::make_overlays(VideoOverlaySet& items) const{
    items.add(COLOR_BLUE, m_move_1_highlight);
    items.add(COLOR_BLUE, m_move_2_highlight);
    items.add(COLOR_BLUE, m_move_3_highlight);
    items.add(COLOR_BLUE, m_move_4_highlight);
}
bool BattleMoveSelectionDetector::process_frame(
    const QImage& frame,
    std::chrono::system_clock::time_point timestamp
){
    size_t highlighted = 0;

    const ImageStats move_1 = image_stats(extract_box(frame, m_move_1_highlight));
    highlighted += is_solid(move_1, {0.34,0.34,0.34}, 0.15);

    const ImageStats move_2 = image_stats(extract_box(frame, m_move_2_highlight));
    highlighted += is_solid(move_2, {0.34,0.34,0.34}, 0.15);
    if (highlighted > 1){
        m_detected.store(false, std::memory_order_release);
        return false;
    }
    
    const ImageStats move_3 = image_stats(extract_box(frame, m_move_3_highlight));
    highlighted += is_solid(move_3, {0.34,0.34,0.34}, 0.15);
    if (highlighted > 1){
        m_detected.store(false, std::memory_order_release);
        return false;
    }

    const ImageStats move_4 = image_stats(extract_box(frame, m_move_4_highlight));
    highlighted += is_solid(move_4, {0.34,0.34,0.34}, 0.15);

    bool detected = highlighted == 1;
    m_detected.store(detected, std::memory_order_release);

    return detected && m_stop_on_detected;
}



}
}
}

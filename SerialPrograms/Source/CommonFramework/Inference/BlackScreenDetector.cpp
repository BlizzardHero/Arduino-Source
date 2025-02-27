/*  Black Border Detector
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 *
 *      Returns true after a black screen has been detected and
 * the black screen has ended.
 *
 */

#include <QImage>
#include "Common/Compiler.h"
#include "CommonFramework/Tools/VideoOverlaySet.h"
#include "CommonFramework/ImageTools/SolidColorTest.h"
#include "BlackScreenDetector.h"

#include <iostream>
using std::cout;
using std::endl;

namespace PokemonAutomation{


BlackScreenDetector::BlackScreenDetector(
    Color color, const ImageFloatBox& box,
    double max_rgb_sum,
    double max_stddev_sum
)
    : m_color(color)
    , m_box(box)
    , m_max_rgb_sum(max_rgb_sum)
    , m_max_stddev_sum(max_stddev_sum)
{}

void BlackScreenDetector::make_overlays(VideoOverlaySet& items) const{
    items.add(m_color, m_box);
}
bool BlackScreenDetector::detect(const QImage& screen) const{
    return is_black(extract_box(screen, m_box), m_max_rgb_sum, m_max_stddev_sum);
}



BlackScreenWatcher::BlackScreenWatcher(
    Color color, const ImageFloatBox& box,
    double max_rgb_sum,
    double max_stddev_sum
)
    : BlackScreenDetector(color, box, max_rgb_sum, max_stddev_sum)
    , VisualInferenceCallback("BlackScreenWatcher")
{}
void BlackScreenWatcher::make_overlays(VideoOverlaySet& items) const{
    BlackScreenDetector::make_overlays(items);
}
bool BlackScreenWatcher::process_frame(
    const QImage& frame,
    std::chrono::system_clock::time_point timestamp
){
    return detect(frame);
}




BlackScreenOverWatcher::BlackScreenOverWatcher(
    Color color, const ImageFloatBox& box,
    double max_rgb_sum,
    double max_stddev_sum
)
    : VisualInferenceCallback("BlackScreenOverWatcher")
    , m_detector(color, box, max_rgb_sum, max_stddev_sum)
{}
void BlackScreenOverWatcher::make_overlays(VideoOverlaySet& items) const{
    m_detector.make_overlays(items);
}

bool BlackScreenOverWatcher::process_frame(
    const QImage& frame,
    std::chrono::system_clock::time_point timestamp
){
    return black_is_over(frame);
}
bool BlackScreenOverWatcher::black_is_over(const QImage& frame){
    if (m_detector.detect(frame)){
        m_has_been_black = true;
        return false;
    }
    return m_has_been_black;
}



}

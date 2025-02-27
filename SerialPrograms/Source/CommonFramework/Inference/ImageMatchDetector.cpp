/*  Image Match Detector
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include <cmath>
#include "CommonFramework/Tools/VideoOverlaySet.h"
#include "CommonFramework/ImageTools/ImageStats.h"
#include "CommonFramework/ImageMatch/ImageDiff.h"
#include "ImageMatchDetector.h"

#include <iostream>
using std::cout;
using std::endl;

namespace PokemonAutomation{



ImageMatchDetector::ImageMatchDetector(
    QImage reference_image, const ImageFloatBox& box,
    double max_rmsd, bool scale_brightness,
    Color color
)
    : m_reference_image(std::move(reference_image))
    , m_average_brightness(image_stats(m_reference_image).average)
    , m_max_rmsd(max_rmsd)
    , m_scale_brightness(scale_brightness)
    , m_color(color)
    , m_box(box)
{
    m_reference_image = extract_box(m_reference_image, m_box);
}

double ImageMatchDetector::rmsd(const QImage& frame) const{
    if (frame.isNull()){
        return 1000;
    }
    QImage scaled = extract_box(frame, m_box);
    if (scaled.size() != m_reference_image.size()){
        scaled = scaled.scaled(m_reference_image.size());
    }

    if (m_scale_brightness){
        FloatPixel image_brightness = ImageMatch::pixel_average(scaled, m_reference_image);
        FloatPixel scale = m_average_brightness / image_brightness;
        if (std::isnan(scale.r)) scale.r = 1.0;
        if (std::isnan(scale.g)) scale.g = 1.0;
        if (std::isnan(scale.b)) scale.b = 1.0;
        scale.bound(0.8, 1.2);
        ImageMatch::scale_brightness(scaled, scale);
    }

//    cout << "asdf" << endl;
    double ret = ImageMatch::pixel_RMSD(m_reference_image, scaled);
//    cout << "rmsd = " << ret << endl;
    return ret;
}

void ImageMatchDetector::make_overlays(VideoOverlaySet& items) const{
    items.add(m_color, m_box);
}
bool ImageMatchDetector::detect(const QImage& screen) const{
    return rmsd(screen) <= m_max_rmsd;
}



ImageMatchWatcher::ImageMatchWatcher(
    QImage reference_image, const ImageFloatBox& box,
    double max_rmsd, bool scale_brightness,
    std::chrono::milliseconds hold_duration,
    Color color
)
    : ImageMatchDetector(std::move(reference_image), box, max_rmsd, scale_brightness, color)
    , VisualInferenceCallback("ImageMatchWatcher")
    , m_hold_duration(hold_duration)
    , m_last_match(false)
    , m_start_of_match(std::chrono::system_clock::time_point::min())
{}

void ImageMatchWatcher::make_overlays(VideoOverlaySet& items) const{
    ImageMatchDetector::make_overlays(items);
}
bool ImageMatchWatcher::process_frame(
    const QImage& frame,
    std::chrono::system_clock::time_point
){
    if (!detect(frame)){
        m_last_match = false;
        return false;
    }
    auto now = std::chrono::system_clock::now();
    if (!m_last_match){
        m_last_match = true;
        m_start_of_match = now;
        return false;
    }
    return now - m_start_of_match >= m_hold_duration;
}






}

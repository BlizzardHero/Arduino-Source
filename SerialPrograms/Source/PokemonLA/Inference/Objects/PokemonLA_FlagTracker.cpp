/*  Flag Tracker
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include <cmath>
#include <map>
#include <QImage>
#include "PokemonLA_FlagTracker.h"

#include <iostream>
using std::cout;
using std::endl;

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonLA{



FlagTracker::FlagTracker(LoggerQt& logger, VideoOverlay& overlay)
    : VisualInferenceCallback("FlagTracker")
    , m_logger(logger)
    , m_watcher(overlay, {0, 0, 1, 1}, {{m_flags, false}})
{}

void FlagTracker::make_overlays(VideoOverlaySet& items) const{
    m_watcher.make_overlays(items);
}

bool FlagTracker::get(double& distance, double& x, double& y){
    SpinLockGuard lg(m_lock);
    if (m_history.empty()){
        return false;
    }
    const Sample& sample = m_history.back();
    x = sample.x;
    y = sample.y;

    std::multimap<int, std::chrono::system_clock::time_point> distances;
    for (const Sample& sample : m_history){
        if (0 <= sample.distance && sample.distance <= 999){
            distances.emplace(sample.distance, sample.timestamp);
        }
    }

    if (distances.size() < 5){
        distance = -1;
        return true;
    }

    //  Find the median.
    size_t mid = distances.size() / 2;
    auto iter = distances.begin();
    for (size_t c = 0; c < mid; c++){
        ++iter;
    }
    double median = iter->first;

    distance = median;
//    cout << distance << endl;

    return true;
}

bool FlagTracker::process_frame(
    const QImage& frame,
    std::chrono::system_clock::time_point timestamp
){
    m_watcher.process_frame(frame, timestamp);

    Sample sample;

    const std::vector<ImagePixelBox>& flags = m_flags.detections();
    bool ok = flags.size() == 1;
    if (ok){
        sample.timestamp = timestamp;
        sample.x = (double)(flags[0].min_x + flags[0].max_x) / (frame.width() * 2);
        sample.y = (double)(flags[0].min_y + flags[0].max_y) / (frame.height() * 2);
        sample.distance = read_flag_distance(frame, sample.x, sample.y);
    }


    SpinLockGuard lg(m_lock);

    //  Clear out old history.
    std::chrono::system_clock::time_point threshold = timestamp - std::chrono::seconds(2);
    while (!m_history.empty() && m_history.front().timestamp < threshold){
        m_history.pop_front();
    }

    if (ok){
        m_history.emplace_back(std::move(sample));
    }

    return false;
}





}
}
}

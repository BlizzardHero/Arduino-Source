/*  Black Border Detector
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include <QImage>
#include "CommonFramework/ImageTools/ImageStats.h"
#include "CommonFramework/ImageTools/SolidColorTest.h"
#include "CommonFramework/Tools/VideoOverlaySet.h"
#include "BlackBorderDetector.h"

#include <iostream>
using std::cout;
using std::endl;

namespace PokemonAutomation{


BlackBorderDetector::BlackBorderDetector()
    : m_top(0.001, 0.001, 0.997, 0.002)
    , m_bottom(0.001, 0.997, 0.997, 0.002)
    , m_left(0.001, 0.001, 0.002, 0.997)
    , m_right(0.997, 0.001, 0.002, 0.997)
//    , m_body(0.100, 0.100, 0.800, 0.800)
{}

void BlackBorderDetector::make_overlays(VideoOverlaySet& items) const{
    items.add(COLOR_RED, m_top);
    items.add(COLOR_RED, m_bottom);
    items.add(COLOR_RED, m_left);
    items.add(COLOR_RED, m_right);
//        items.add(COLOR_RED, m_body);
}
bool BlackBorderDetector::detect(const QImage& screen) const{
    ImageStats top = image_stats(extract_box(screen, m_top));
//    cout << "top = " << top.average << top.stddev << endl;
//    extract_box(screen, m_top).save("top.png");
    if (!is_black(top, 100, 30)){
        return false;
    }
    ImageStats bottom = image_stats(extract_box(screen, m_bottom));
//    cout << "bottom = " << bottom.average << bottom.stddev << endl;
    if (!is_black(bottom, 100, 30)){
        return false;
    }
    ImageStats left = image_stats(extract_box(screen, m_left));
//    cout << "left = " << left.average << left.stddev << endl;
    if (!is_black(left, 100, 30)){
        return false;
    }
    ImageStats right = image_stats(extract_box(screen, m_right));
//    cout << "right = " << right.average << right.stddev << endl;
    if (!is_black(right, 100, 30)){
        return false;
    }
//    ImageStats body = image_stats(extract_box(screen, m_body));
//    cout << "body = " << body.average << body.stddev << endl;
//    if (is_black(right, 100, 30)){
//        return false;
//    }


//    for (int c = 0; c < screen.width(); c++){
//        QRgb pixel = screen.pixel(c, 0);
//        cout << "(" << qRed(pixel) << "," << qGreen(pixel) << "," << qBlue(pixel) << ")";
//    }

    return true;
}


}

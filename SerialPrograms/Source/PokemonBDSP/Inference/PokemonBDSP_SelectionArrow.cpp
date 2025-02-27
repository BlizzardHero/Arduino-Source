/*  Selection Arrow
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "Common/Compiler.h"
#include "Kernels/Waterfill/Kernels_Waterfill.h"
#include "CommonFramework/Globals.h"
#include "CommonFramework/Tools/VideoOverlaySet.h"
#include "CommonFramework/ImageTools/ImageBoxes.h"
#include "CommonFramework/ImageMatch/ExactImageMatcher.h"
#include "CommonFramework/BinaryImage/BinaryImage_FilterRgb32.h"
#include "PokemonBDSP_SelectionArrow.h"

#include <iostream>
using std::cout;
using std::endl;

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonBDSP{

using namespace Kernels;
using namespace Kernels::Waterfill;


const ImageMatch::ExactImageMatcher& SELECTION_ARROW(){
    static ImageMatch::ExactImageMatcher matcher(QImage(RESOURCE_PATH() + "PokemonBDSP/SelectionArrow.png"));
    return matcher;
}


bool is_selection_arrow(const QImage& image, const WaterfillObject& object){
    size_t width = object.width();
    size_t height = object.height();
    if (width > height){
        return false;
    }
    if (height > 3 * width){
        return false;
    }

//    const QImage& exclamation_mark = SELECTION_ARROW();
    QImage cropped = image.copy(
        (int)object.min_x, (int)object.min_y,
        (int)width, (int)height
    );

//    static int c = 0;
//    scaled.save("test-" + QString::number(c++) + ".png");

    double rmsd = SELECTION_ARROW().rmsd(cropped);

//    scaled = scaled.scaled(exclamation_mark.width(), exclamation_mark.height());
//    double rmsd = ImageMatch::pixel_RMSD(exclamation_mark, scaled);
//    cout << "rmsd = " << rmsd << endl;
    return rmsd <= 90;
}


std::vector<ImagePixelBox> find_selection_arrows(const QImage& image){
    PackedBinaryMatrix matrix = compress_rgb32_to_binary_max(image, 200, 200, 200);
    std::vector<WaterfillObject> objects = find_objects_inplace(matrix, 200, false);
    std::vector<ImagePixelBox> ret;
    for (const WaterfillObject& object : objects){
        if (is_selection_arrow(image, object)){
            ret.emplace_back(
                ImagePixelBox(object.min_x, object.min_y, object.max_x, object.max_y)
            );
        }
    }
    return ret;
}




SelectionArrowFinder::SelectionArrowFinder(
    VideoOverlay& overlay,
    const ImageFloatBox& box,
    Color color
)
    : VisualInferenceCallback("SelectionArrowFinder")
    , m_overlay(overlay)
    , m_color(color)
    , m_box(box)
{}

void SelectionArrowFinder::detect(const QImage& screen){
    std::vector<ImagePixelBox> arrows = find_selection_arrows(extract_box(screen, m_box));

    m_arrow_boxes.clear();
    for (const ImagePixelBox& mark : arrows){
        m_arrow_boxes.emplace_back(m_overlay, translate_to_parent(screen, m_box, mark), COLOR_MAGENTA);
    }
}
void SelectionArrowFinder::make_overlays(VideoOverlaySet& items) const{
    items.add(m_color, m_box);
}
bool SelectionArrowFinder::process_frame(
    const QImage& frame,
    std::chrono::system_clock::time_point timestamp
){
    detect(frame);
//    cout << m_arrow_boxes.size() << endl;
//    if (!m_arrow_boxes.empty()){
//        extract_box(frame, m_arrow_boxes[0]).save("temp.png");
//        frame.save("test.png");
//    }
//    return !m_arrow_boxes.empty();

    //  Need 5 consecutive successful detections.
    if (m_arrow_boxes.empty()){
        m_trigger_count = 0;
        return false;
    }
    m_trigger_count++;
    return m_trigger_count >= 5;
}









}
}
}

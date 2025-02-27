/*  Sub-Object Template Matcher
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 *      Suppose we have an object with a distinctive sub-feature.
 *
 *  We want to find this object in an image. But instead of searching for the
 *  entire object as a whole, we search for that distinctive sub-feature.
 *
 *  Once we've found that sub-feature and have it's bounding box, we want to
 *  verify that the rest of the object matches the template.
 *
 */

#ifndef PokemonAutomation_CommonFramework_SubObjectTemplateMatcher_H
#define PokemonAutomation_CommonFramework_SubObjectTemplateMatcher_H

#include "Common/Cpp/Color.h"
#include "Kernels/BinaryMatrix/Kernels_BinaryMatrix.h"
#include "CommonFramework/ImageTools/ImageBoxes.h"
#include "CommonFramework/ImageMatch/ExactImageMatcher.h"

namespace PokemonAutomation{
namespace ImageMatch{


class SubObjectTemplateMatcher{
protected:
    using PackedBinaryMatrix = Kernels::PackedBinaryMatrix;
    using WaterfillObject = Kernels::Waterfill::WaterfillObject;

public:
    virtual ~SubObjectTemplateMatcher() = default;
    SubObjectTemplateMatcher(const char* path, double max_rmsd);
    SubObjectTemplateMatcher(const char* path, Color background_replacement, double max_rmsd);

    ImagePixelBox object_from_subobject(const ImagePixelBox& subobject_in_image) const;

    //  Compute RMSD of current object against the template as-is.
    double rmsd(
        ImagePixelBox& object_box,
        const QImage& image, const ImagePixelBox& subobject_in_image
    ) const;

    //  Replace the background of the image with the "m_background_replacement".
    //  Then compute RMSD of it against the template.
    //  The background is defined by zero-bits in the binary matrix in "binary_image".
    double rmsd_with_background_replace(
        ImagePixelBox& object_box,
        const QImage& image, const PackedBinaryMatrix& binary_image,
        const ImagePixelBox& subobject_in_image
    ) const;

    virtual bool matches(
        ImagePixelBox& object_box,
        const QImage& image,
        const WaterfillObject& subobject_in_image
    ) const;
    virtual bool matches_with_background_replace(
        ImagePixelBox& object_box,
        const QImage& image, const PackedBinaryMatrix& binary_image,
        const WaterfillObject& subobject_in_image
    ) const;


protected:
    virtual bool check_image(const QImage& image) const{ return true; };

    void set_subobject(const WaterfillObject& subobject_in_object);
    bool check_aspect_ratio(size_t candidate_width, size_t candidate_height) const;
    bool check_area_ratio(double candidate_area_ratio) const;

protected:
    QString m_path;
    QImage m_object;
    Color m_background_replacement;

    double m_max_rmsd;
    double m_aspect_ratio_lower = 0.80;
    double m_aspect_ratio_upper = 1.25;
    double m_area_ratio_lower = 0.80;
    double m_area_ratio_upper = 1.25;

    ExactImageMatcher m_matcher;
    ImagePixelBox m_subobject_in_object_p;
    ImageFloatBox m_subobject_in_object_f;
    double m_subobject_area_ratio;
};







}
}
#endif

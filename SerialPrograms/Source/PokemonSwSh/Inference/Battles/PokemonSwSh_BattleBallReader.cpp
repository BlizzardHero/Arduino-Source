/*  Ball Sprite Reader
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "CommonFramework/Tools/ErrorDumper.h"
#include "CommonFramework/OCR/OCR_NumberReader.h"
#include "CommonFramework/OCR/OCR_StringNormalization.h"
#include "CommonFramework/OCR/OCR_Filtering.h"
#include "PokemonSwSh/Resources/PokemonSwSh_PokeballSprites.h"
#include "PokemonSwSh_BattleBallReader.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSwSh{


const double BattleBallReader::MAX_ALPHA = 0.25;
const double BattleBallReader::ALPHA_SPREAD = 0.02;


ImageMatch::ExactImageDictionaryMatcher make_BALL_SPRITE_MATCHER(){
    ImageMatch::ExactImageDictionaryMatcher matcher({1, 128});
    for (const auto& item : all_pokeball_sprites()){
        matcher.add(item.first, item.second.sprite());
    }
    return matcher;
}
const ImageMatch::ExactImageDictionaryMatcher& BALL_SPRITE_MATCHER(){
    static ImageMatch::ExactImageDictionaryMatcher matcher = make_BALL_SPRITE_MATCHER();
    return matcher;
}



BattleBallReader::BattleBallReader(
    ConsoleHandle& console,
    Language language
)
    : m_matcher(BALL_SPRITE_MATCHER())
    , m_name_reader(PokeballNameReader::instance())
    , m_language(language)
    , m_console(console)
    , m_box_sprite(console, 0.649, 0.624, 0.0335, 0.060)
    , m_box_name(console, 0.710, 0.624, 0.18, 0.060)
    , m_box_quantity(console, 0.895, 0.624, 0.059, 0.060)
{}

std::string BattleBallReader::read_ball(const QImage& screen) const{
    if (screen.isNull()){
        return "";
    }

    ImageMatch::ImageMatchResult sprite_result;
    {
        sprite_result = m_matcher.match(screen, m_box_sprite, 2, ALPHA_SPREAD);
        sprite_result.log(m_console, 0.50);
        if (!sprite_result.results.empty() && sprite_result.results.begin()->first > MAX_ALPHA){
            sprite_result.results.clear();
        }
    }
    OCR::StringMatchResult name_result;
    {
        QImage cropped = extract_box(screen, m_box_name);
        OCR::make_OCR_filter(cropped).apply(cropped);
        name_result = m_name_reader.read_substring(m_console, m_language, cropped);
    }

    if (sprite_result.results.empty()){
        if (name_result.results.size() == 1){
            return name_result.results.begin()->second.token;
        }
        dump_image(m_console, ProgramInfo(), "BattleBallReader", screen);
        return "";
    }

    if (sprite_result.results.size() == 1){
        return sprite_result.results.begin()->second;
    }

    std::set<std::string> ocr_slugs;
    for (const auto& item : name_result.results){
        ocr_slugs.insert(item.second.token);
    }

    std::vector<std::string> overlap;
    for (const auto& item : sprite_result.results){
        auto iter = ocr_slugs.find(item.second);
        if (iter != ocr_slugs.end()){
            overlap.emplace_back(item.second);
        }
    }
    if (overlap.size() != 1){
        dump_image(m_console, ProgramInfo(), "BattleBallReader", screen);
        return "";
    }
    return overlap[0];
}
uint16_t BattleBallReader::read_quantity(const QImage& screen) const{
    QImage image = extract_box(screen, m_box_quantity);
    auto filter = OCR::make_OCR_filter(image);
    filter.apply(image);
    return OCR::read_number(m_console, image);
}






}
}
}

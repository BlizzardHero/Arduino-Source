/*  Test Program (Switch)
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include <cmath>
//#include <QSystemTrayIcon>
#include <QProcess>
#include "Common/Cpp/Exceptions.h"
#include "Common/Cpp/PrettyPrint.h"
#include "Common/Cpp/AlignedVector.h"
#include "Common/Cpp/SIMDDebuggers.h"
#include "Common/Qt/QtJsonTools.h"
#include "ClientSource/Libraries/Logging.h"
#include "CommonFramework/PersistentSettings.h"
#include "CommonFramework/Tools/StatsTracking.h"
#include "CommonFramework/Tools/StatsDatabase.h"
#include "CommonFramework/Tools/InterruptableCommands.h"
#include "CommonFramework/Inference/InferenceThrottler.h"
#include "CommonFramework/Inference/AnomalyDetector.h"
#include "CommonFramework/Inference/StatAccumulator.h"
#include "CommonFramework/Inference/TimeWindowStatTracker.h"
#include "CommonFramework/InferenceInfra/VisualInferenceSession.h"
#include "CommonFramework/InferenceInfra/InferenceRoutines.h"
#include "CommonFramework/Inference/FrozenImageDetector.h"
#include "CommonFramework/Inference/BlackScreenDetector.h"
#include "CommonFramework/ImageTools/SolidColorTest.h"
#include "CommonFramework/ImageMatch/FilterToAlpha.h"
#include "CommonFramework/ImageMatch/ImageDiff.h"
#include "CommonFramework/ImageMatch/ImageCropper.h"
#include "CommonFramework/ImageMatch/ImageDiff.h"
#include "CommonFramework/OCR/OCR_RawOCR.h"
#include "CommonFramework/OCR/OCR_Filtering.h"
#include "CommonFramework/OCR/OCR_StringNormalization.h"
#include "CommonFramework/OCR/OCR_TextMatcher.h"
#include "CommonFramework/OCR/OCR_LargeDictionaryMatcher.h"
#include "CommonFramework/ImageMatch/ExactImageDictionaryMatcher.h"
#include "CommonFramework/ImageMatch/CroppedImageDictionaryMatcher.h"
#include "CommonFramework/Inference/ImageMatchDetector.h"
#include "CommonFramework/Notifications/ProgramNotifications.h"
#include "CommonFramework/Tools/ErrorDumper.h"
#include "NintendoSwitch/NintendoSwitch_Settings.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_Device.h"
#include "NintendoSwitch/Commands/NintendoSwitch_Commands_PushButtons.h"
#include "Pokemon/Resources/Pokemon_PokemonNames.h"
#include "PokemonSwSh/ShinyHuntTracker.h"
#include "PokemonSwSh/Resources/PokemonSwSh_PokemonSprites.h"
#include "PokemonSwSh/Resources/PokemonSwSh_PokeballSprites.h"
#include "PokemonSwSh/Inference/ShinyDetection/PokemonSwSh_ShinyEncounterDetector.h"
#include "PokemonSwSh/Inference/Dens/PokemonSwSh_RaidLobbyReader.h"
#include "PokemonSwSh/Inference/Battles/PokemonSwSh_StartBattleDetector.h"
#include "PokemonSwSh/Inference/PokemonSwSh_SummaryShinySymbolDetector.h"
#include "PokemonSwSh/Inference/Dens/PokemonSwSh_RaidCatchDetector.h"
#include "PokemonSwSh/Inference/Battles/PokemonSwSh_BattleMenuDetector.h"
#include "PokemonSwSh/Inference/PokemonSwSh_SelectionArrowFinder.h"
#include "PokemonSwSh/Inference/PokemonSwSh_FishingDetector.h"
#include "PokemonSwSh/Inference/PokemonSwSh_MarkFinder.h"
#include "PokemonSwSh/Inference/PokemonSwSh_ReceivePokemonDetector.h"
#include "PokemonSwSh/Inference/PokemonSwSh_PokemonSpriteReader.h"
#include "PokemonSwSh/Inference/PokemonSwSh_TypeSymbolFinder.h"
#include "PokemonSwSh/Inference/Battles/PokemonSwSh_BattleBallReader.h"
#include "PokemonSwSh/Inference/Dens/PokemonSwSh_DenMonReader.h"
#include "PokemonSwSh/Inference/Battles/PokemonSwSh_ExperienceGainDetector.h"
#include "PokemonSwSh/Inference/PokemonSwSh_YCommDetector.h"
#include "PokemonSwSh/MaxLair/Inference/PokemonSwSh_MaxLair_Detect_Entrance.h"
#include "PokemonSwSh/MaxLair/Inference/PokemonSwSh_MaxLair_Detect_PokemonReader.h"
#include "PokemonSwSh/MaxLair/Inference/PokemonSwSh_MaxLair_Detect_PathSelect.h"
#include "PokemonSwSh/MaxLair/Inference/PokemonSwSh_MaxLair_Detect_ItemSelectMenu.h"
#include "PokemonSwSh/MaxLair/Inference/PokemonSwSh_MaxLair_Detect_BattleMenu.h"
#include "PokemonSwSh/MaxLair/Inference/PokemonSwSh_MaxLair_Detect_EndBattle.h"
#include "PokemonSwSh/MaxLair/Inference/PokemonSwSh_MaxLair_Detect_Lobby.h"
#include "PokemonSwSh/MaxLair/Inference/PokemonSwSh_MaxLair_Detect_PokemonSwapMenu.h"
#include "PokemonSwSh/MaxLair/Inference/PokemonSwSh_MaxLair_Detect_PokemonSelectMenu.h"
#include "PokemonSwSh/MaxLair/Inference/PokemonSwSh_MaxLair_Detect_ProfessorSwap.h"
#include "PokemonSwSh/MaxLair/Program/PokemonSwSh_MaxLair_Run_CaughtScreen.h"
#include "PokemonSwSh/MaxLair/Program/PokemonSwSh_MaxLair_Run_Entrance.h"
#include "PokemonSwSh/MaxLair/AI/PokemonSwSh_MaxLair_AI.h"
#include "PokemonSwSh/MaxLair/AI/PokemonSwSh_MaxLair_AI_PathMatchup.h"
#include "PokemonSwSh/MaxLair/AI/PokemonSwSh_MaxLair_AI_RentalBossMatchup.h"
#include "PokemonSwSh/PkmnLib/PokemonSwSh_PkmnLib_Moves.h"
#include "PokemonSwSh/PkmnLib/PokemonSwSh_PkmnLib_Pokemon.h"
#include "PokemonSwSh/PkmnLib/PokemonSwSh_PkmnLib_Matchup.h"
#include "PokemonSwSh/Resources/PokemonSwSh_MaxLairDatabase.h"
#include "PokemonSwSh/Programs/PokemonSwSh_BasicCatcher.h"
#include "PokemonSwSh/Programs/PokemonSwSh_Internet.h"
#include "PokemonSwSh/Resources/PokemonSwSh_TypeSprites.h"
#include "Kernels/Kernels_x64_SSE41.h"
#include "Kernels/PartialWordAccess/Kernels_PartialWordAccess_x64_SSE41.h"
//#include "Kernels/PartialWordAccess/Kernels_PartialWordAccess_x64_AVX2.h"
#include "Kernels/ImageStats/Kernels_ImagePixelSumSqr.h"
#include "Kernels/ImageStats/Kernels_ImagePixelSumSqrDev.h"
#include "Kernels/Kernels_Alignment.h"
//#include "Kernels/Waterfill/Kernels_Waterfill_Intrinsics_SSE4.h"
//#include "Kernels/Waterfill/Kernels_Waterfill_FillQueue.h"
//#include "Kernels/BinaryImage/Kernels_BinaryImage_Default.h"
//#include "Kernels/BinaryImage/Kernels_BinaryImage_x64_SSE42.h"
#include "Kernels/BinaryImageFilters/Kernels_BinaryImage_BasicFilters_Default.h"
#include "Kernels/BinaryImageFilters/Kernels_BinaryImage_BasicFilters_x64_SSE42.h"
//#include "Kernels/BinaryImageFilters/Kernels_BinaryImage_BasicFilters_x64_AVX2.h"
//#include "Kernels/BinaryImageFilters/Kernels_BinaryImage_BasicFilters_x64_AVX512.h"
#include "Kernels/Waterfill/Kernels_Waterfill.h"
#include "CommonFramework/BinaryImage/BinaryImage_FilterRgb32.h"
#include "Integrations/DiscordWebhook.h"
#include "Pokemon/Pokemon_Notification.h"
#include "PokemonSwSh/Programs/PokemonSwSh_StartGame.h"
#include "PokemonSwSh/Inference/Battles/PokemonSwSh_BattleDialogTracker.h"
#include "PokemonBDSP/PokemonBDSP_Settings.h"
#include "PokemonBDSP/Inference/PokemonBDSP_DialogDetector.h"
#include "CommonFramework/ImageTools/ColorClustering.h"
#include "PokemonBDSP/Inference/PokemonBDSP_DialogDetector.h"
#include "PokemonBDSP/Inference/ShinyDetection/PokemonBDSP_ShinyEncounterDetector.h"
#include "PokemonBDSP/Inference/PokemonBDSP_MarkFinder.h"
#include "PokemonBDSP/Programs/PokemonBDSP_GameEntry.h"
#include "PokemonBDSP/Inference/PokemonBDSP_MapDetector.h"
#include "PokemonBDSP/Inference/Battles/PokemonBDSP_BattleBallReader.h"
#include "PokemonBDSP/Inference/PokemonBDSP_SelectionArrow.h"
#include "PokemonBDSP/Inference/Battles/PokemonBDSP_BattleMenuDetector.h"
#include "PokemonBDSP/Inference/PokemonBDSP_VSSeekerReaction.h"
#include "PokemonBDSP/Inference/Battles/PokemonBDSP_StartBattleDetector.h"
#include "PokemonBDSP/Inference/PokemonBDSP_MenuDetector.h"
#include "PokemonBDSP/Inference/BoxSystem/PokemonBDSP_BoxDetector.h"
#include "PokemonBDSP/Inference/BoxSystem/PokemonBDSP_BoxShinyDetector.h"
#include "PokemonBDSP/Programs/Eggs/PokemonBDSP_EggRoutines.h"
#include "PokemonBDSP/Programs/Eggs/PokemonBDSP_EggFeedback.h"
#include "PokemonBDSP/Programs/PokemonBDSP_RunFromBattle.h"
#include "PokemonBDSP/Programs/PokemonBDSP_BoxRelease.h"
#include "PokemonBDSP/Inference/BoxSystem/PokemonBDSP_IVCheckerReader.h"
//#include "CommonFramework/BinaryImage/BinaryImage.h"
#include "PokemonSwSh/MaxLair/Inference/PokemonSwSh_MaxLair_Detect_BattleMenu.h"
#include "PokemonSwSh/MaxLair/Inference/PokemonSwSh_MaxLair_Detect_PathSide.h"
#include "PokemonSwSh/Inference/PokemonSwSh_TypeSymbolFinder.h"
#include "PokemonSwSh/Inference/ShinyDetection/PokemonSwSh_SparkleDetectorRadial.h"
#include "PokemonSwSh/Inference/ShinyDetection/PokemonSwSh_SparkleDetectorSquare.h"
#include "PokemonSwSh/Inference/ShinyDetection/PokemonSwSh_ShinySparkleSet.h"
#include "PokemonBDSP/Inference/Battles/PokemonBDSP_ExperienceGainDetector.h"
#include "PokemonLA/Inference/Objects/PokemonLA_BubbleDetector.h"
#include "PokemonLA/Inference/Objects/PokemonLA_ArcDetector.h"
#include "PokemonLA/Inference/Objects/PokemonLA_QuestMarkDetector.h"
#include "PokemonLA/Inference/Objects/PokemonLA_ShinySymbolDetector.h"
#include "CommonFramework/ImageMatch/SubObjectTemplateMatcher.h"
#include "CommonFramework/Inference/BlackBorderDetector.h"
#include "PokemonLA/Programs/PokemonLA_GameEntry.h"
#include "PokemonLA/PokemonLA_Settings.h"
#include "PokemonSwSh/Inference/PokemonSwSh_SelectionArrowFinder.h"
#include "PokemonLA/Inference/PokemonLA_MountDetector.h"
#include "PokemonLA/Inference/Objects/PokemonLA_FlagDetector.h"
#include "PokemonLA/Inference/Objects/PokemonLA_FlagTracker.h"
#include "CommonFramework/Tools/InterruptableCommands.h"
#include "CommonFramework/Tools/SuperControlSession.h"
#include "PokemonLA/Programs/PokemonLA_FlagNavigationAir.h"
#include "CommonFramework/ImageMatch/WaterfillTemplateMatcher.h"
#include "PokemonLA/Inference/Objects/PokemonLA_ButtonDetector.h"
#include "Kernels/ImageFilters/Kernels_ImageFilter_Basic.h"
#include "PokemonLA/Inference/PokemonLA_NotificationReader.h"
#include "PokemonLA/Inference/PokemonLA_OutbreakReader.h"
#include "PokemonLA/Inference/PokemonLA_SelectedRegionDetector.h"
#include "PokemonLA/Inference/PokemonLA_MapDetector.h"
#include "PokemonLA/Programs/PokemonLA_RegionNavigation.h"
#include "PokemonLA/Programs/PokemonLA_TradeRoutines.h"
#include "PokemonLA/Inference/PokemonLA_DialogDetector.h"
#include "PokemonLA/Inference/PokemonLA_OverworldDetector.h"
#include "CommonFramework/Tools/MultiConsoleErrors.h"
#include "PokemonLA/Inference/PokemonLA_UnderAttackDetector.h"
#include "PokemonLA/Programs/PokemonLA_EscapeFromAttack.h"
#include "PokemonLA/Inference/Objects/PokemonLA_ArcPhoneDetector.h"
#include "CommonFramework/Inference/SpectrogramMatcher.h"
#include "CommonFramework/ImageMatch/WaterfillTemplateMatcher.h"
#include "TestProgramSwitch.h"

#include <immintrin.h>
#include <fstream>
#include <QHttpMultiPart>
#include <QFile>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

//#include <Windows.h>
#include <iostream>
using std::cout;
using std::endl;


//#include "../Internal/SerialPrograms/NintendoSwitch_Commands_ScalarButtons.h"
using namespace PokemonAutomation::Kernels;
using namespace PokemonAutomation::Kernels::Waterfill;



namespace PokemonAutomation{
namespace NintendoSwitch{


TestProgram_Descriptor::TestProgram_Descriptor()
    : MultiSwitchProgramDescriptor(
        "NintendoSwitch:TestProgram",
        "Nintendo Switch", "Test Program (Switch)",
        "",
        "Test Program (Switch)",
        FeedbackType::OPTIONAL_, true,
        PABotBaseLevel::PABOTBASE_12KB,
        1, 4, 1
    )
{}


TestProgram::TestProgram(const TestProgram_Descriptor& descriptor)
    : MultiSwitchProgramInstance(descriptor)
    , LANGUAGE(
        "<b>OCR Language:</b>",
        { Language::English }
    )
    , NOTIFICATION_TEST("Test", true, true, ImageAttachmentMode::JPG)
    , NOTIFICATIONS({
        &NOTIFICATION_TEST,
//        &NOTIFICATION_ERROR_RECOVERABLE,
//        &NOTIFICATION_ERROR_FATAL,
    })
{
    PA_ADD_OPTION(LANGUAGE);
    PA_ADD_OPTION(NOTIFICATIONS);
}


//using namespace Kernels;
//using namespace Kernels::Waterfill;

using namespace PokemonLA;









void TestProgram::program(MultiSwitchProgramEnvironment& env){
    using namespace Kernels;
    using namespace Kernels::Waterfill;
    using namespace OCR;
    using namespace Pokemon;
//    using namespace PokemonSwSh;
//    using namespace PokemonBDSP;
    using namespace PokemonLA;

    LoggerQt& logger = env.logger();
    ConsoleHandle& console = env.consoles[0];
    BotBase& botbase = env.consoles[0];
    VideoFeed& feed = env.consoles[0];
    VideoOverlay& overlay = env.consoles[0];



#if 0
    for (size_t c = 0; c < 10; c++){
        QImage image("Digit-" + QString::number(c) + "-Original.png");

        image = image.convertToFormat(QImage::Format::Format_ARGB32);
        uint32_t* ptr = (uint32_t*)image.bits();
        size_t words = image.bytesPerLine() / sizeof(uint32_t);
        for (int r = 0; r < image.height(); r++){
            for (int c = 0; c < image.width(); c++){
                uint32_t& pixel = ptr[r * words + c];
                uint32_t red = qRed(pixel);
                uint32_t green = qGreen(pixel);
                uint32_t blue = qBlue(pixel);
                if (red < 0xa0 || green < 0xa0 || blue < 0xa0){
                    pixel = 0x00000000;
                }
            }
        }
        image.save("Digit-" + QString::number(c) + "-Template.png");
    }
#endif





//    QImage image("Distance-test.png");

    QImage frame = feed.snapshot();

    FlagTracker tracker(console, console);
    tracker.process_frame(frame, std::chrono::system_clock::now());

    double distance, flag_x, flag_y;
    tracker.get(distance, flag_x, flag_y);


#if 0
    ImageFloatBox box(flag_x - 0.017, flag_y - 0.055, 0.032, 0.025);
    QImage image = extract_box(frame, box);



    int c = 0;

    PackedBinaryMatrix matrix = compress_rgb32_to_binary_range(image, 0xff808080, 0xffffffff);
//    PackedBinaryMatrix matrix = compress_rgb32_to_binary_range(image, 0xffd0d0d0, 0xffffffff);
    WaterFillIterator finder(matrix, 30);
    WaterfillObject object;
    while (finder.find_next(object)){
        //  Skip anything that touches the edge.
        if (object.min_x == 0 || object.min_y == 0 ||
            object.max_x + 1 == matrix.width() || object.max_y + 1 == matrix.height()
        ){
            continue;
        }
//        extract_box(image, object).save("image-" + QString::number(c++) + ".png");
        read_digit(image, object);
    }
#endif






//    goto_camp_from_jubilife(env, console, WarpSpot::ICELANDS_ICEPEAK_ARENA);

#if 0
    QImage image("screenshot-20220309-005426729947.png");

    MountDetector detector;
    detector.detect(image);
#endif


#if 0
    pbf_move_left_joystick(console, 0, 0, 50, 0);
    pbf_press_button(console, BUTTON_B, 20, 250);
    pbf_mash_button(console, BUTTON_ZL, 250);
    pbf_press_button(console, BUTTON_HOME, 20, 230);
#endif

#if 0
    {
        QImage image("screenshot-20220306-163207833403.png");
//        QImage image("screenshot-20220306-172029164014.png");

        DialogSurpriseDetector detector(logger, overlay, true);
        detector.process_frame(image, std::chrono::system_clock::now());
    }
#endif
#if 0
    {
        QImage image("screenshot-20220302-094034596712.png");

        DialogDetector detector(logger, overlay, true);
        detector.process_frame(image, std::chrono::system_clock::now());
    }
    {
        QImage image("screenshot-Gin");

        DialogDetector detector(logger, overlay, true);
        detector.process_frame(image, std::chrono::system_clock::now());
    }
#endif


#if 0
    InferenceBoxScope box0(console, {0.925, 0.100, 0.014, 0.030});

//    QImage screen("screenshot-20220228-121927882824.png");
    QImage screen = feed.snapshot();

    QImage image = extract_box(screen, box0);
    ImageStats stats = image_stats(image);
    cout << stats.average << stats.stddev << endl;
    bool ok = is_white(stats);
    cout << ok << endl;
#endif

//    throw UserSetupError(env.logger(), "asdf");
//    throw OperationFailedException(env.logger(), "asdf");



//    FlagNavigationAir session(env, console);
//    session.run_session();



//    goto_camp_from_overworld(env, console);


//    InferenceBoxScope box(console, {0.450, 0.005, 0.040, 0.010});
//    ImageStats stats = image_stats(extract_box(console.video().snapshot(), box));
//    cout << stats.average << stats.stddev << endl;



#if 0
    pbf_press_dpad(console, DPAD_UP, 20, 480);
    pbf_press_button(console, BUTTON_A, 20, 480);
    pbf_press_button(console, BUTTON_B, 20, 230);
    pbf_press_button(console, BUTTON_B, 20, 230);
#endif


#if 0
    auto& context = console;

                    pbf_move_left_joystick(context, 0, 212, 50, 0);
                    pbf_press_button(context, BUTTON_B, 500, 80);

                    pbf_move_left_joystick(context, 224, 0, 50, 0);
                    pbf_press_button(context, BUTTON_B, 350, 80);

                    pbf_move_left_joystick(context, 0, 64, 50, 0);
                    pbf_press_button(context, BUTTON_B, 250, 80);

                    pbf_move_left_joystick(context, 0, 96, 50, 0);
                    pbf_press_button(context, BUTTON_B, 500, 0);
#endif


#if 0
    VideoOverlaySet set(overlay);
    DialogDetector detector(console, console);
    detector.make_overlays(set);
    detector.process_frame(feed.snapshot(), std::chrono::system_clock::now());
#endif




#if 0
    InferenceBoxScope box0(overlay, {0.010, 0.700, 0.050, 0.100});
    QImage image = extract_box(feed.snapshot(), box0);

    ArcPhoneDetector detector(console, console, std::chrono::milliseconds(200), true);
    detector.process_frame(image, std::chrono::system_clock::now());
    wait_until(
        env, console, std::chrono::seconds(10),
        { &detector }
    );
#endif


//    from_professor_return_to_jubilife(env, console);



//    InferenceBoxScope box0(overlay, {0.900, 0.955, 0.080, 0.045});
//    InferenceBoxScope box1(overlay, {0.500, 0.621, 0.300, 0.043});



//    EscapeFromAttack session(env, console);
//    session.run_session();
//    cout << "Done escaping!" << endl;


#if 0
    MapDetector detector;
    VideoOverlaySet overlays(overlay);
    detector.make_overlays(overlays);
#endif

#if 0
    ButtonDetector detector(
        console, console,
        ButtonType::ButtonA,
        {0.55, 0.40, 0.20, 0.40},
        std::chrono::milliseconds(200), true
    );
    VideoOverlaySet overlays(overlay);
    detector.make_overlays(overlays);

    detector.process_frame(feed.snapshot(), std::chrono::system_clock::now());
#endif


#if 0
    UnderAttackWatcher watcher;
    wait_until(
        env, console,
         std::chrono::seconds(60),
         { &watcher }
    );
#endif


//    InferenceBoxScope box(overlay, 0.49, 0.07, 0.02, 0.03);
//    ImageStats stats = image_stats(extract_box(feed.snapshot(), box));
//    cout << stats.average << stats.stddev << endl;


//    return_to_jubilife_from_overworld(env, console);







#if 0
    pbf_move_right_joystick(console, 0, 128, 145, 0);
    pbf_move_left_joystick(console, 128, 0, 50, 0);
    pbf_press_button(console, BUTTON_B, 500, 125);

    pbf_move_right_joystick(console, 255, 128, 45, 0);
    pbf_move_left_joystick(console, 128, 0, 50, 0);
    pbf_press_button(console, BUTTON_B, 420, 125);

    pbf_move_right_joystick(console, 0, 128, 100, 0);
    pbf_move_left_joystick(console, 128, 0, 50, 0);
    pbf_press_button(console, BUTTON_B, 420, 125);
#endif



#if 0

    TradeNameReader reader(console, LANGUAGE, console);
    reader.read(feed.snapshot());


    InferenceBoxScope box(overlay, {0.920, 0.100, 0.020, 0.030});
    QImage image = extract_box(feed.snapshot(), box);
    ImageStats stats = image_stats(image);
    cout << stats.average << stats.stddev << endl;

//    is_white()


//    std::map<std::string, int> catch_count;
#endif




#if 0
    TradeStats stats;
    MultiConsoleErrorState error_state;

    env.run_in_parallel([&](ConsoleHandle& console){
        trade_current_pokemon(env, console, error_state, stats);
    });
#endif


//    trade_current_pokemon(env, console, error_state, stats);






#if 0
    QImage image("screenshot-20220213-141558364230.png");

//    CenterAButtonTracker tracker;
//    WhiteObjectWatcher detector(console, {0.40, 0.50, 0.40, 0.50}, {{tracker, false}});

//    detector.process_frame(image, std::chrono::system_clock::now());

    ButtonDetector detector(console, console, ButtonType::ButtonA, {0.40, 0.50, 0.40, 0.50});


    AsyncVisualInferenceSession visual(env, console, console, console);
    visual += detector;
#endif




//    InferenceBoxScope box(overlay, 0.40, 0.50, 0.40, 0.50);


//    cout << std::chrono::system_clock::time_point::min() - std::chrono::system_clock::now() << endl;

//    pbf_move_right_joystick(console, 0, 128, 45, 0);

#if 0
                pbf_move_right_joystick(console, 128, 255, 200, 0);
                pbf_move_right_joystick(console, 128, 0, 200, 0);
                pbf_move_right_joystick(console, 128, 255, 80, 0);
                pbf_move_right_joystick(console, 0, 128, 400, 0);
                pbf_move_right_joystick(console, 128, 255, 120, 0);
                pbf_move_right_joystick(console, 0, 128, 400, 0);
                pbf_move_right_joystick(console, 128, 0, 200, 0);
                pbf_move_right_joystick(console, 0, 128, 400, 0);
#endif

#if 0
    FlagTracker flag(logger, overlay);
    MountTracker mount(logger);

    {
        AsyncVisualInferenceSession visual(env, console, console, console);
        visual += flag;
        visual += mount;

        AsyncCommandSession commands(env, console.botbase());

        while (true){
//            commands.dispatch([=](const BotBaseContext& context){
//                pbf_move_right_joystick(context, 0, 128, 5 * TICKS_PER_SECOND, 0);
//            });
//            commands.wait();

            double flag_distance, flag_x, flag_y;
            bool flag_ok = flag.get(flag_distance, flag_x, flag_y);

            if (flag_ok && 0.4 < flag_x && flag_x < 0.6 && flag_y > 0.6){
            commands.dispatch([=](const BotBaseContext& context){
                pbf_press_button(context, BUTTON_B, 300 * TICKS_PER_SECOND, 0);
            });



            }
        }

        commands.stop_session();
        visual.stop();
    }
#endif






#if 0
    MountDetector mount_detector;
    FlagDetector flags;
    WhiteObjectWatcher watcher(console, {{flags, false}});

    QImage image(feed.snapshot());

    MountState mount_state = mount_detector.detect(image);
    cout << MOUNT_STATE_STRINGS[(int)mount_state] << endl;

//    watcher.process_frame(image, std::chrono::system_clock::now());
//    flags.detections()

#endif



#if 0
    QImage image("screenshot-20220211-023701107827.png");


    FlagDetector flags;

    WhiteObjectWatcher watcher(
        console,
        {{flags, false}}
    );

    watcher.process_frame(image, std::chrono::system_clock::now());
#endif


#if 0
    QImage image(feed.snapshot());
//    QImage image("screenshot-20220210-231922898436.png");
//    QImage image("screenshot-20220211-005950586653.png");
//    QImage image("screenshot-20220211-012426947705.png");
//    QImage image("screenshot-20220211-014247032114.png");
//    QImage image("screenshot-20220211-022344759878.png");

    HmDetector detector(overlay);
    cout << HM_STATE_STRINGS[(int)detector.detect(image)] << endl;
#endif


#if 0
    InferenceBoxScope box(overlay, 0.905, 0.65, 0.08, 0.13);
    QImage image = extract_box(feed.snapshot(), box);


    PackedBinaryMatrix matrix = compress_rgb32_to_binary_range(
        image,
        192, 255,
        192, 255,
        0, 255
    );
    std::vector<WaterfillObject> objects = find_objects_inplace(matrix, 20, false);


    ImagePixelBox result;
    for (const WaterfillObject& object : objects){
        int c = 0;
        for (const auto& object : objects){
            extract_box(image, object).save("test-" + QString::number(c++) + ".png");
        }
        if (HmWyrdeerMatcher::on().matches(result, image, object)){
            cout << "Wyrdeer On" << endl;
        }
#if 0
        if (HmWyrdeerMatcher::off().matches(result, image, object)){
            cout << "Wyrdeer Off" << endl;
        }
        if (HmWyrdeerMatcher::on().matches(result, image, object)){
            cout << "Wyrdeer On" << endl;
        }
        if (HmUrsalunaMatcher::off().matches(result, image, object)){
            cout << "Ursaluna Off" << endl;
        }
        if (HmUrsalunaMatcher::on().matches(result, image, object)){
            cout << "Ursaluna On" << endl;
        }
        if (HmSneaslerMatcher::off().matches(result, image, object)){
            cout << "Sneasler Off" << endl;
        }
        if (HmSneaslerMatcher::on().matches(result, image, object)){
            cout << "Sneasler On" << endl;
        }
        if (HmBraviaryMatcher::off().matches(result, image, object)){
            cout << "Braviary Off" << endl;
        }
        if (HmBraviaryMatcher::on().matches(result, image, object)){
            cout << "Braviary On" << endl;
        }
#endif
    }
#endif



#if 0
    InferenceBoxScope box(overlay, 0.905, 0.65, 0.08, 0.13);
    QImage image = extract_box(feed.snapshot(), box);

//    QImage image("test.png");
    PackedBinaryMatrix matrix = compress_rgb32_to_binary_range(
        image,
        128, 255,
        128, 255,
        0, 255
    );
    std::vector<WaterfillObject> objects = find_objects_inplace(matrix, 20, false);
    cout << objects.size() << endl;


//    int c = 0;
//    for (const auto& object : objects){
//        extract_box(image, object).save("test-" + QString::number(c++) + ".png");
//    }

#if 1
    WaterfillObject object;
    for (const WaterfillObject& obj : objects){
        object.merge_assume_no_overlap(obj);
    }

    ImagePixelBox sbox(object.min_x, object.min_y, object.max_x, object.max_y);
    extract_box(image, sbox).save("test.png");
#endif

#endif




//    QImage image("2054071609223400_s.jpg");
//    SelectionArrowFinder arrow_detector(console, ImageFloatBox(0.350, 0.450, 0.500, 0.400));
//    arrow_detector.detect(image);



#if 0

//    ArcWatcher arcs(overlay);
//    BubbleWatcher bubbles(overlay);

    BubbleDetector bubbles;
    ArcDetector arcs;
    QuestMarkDetector quest_marks;
    WhiteObjectWatcher watcher(
        overlay,
        {{bubbles, false}, {arcs, false}, {quest_marks, false}}
    );
//    watcher.process_frame(feed.snapshot(), std::chrono::system_clock::now());
#if 1
    {
        VisualInferenceSession session(env, logger, feed, overlay);
        session += watcher;
        session.run();
    }
#endif


#endif

#if 0
//    InferenceBoxScope box(overlay, 0.40, 0.50, 0.40, 0.50);
    InferenceBoxScope box(overlay, 0.010, 0.700, 0.050, 0.100);

    QImage image(feed.snapshot());
    image = extract_box(image, box);

    PackedBinaryMatrix matrix = compress_rgb32_to_binary_range(
        image,
        128, 255,
        128, 255,
        128, 255
    );
    std::vector<WaterfillObject> objects = find_objects_inplace(matrix, 20, false);
    cout << objects.size() << endl;

    int c = 0;
    for (const auto& object : objects){
        extract_box(image, object).save("test-" + QString::number(c++) + ".png");
    }
#endif




//    ShinySymbolWatcher watcher(overlay, SHINY_SYMBOL_BOX_BOTTOM);
//    watcher.process_frame(feed.snapshot(), std::chrono::system_clock::now());


//    ArcDetector detector;


//    QImage image("screenshot-20220124-212851483502.png");
//    QImage image(feed.snapshot());
//    find_arcs(image);

#if 0
    QImage image0("test-image.png");
    QImage image1("test-sprite.png");
    for (int r = 0; r < image0.height(); r++){
        for (int c = 0; c < image0.width(); c++){
            uint32_t pixel1 = image1.pixel(c, r);
            if ((pixel1 >> 24) == 0){
                image0.setPixel(c, r, 0xff00ff00);
            }
        }
    }
    image0.save("test.png");
#endif



#if 0
    QImage image("screenshot-20220124-212851483502.png");

    InferenceBoxScope box(overlay, 0.4, 0.4, 0.2, 0.2);
    image = extract_box(image, box);
    PackedBinaryMatrix matrix = compress_rgb32_to_binary_range(
        image,
        128, 255,
        128, 255,
        128, 255
    );
    std::vector<WaterfillObject> objects = find_objects_inplace(matrix, 20, false);
    cout << objects.size() << endl;

    int c = 0;
    for (const auto& object : objects){
        extract_box(image, object).save("test-" + QString::number(c++) + ".png");
    }

//    WaterfillObject object = objects[1];
//    object.merge_assume_no_overlap(objects[2]);

 //   extract_box(image, object).save("test.png");
#endif




#if 0
    QImage image("MountOn-Wyrdeer-Template.png");

    image = image.convertToFormat(QImage::Format::Format_ARGB32);
    uint32_t* ptr = (uint32_t*)image.bits();
    size_t words = image.bytesPerLine() / sizeof(uint32_t);
    for (int r = 0; r < image.height(); r++){
        for (int c = 0; c < image.width(); c++){
            uint32_t& pixel = ptr[r * words + c];
            uint32_t red = qRed(pixel);
            uint32_t green = qGreen(pixel);
            uint32_t blue = qBlue(pixel);
            if (red < 0xa0 || green < 0xa0 || blue < 0xa0){
                pixel = 0x00000000;
            }
        }
    }
    image.save("MountOn-Wyrdeer-Template-1.png");
#endif



#if 0
    QImage image("screenshot-20220123-225755803973.png");

    InferenceBoxScope box(overlay, 0.32, 0.87, 0.03, 0.04);
    image = extract_box(image, box);
    PackedBinaryMatrix matrix = compress_rgb32_to_binary_range(
        image,
        128, 255,
        128, 255,
        128, 255
    );
    std::vector<WaterfillObject> objects = find_objects_inplace(matrix, 20, false);
    cout << objects.size() << endl;

//    int c = 0;
//    for (const auto& object : objects){
//        extract_box(image, object).save("test-" + QString::number(c++) + ".png");
//    }

    WaterfillObject object = objects[0];
    object.merge_assume_no_overlap(objects[1]);
    extract_box(image, object).save("Sparkle.png");

#endif





#if 0
    BlackBorderDetector detector;
    VideoOverlaySet overlays(overlay);
    detector.make_overlays(overlays);
    detector.detect(feed.snapshot());
#endif


#if 0
    ExperienceGainDetector detector;
    VideoOverlaySet overlays(overlay);
    detector.make_overlays(overlays);
    detector.detect(feed.snapshot());
#endif



#if 0
    ShinyEncounterTracker tracker(logger, overlay, BattleType::STANDARD);
    {
        VisualInferenceSession session(env, console, feed, overlay);
        session += tracker;
        session.run();
    }

    DoublesShinyDetection wild_result;
    ShinyDetectionResult your_result;
    determine_shiny_status(
        logger,
        wild_result, your_result,
        tracker.dialog_tracker(),
        tracker.sparkles_wild_overall(),
        tracker.sparkles_wild_left(),
        tracker.sparkles_wild_right(),
        tracker.sparkles_own()
    );
#if 0
    determine_shiny_status(
        logger,
        SHINY_BATTLE_REGULAR,
        tracker.dialog_timer(),
        tracker.sparkles_wild()
    );
#endif
#endif



#if 0
    VisualInferenceSession session(env, feed, overlay);
    ShinySparkleTracker tracker(overlay);
    session += tracker;
    session.run();
#endif








#if 0
    QImage image = feed.snapshot();
    PokemonSwSh::SparkleSet sparkles = PokemonSwSh::find_sparkles(image);

    VideoOverlaySet overlays(overlay);
    sparkles.draw_boxes(overlays, image, {0, 0, 1, 1});
#endif





#if 0
    Kernels::PackedBinaryMatrix matrix0;
    matrix0 = compress_rgb32_to_binary_min(image, 192, 192, 0);

//    std::vector<WaterfillObject> objects = find_objects_inplace(matrix0, 10, false);
    WaterFillIterator finder(matrix0, 20);
    WaterfillObject object;
    size_t c = 0;
    VideoOverlaySet overlays(overlay);
    while (finder.find_next(object)){
        image.copy(object.min_x, object.min_y, object.width(), object.height()).save("test-" + QString::number(c) + ".png");
        cout << c++ << endl;
        PokemonSwSh::RadialSparkleDetector radial(object);
        if (radial.is_ball()){
            overlays.add(
                COLOR_GREEN,
                translate_to_parent(
                    image, {0, 0, 1, 1},
                    {object.min_x, object.min_y, object.max_x, object.max_y}
                )
            );
            continue;
        }
        if (radial.is_star()){
            overlays.add(
                COLOR_BLUE,
                translate_to_parent(
                    image, {0, 0, 1, 1},
                    {object.min_x, object.min_y, object.max_x, object.max_y}
                )
            );
            continue;
        }
        if (PokemonSwSh::is_line_sparkle(object, image.width() * 0.25)){
            overlays.add(
                COLOR_YELLOW,
                translate_to_parent(
                    image, {0, 0, 1, 1},
                    {object.min_x, object.min_y, object.max_x, object.max_y}
                )
            );
            continue;
        }
        if (PokemonSwSh::is_square_sparkle(object)){
            overlays.add(
                COLOR_MAGENTA,
                translate_to_parent(
                    image, {0, 0, 1, 1},
                    {object.min_x, object.min_y, object.max_x, object.max_y}
                )
            );
        }
    }
#endif



//    PokemonSwSh::SelectionArrowFinder finder(overlay, ImageFloatBox(0.640, 0.600, 0.055, 0.380));

//    QImage image("screenshot-20220108-185053570093.png");
//    cout << finder.detect(image) << endl;

//    QImage image("20220111-124433054843-PathPartyReader-ReadHP.png");
//    QImage image("20220116-044701249467-ReadPathSide.png");

//    cout << (int)PokemonSwSh::MaxLairInternal::read_side(image) << endl;

#if 0
    QImage image("20220116-053836954926-ReadPath.png");

    std::multimap<double, std::pair<PokemonType, ImagePixelBox>> candidates = PokemonSwSh::find_symbols(image, 0.20);


//    std::deque<InferenceBoxScope> hits;
//    hits.clear();
    cout << "---------------" << endl;
    for (const auto& item : candidates){
        cout << get_type_slug(item.second.first) << ": " << item.first << endl;
//        hits.emplace_back(overlay, translate_to_parent(screen, box, item.second.second), COLOR_GREEN);
    }
#endif






//    QImage image("20220111-124433054843-PathPartyReader-ReadHP.png");

//    NintendoSwitch::PokemonSwSh::MaxLairInternal::PathReader detector(overlay, 0);
//    double hp[4];
//    detector.read_hp(logger, image, hp);



#if 0
    MapDetector detector;
    VideoOverlaySet set(overlay);
    detector.make_overlays(set);

    cout << detector.detect(feed.snapshot()) << endl;
#endif



//    QImage image("screenshot-20220108-185053570093.png");
//    PokemonSwSh::MaxLairInternal::BattleMenuReader reader(overlay, Language::English);

//    cout << reader.can_dmax(image) << endl;




//    SelectionArrowFinder detector(overlay, {0.50, 0.58, 0.40, 0.10}, COLOR_RED);
//    detector.detect(feed.snapshot());



//    InferenceBoxScope box(overlay, {0.23, 0.30, 0.35, 0.30});


#if 0
    QImage image("screenshot-20220103-011451179122.png");

    PackedBinaryMatrix matrix = filter_rgb32_range(
        image,
        192, 255,
        0, 160,
        0, 192
    );

    std::vector<WaterfillObject> objects = find_objects(matrix, 100, false);
    VideoOverlaySet set(overlay);
    size_t c = 0;
    for (const WaterfillObject& object : objects){
        ImagePixelBox box(object.min_x, object.min_y, object.max_x, object.max_y);
        ImageFloatBox fbox = translate_to_parent(image, {0, 0, 1, 1}, box);
        set.add(COLOR_RED, fbox);

        image.copy(object.min_x, object.min_y, object.width(), object.height()).save("test-" + QString::number(c++) + ".png");
    }
#endif


#if 0
    QImage image("ExclamationTop-0.png");

    for (int r = 0; r < image.height(); r++){
        for (int c = 0; c < image.width(); c++){
            uint32_t pixel = image.pixel(c, r);
            cout << "(" << qRed(pixel) << "," << qGreen(pixel) << "," << qBlue(pixel) << ")";
        }
        cout << endl;
    }
#endif



#if 0
    QImage image("QuestionTop-0.png");
    image = image.convertToFormat(QImage::Format_ARGB32);

    uint32_t* ptr = (uint32_t*)image.bits();
    size_t words = image.bytesPerLine() / sizeof(uint32_t);

    for (int r = 0; r < image.height(); r++){
        for (int c = 0; c < image.width(); c++){
            uint32_t pixel = ptr[r*words + c];
            if (qRed(pixel) + qGreen(pixel) + qBlue(pixel) < 50){
                ptr[r*words + c] = 0;
            }
        }
    }

    image.save("QuestionTop-1.png");
#endif


#if 0
    cout << std::hex << QColor("green").rgb() << endl;
    cout << std::hex << QColor(Qt::green).rgb() << endl;
    cout << std::hex << QColor(Qt::darkGreen).rgb() << endl;
    cout << std::hex << QColor(Qt::darkCyan).rgb() << endl;
#endif


#if 0
    QImage image("screenshot-20211227-082121670685.png");
    image = extract_box(image, ImageFloatBox({0.95, 0.10, 0.05, 0.10}));
    image.save("test.png");


    BinaryImage binary_image = filter_rgb32_range(
        image,
        255, 255,
        128, 255,
        0, 128,
        0, 128
    );
    cout << binary_image.dump() << endl;
#endif


#if 0
    ShortDialogDetector detector;
    OverlaySet overlays(overlay);
    detector.make_overlays(overlays);

    cout << detector.detect(QImage("20211228-013942613330.jpg")) << endl;
//    cout << detector.detect(feed.snapshot()) << endl;
#endif



#if 0
    BoxShinyDetector detector;
    cout << detector.detect(QImage("20211226-031611120900.jpg")) << endl;

//    pbf_mash_button(console, BUTTON_X, 10 * TICKS_PER_SECOND);
#endif


#if 0
    BattleMenuDetector detector(BattleType::WILD);
    OverlaySet overlays(overlay);
    detector.make_overlays(overlays);
#endif


    env.wait_for(std::chrono::seconds(60));


}





}
}





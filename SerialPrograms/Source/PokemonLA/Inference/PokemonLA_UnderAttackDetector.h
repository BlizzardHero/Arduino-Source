/*  Under Attack Detector
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */


#ifndef PokemonAutomation_PokemonLA_UnderAttackDetector_H
#define PokemonAutomation_PokemonLA_UnderAttackDetector_H

#include <deque>
#include <map>
#include "Common/Cpp/SpinLock.h"
#include "CommonFramework/Logging/LoggerQt.h"
#include "CommonFramework/ImageTools/ImageBoxes.h"
#include "CommonFramework/InferenceInfra/VisualInferenceCallback.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonLA{


enum class UnderAttackState{
    UNKONWN,
    SAFE,
    UNDER_ATTACK,
};
extern const char* UNDER_ATTACK_STRINGS[];


class UnderAttackWatcher : public VisualInferenceCallback{
public:

public:
    UnderAttackWatcher(LoggerQt& logger);

    UnderAttackState state() const{
        return m_state.load(std::memory_order_acquire);
    }

    virtual void make_overlays(VideoOverlaySet& items) const override;
    virtual bool process_frame(
        const QImage& frame,
        std::chrono::system_clock::time_point timestamp
    ) override;

private:
    UnderAttackState detect(const QImage& frame);

private:
    struct Sample{
        std::chrono::system_clock::time_point timestamp;
        UnderAttackState state;
    };

    LoggerQt& m_logger;
    ImageFloatBox m_box;

    std::atomic<UnderAttackState> m_state;

    SpinLock m_lock;
    std::deque<Sample> m_history;
    std::map<UnderAttackState, size_t> m_counts;
};



}
}
}
#endif

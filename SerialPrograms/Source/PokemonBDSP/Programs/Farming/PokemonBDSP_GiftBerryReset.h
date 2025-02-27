/*  Gift Berry Reset
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_PokemonBDSP_GiftBerryReset_H
#define PokemonAutomation_PokemonBDSP_GiftBerryReset_H

#include "NintendoSwitch/Framework/NintendoSwitch_SingleSwitchProgram.h"
#include "CommonFramework/Notifications/EventNotificationsTable.h"
#include "CommonFramework/OCR/OCR_LanguageOptionOCR.h"
#include "NintendoSwitch/Options/GoHomeWhenDoneOption.h"
#include "PokemonBDSP/Options/PokemonBDSP_BerrySelector.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonBDSP{


class GiftBerryReset_Descriptor : public RunnableSwitchProgramDescriptor{
public:
    GiftBerryReset_Descriptor();
};


class GiftBerryReset : public SingleSwitchProgramInstance{
public:
    GiftBerryReset(const GiftBerryReset_Descriptor& descriptor);

    virtual std::unique_ptr<StatsTracker> make_stats() const override;
    virtual void program(SingleSwitchProgramEnvironment& env) override;


private:
    struct Stats;

private:
    GoHomeWhenDoneOption GO_HOME_WHEN_DONE;

    OCR::LanguageOCR LANGUAGE;
    
    BerrySelectorOption TARGET_BERRIES;

    EventNotificationOption NOTIFICATION_STATUS_UPDATE;
    EventNotificationOption NOTIFICATION_PROGRAM_FINISH;
    EventNotificationsOption NOTIFICATIONS;
};




}
}
}
#endif

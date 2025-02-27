/*  God Egg Duplication
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_PokemonSwSh_GodEggDuplication_H
#define PokemonAutomation_PokemonSwSh_GodEggDuplication_H

#include "CommonFramework/Options/BooleanCheckBoxOption.h"
#include "CommonFramework/Options/SimpleIntegerOption.h"
#include "NintendoSwitch/Options/StartInGripMenuOption.h"
#include "NintendoSwitch/Framework/NintendoSwitch_SingleSwitchProgram.h"
#include "PokemonSwSh_EggHelpers.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonSwSh{


class GodEggDuplication_Descriptor : public RunnableSwitchProgramDescriptor{
public:
    GodEggDuplication_Descriptor();
};



class GodEggDuplication : public SingleSwitchProgramInstance{
public:
    GodEggDuplication(const GodEggDuplication_Descriptor& descriptor);

    void collect_godegg(const BotBaseContext& context, uint8_t party_slot) const;
    void run_program(SingleSwitchProgramEnvironment& env, uint16_t attempts) const;
    virtual void program(SingleSwitchProgramEnvironment& env) override;

private:
    StartInGripOrGameOption START_IN_GRIP_MENU;
    SimpleIntegerOption<uint16_t> MAX_FETCH_ATTEMPTS;
    SimpleIntegerOption<uint8_t> PARTY_ROUND_ROBIN;
};


}
}
}
#endif

/*  Activate Menu Glitch (1.1.3)
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_PokemonBDSP_ActivateMenuGlitch113_H
#define PokemonAutomation_PokemonBDSP_ActivateMenuGlitch113_H

#include "NintendoSwitch/Options/TimeExpressionOption.h"
#include "NintendoSwitch/Framework/NintendoSwitch_SingleSwitchProgram.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonBDSP{


class ActivateMenuGlitch113_Descriptor : public RunnableSwitchProgramDescriptor{
public:
    ActivateMenuGlitch113_Descriptor();
};



class ActivateMenuGlitch113 : public SingleSwitchProgramInstance{
public:
    ActivateMenuGlitch113(const ActivateMenuGlitch113_Descriptor& descriptor);

    virtual void program(SingleSwitchProgramEnvironment& env) override;

private:
    TimeExpressionOption<uint16_t> FLY_A_TO_X_DELAY;
};



}
}
}
#endif

/*  Overworld Watcher
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_PokemonLA_OverworldWatcher_H
#define PokemonAutomation_PokemonLA_OverworldWatcher_H

#include "CommonFramework/Notifications/EventNotificationsTable.h"
#include "NintendoSwitch/Framework/NintendoSwitch_SingleSwitchProgram.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonLA{


class OverworldWatcher_Descriptor : public RunnableSwitchProgramDescriptor{
public:
    OverworldWatcher_Descriptor();
};


class OverworldWatcher : public SingleSwitchProgramInstance{
public:
    OverworldWatcher(const OverworldWatcher_Descriptor& descriptor);

    virtual void program(SingleSwitchProgramEnvironment& env) override;


};





}
}
}
#endif

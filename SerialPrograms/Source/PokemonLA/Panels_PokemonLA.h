/*  Pokemon Legends Arceus Panels
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#ifndef PokemonAutomation_PokemonLA_Panels_H
#define PokemonAutomation_PokemonLA_Panels_H

#include "CommonFramework/Panels/PanelList.h"

namespace PokemonAutomation{
namespace NintendoSwitch{
namespace PokemonLA{


class Panels : public PanelList{
public:
    Panels(QTabWidget& parent, PanelListener& listener);
};



}
}
}
#endif

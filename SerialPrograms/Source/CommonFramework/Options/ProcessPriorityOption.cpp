/*  Process Priority Option
 *
 *  From: https://github.com/PokemonAutomation/Arduino-Source
 *
 */

#include "CommonFramework/Environment/Environment.h"
#include "EnumDropdownWidget.h"
#include "ProcessPriorityOption.h"

namespace PokemonAutomation{



class ProcessPriorityWidget : public EnumDropdownWidget{
public:
    ProcessPriorityWidget(QWidget& parent, EnumDropdownOption& value)
         : EnumDropdownWidget(parent, value)
    {
        connect(
            m_box, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [=](int index){
                if (index < 0){
                    return;
                }
                set_priority_by_index(index);
            }
        );
    }
};





ProcessPriorityOption::ProcessPriorityOption()
    : EnumDropdownOption(
        "<b>Process Priority:</b><br>"
        "Set the priority of this process.<br>"
        "Higher priority may reduce errors due to CPU starvation from background programs. "
        "Lower priority may improve system responsiveness if your computer isn't powerful enough.<br>"
        "On Mac and Linux, you may need to restart the program for this to properly take effect.",
        PRIORITY_MODES, DEFAULT_PRIORITY_INDEX
    )
{}

void ProcessPriorityOption::update_priority_to_option() const{
    if (PRIORITY_MODES.size() > 1){
        set_priority_by_name(current_case());
    }
}

ConfigWidget* ProcessPriorityOption::make_ui(QWidget& parent){
    return new ProcessPriorityWidget(parent, *this);
}



}

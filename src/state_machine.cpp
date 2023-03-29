/**
 * @file state_machine.cpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "state_machine.hpp"

namespace ethercat_interface
{
    namespace state_machine
    {
        namespace CIA402
        {
            StateMachine::StateMachine(){};
            
            StateMachine::StateMachine(uint16_t* slave_status_ptr)
            {
                m_SlaveStatusPtr = std::make_unique<uint16_t>(slave_status_ptr);
            }
        }
    }
}
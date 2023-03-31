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
            
            StateMachine::StateMachine(std::shared_ptr<uint16_t>& slave_status_ptr)
            {
                m_SlaveStatusPtr = slave_status_ptr;

            }

            void StateMachine::setWriteCallback(std::function<void(const std::string&, const uint16_t&, int)> function_ptr)
            {
                writeControlCommand = function_ptr;
            }

            bool StateMachine::switchState(const State& target_state)
            {
                bool isSwitchSuccessful = false;

                switch (target_state)
                {
                case State::SwitchOnDisabled:
                    
                case State::ReadyToSwitchOn:
                    
                    break;
                case State::SwitchedOn:
                    
                    break;
                case State::OperationEnabled:
                    
                    break;
                case State::QuickStopActive:
                    
                    break;
                default:
                    isSwitchSuccessful = false;
                    break;
                }
                
                return isSwitchSuccessful;
            }


            bool statusCheck(const uint16_t& current_status, const State& target_state)
            {
                auto skip_indexes = StatusHelper::getNotImportantBits(target_state);

                uint8_t first = (uint8_t)(current_status & 0x00FF);
                uint8_t second = (uint8_t)(get(target_state) & 0x00FF);

                uint8_t mask = 1;
                for (int i = 0; i < 8; i++) {
                  if (std::find(skip_indexes.begin(), skip_indexes.end(), i) == skip_indexes.end()) {
                    if ((current_status & mask) != (get(target_state) & mask)) {
                      return false;
                    }
                  }
                  mask <<= 1;
                }
                return true;
            }
            
            StatusHelper::UnimportantBitIndexes StatusHelper::NotReadyToSwitchOn = std::vector<int>{4, 7};
            StatusHelper::UnimportantBitIndexes StatusHelper::SwitchOnDisabled = std::vector<int>{4, 5, 7};
            StatusHelper::UnimportantBitIndexes StatusHelper::ReadyToSwitchOn = std::vector<int>{4, 7};
            StatusHelper::UnimportantBitIndexes StatusHelper::SwitchedOn = std::vector<int>{4, 7};
            StatusHelper::UnimportantBitIndexes StatusHelper::OperationEnabled = std::vector<int>{4, 7};
            StatusHelper::UnimportantBitIndexes StatusHelper::QuickStopActive = std::vector<int>{4, 5, 7};
            StatusHelper::UnimportantBitIndexes StatusHelper::FaultResponseActive = std::vector<int>{4, 5, 7};
            StatusHelper::UnimportantBitIndexes StatusHelper::Fault = std::vector<int>{4, 5, 7};

        }
    
    }
}

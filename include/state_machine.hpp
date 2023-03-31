/**
 * @file state_machine.hpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-29
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <iostream>
#include <memory>
#include <functional>
#include <unordered_map>

#include "slave.hpp"

namespace ethercat_interface
{   
    namespace state_machine
    {
        
        namespace CIA402
        {
            enum class State : uint16_t;
            enum class ControlCommand : uint16_t;
            
            class StateMachine
            {
                public:
                
                StateMachine();
                StateMachine(std::shared_ptr<uint16_t>& slave_status_ptr);

                ~StateMachine();

                std::weak_ptr<uint16_t> m_SlaveStatusPtr; 

                inline void setStatusPtr(std::shared_ptr<uint16_t>& slave_status_ptr)
                {
                    m_SlaveStatusPtr = slave_status_ptr;
                }

                void setWriteCallback(std::function<void(const std::string&, const uint16_t&, int)> function_ptr);
                
                bool switchState(const State& target_state);

                private:

                std::function<void(const std::string&, const uint16_t&, int)> writeControlCommand;
                
                State m_CurrentState = State::SwitchOnDisabled; 

                typedef std::pair<State, Command> Transition;
                typedef std::vector<Transition> NextStates;

                const std::unordered_map<State, NextStates> m_TransitionTable = {
                    {State::SwitchOnDisabled, 
                        {
                            {State::ReadyToSwitchOn, Command::Shutdown}
                        }
                    },
                    {State::ReadyToSwitchOn, 
                        {
                            {State::SwitchedOn, Command::SwitchOn}, 
                            {State::SwitchOnDisabled, Command::QuickStop}
                        }
                    },
                    {State::SwitchedOn, 
                        {
                            {State::OperationEnabled, Command::EnableOperation}, 
                            {State::SwitchOnDisabled, Command::QuickStop}
                        }
                    },
                    {State::OperationEnabled, 
                        {
                            {State::ReadyToSwitchOn, Command::Shutdown},
                            {State::QuickStopActive, Command::QuickStop}, 
                            {State::SwitchedOn, Command::DisableOperation}
                        }
                    },
                    {State::Fault, 
                    {
                        {State::SwitchOnDisabled, Command::ResetFault}
                    }
                    }
                };
                
                
            };

            bool statusCheck(const uint16_t& current_status, const State& target_state);

            enum class State : uint16_t
            {
                NotReadyToSwitchOn = 0x00,
                SwitchOnDisabled = 0x40,
                ReadyToSwitchOn = 0x21,
                SwitchedOn = 0x23,
                OperationEnabled = 0x27,
                QuickStopActive = 0x07,
                FaultResponseActive = 0x0F,
                Fault = 0x008
            };

            inline uint16_t get(const State& state)
            {
                return static_cast<std::underlying_type<State>::type>(state);
            }


            enum class Command : uint16_t
            {
                Shutdown = 0x06, // Changes the device status from "Switch On disabled" to "Ready to switch on". 
                SwitchOn = 0x07, // Deactivates the switch on inhibit., 
                EnableOperation = 0x000f, // Enables the operation and actives quick stop again. 
                QuickStop = 0x02, // Activates Quick Stop.
                DisableOperation = 0x01f, // Disables the operation
                DisableVoltage = 0x01, // Inhibits the output stages of the controller.
                ResetFault = 0x80 // Acknowledges an existing error message. Requires a 0 to 1 rising edge.
            };

            inline uint16_t get(const Command& command)
            {
                return static_cast<std::underlying_type<Command>::type>(command);
            }

            class StatusHelper
            {
                public:
                /**
                 * @brief Pair first: StatusType Enum, second: Vector of integers containing the unimportant bit's indexes.
                 * 
                 */
                typedef std::vector<int> UnimportantBitIndexes;
                static UnimportantBitIndexes getNotImportantBits(State type)
                {
                    switch (type)
                    {
                    case State::NotReadyToSwitchOn:
                        return NotReadyToSwitchOn;
                        break;
                    case State::SwitchOnDisabled:
                        return SwitchOnDisabled;
                        break;
                    case State::ReadyToSwitchOn:
                        return ReadyToSwitchOn;
                        break;
                    case State::SwitchedOn:
                        return SwitchedOn;
                        break;
                    case State::OperationEnabled:
                        return OperationEnabled;
                        break;
                    case State::QuickStopActive:
                        return QuickStopActive;
                        break;
                    case State::FaultResponseActive:
                        return FaultResponseActive;
                        break;
                    case State::Fault:
                        return Fault;
                        break;
                    default:
                        break;
                    }
                }
                static UnimportantBitIndexes NotReadyToSwitchOn;
                static UnimportantBitIndexes SwitchOnDisabled;
                static UnimportantBitIndexes ReadyToSwitchOn;
                static UnimportantBitIndexes SwitchedOn;
                static UnimportantBitIndexes OperationEnabled;
                static UnimportantBitIndexes QuickStopActive;
                static UnimportantBitIndexes FaultResponseActive;
                static UnimportantBitIndexes Fault;
            
            };
            
        }
    }
}
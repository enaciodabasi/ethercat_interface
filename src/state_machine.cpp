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

#include "ethercat_interface/state_machine.hpp"

namespace ethercat_interface
{


    namespace state_machine
    {

    namespace CIA402
    {

        const State detectCurrentState(const uint16_t& status_word)
        {
            State currentState = State::Unknown;

            const uint16_t state = status_word & (sod | qs | f | oe | so | rtso);

            switch(state)
            {

                case ( 0 | 0 | 0 | 0 | 0 | 0 ):
                case ( 0 | qs | 0 | 0 | 0 | 0 ):
                    currentState = State::NotReadyToSwitchOn;
                    break;

                case ( sod | 0 | 0 | 0 | 0 | 0 ):
                case ( sod | qs | 0 | 0 | 0 | 0 ):
                    currentState = State::SwitchOnDisabled;
                    break;

                case ( 0 | qs | 0 | 0 | 0 | rtso ):
                    currentState = State::ReadyToSwitchOn;
                    break;

                case ( 0 | qs | 0 | 0 | so | rtso ):
                    currentState = State::SwitchedOn;
                    break;

                case ( 0 | qs | 0 | oe | so | rtso ):
                    currentState = State::OperationEnabled;
                    break;

                case ( 0 | 0 | 0 | oe | so | rtso ):
                    currentState = State::QuickStopActive;
                    break;

                case ( 0 | 0 | f | oe | so | rtso ):
                case ( 0 | qs | f | oe | so | rtso ):
                    currentState = State::FaultReactionActive;
                    break;

                case ( 0 | 0 | f | 0 | 0 | 0 ):
                case ( 0 | qs | f | 0 | 0 | 0 ):
                    currentState = State::Fault;
                    break;

                default:
                    currentState = State::Unknown;
                    break;

            }
            

            return currentState;
        }

        StateMachine::StateMachine()
        {
            
        }

        StateMachine::~StateMachine()
        {

        }

        bool StateMachine::init()
        {
            /* 0. */
            this->addTransition(
                State::Start,
                State::NotReadyToSwitchOn,
                std::make_pair(
                    automaticTransitionSet,
                    automaticTransitionReset
                )
            );
            /* 1. */    
            this->addTransition(
                State::NotReadyToSwitchOn,
                State::SwitchOnDisabled,
                std::make_pair(
                    automaticTransitionSet,
                    automaticTransitionReset
                )
            );
            /* 2. */
            this->addTransition(
                State::SwitchOnDisabled,
                State::ReadyToSwitchOn,
                std::make_pair(
                    shutdownSet,
                    shutdownReset
                )
            );
            /* 3. */
            this->addTransition(
                State::ReadyToSwitchOn,
                State::SwitchedOn,
                std::make_pair(
                    switchOnSet,
                    switchOnReset
                )
            );
            /* 4. */
            this->addTransition(
                State::SwitchedOn,
                State::OperationEnabled,
                std::make_pair(
                    enableOperationSet,
                    enableOperationReset
                )
            );
            /* 5. */
            this->addTransition(
                State::OperationEnabled,
                State::SwitchedOn,
                std::make_pair(
                    switchOnSet,
                    switchOnReset
                )
            );
            /* 6. */
            this->addTransition(
                State::SwitchedOn,
                State::ReadyToSwitchOn,
                std::make_pair(
                    shutdownSet,
                    shutdownReset
                )
            );
            
            /* 7. */
            this->addTransition(
                State::ReadyToSwitchOn,
                State::QuickStopActive,
                std::make_pair(
                    quickStopSet,
                    quickStopReset
                )
            );

            /* 8. */
            this->addTransition(
                State::OperationEnabled,
                State::ReadyToSwitchOn,
                std::make_pair(
                    shutdownSet,
                    shutdownReset
                )
            );

            /* 9. */
            this->addTransition(
                State::OperationEnabled,
                State::SwitchOnDisabled,
                std::make_pair(
                    disableVoltageSet,
                    disableVoltageReset
                )
            );

            /* 10. */
            this->addTransition(
                State::SwitchedOn,
                State::SwitchOnDisabled,
                std::make_pair(
                    disableVoltageSet,
                    disableVoltageReset
                )
            );

            /* 11. */
            this->addTransition(
                State::OperationEnabled,
                State::QuickStopActive,
                std::make_pair(
                    quickStopSet,
                    quickStopReset
                )
            );

            /* 12. */
            this->addTransition(
                State::QuickStopActive,
                State::SwitchOnDisabled,
                std::make_pair(
                    disableVoltageSet,
                    disableVoltageReset
                )
            );

            /* 14. */
            this->addTransition(
                State::OperationEnabled,
                State::SwitchOnDisabled,
                std::make_pair(
                    disableVoltageSet,
                    disableVoltageReset
                )
            );

            /* 15. */
            this->addTransition(
                State::Fault,
                State::SwitchOnDisabled,
                std::make_pair(
                    faultResetSet,
                    faultResetReset
                )
            );

            /* 16. */
            this->addTransition(
                State::QuickStopActive,
                State::OperationEnabled,
                std::make_pair(
                    enableOperationSet,
                    enableOperationReset
                )
            );

            return true;
        }

        std::optional<uint16_t> StateMachine::getControlWord(
            const uint16_t& status_word,
            const State& target_state
        )
        {
            const auto currentState = detectCurrentState(
                status_word
            );

            if(currentState == State::Unknown)
            {
                return std::nullopt;
            }

            const auto nextState = this->enableOperationSubMachine(
                currentState
            );
            if(nextState == std::nullopt)
            {
                return std::nullopt;
            }

            uint16_t newCW = m_LastControlWord;
            if(!getRequiredCommand(std::make_pair(currentState, nextState.value()), newCW))
            {
                return std::nullopt;
            }

            m_LastControlWord = newCW;

            return m_LastControlWord;

        }

        bool StateMachine::getRequiredCommand(
            const Transition& transition, 
            uint16_t& control_word
        )
        {

            const auto cmdFound = m_TransitionTable.find(transition);
            if(cmdFound == m_TransitionTable.end())
            {
                return false;
            }
            const auto cmd = cmdFound->second; // Get the value from the iterator.

            control_word = (control_word & ~cmd.second) | cmd.first;
            
            return true;
        }

    }

    } //namespace state_machine

}
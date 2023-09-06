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

#ifndef STATE_MACHINE_HPP
#define STATE_MACHINE_HPP

#include <iostream>
#include <memory>
#include <functional>
#include <map>
#include <unordered_map>
#include <optional>

#include "defs.hpp"

namespace ethercat_interface
{
    namespace state_machine
    {
    
    namespace CIA402
    {

        /**
         * @brief pair.first = Bits to set, pair.second = Bits to reset.
         * 
         */
        using Command = std::pair<uint16_t, uint16_t>;
        
        /**
         * @brief pair.first = current state to transition from, pair.second = state to transition to.
         * 
         */
        using Transition = std::pair<State, State>;
        
        using TransitionTable = std::map<Transition, Command>;
    
        namespace
        {
            inline constexpr uint16_t rtso = (1 << getStatusWordIndex(StatusWord::ReadyToSwitchOn));
            inline constexpr uint16_t so = (1 << getStatusWordIndex(StatusWord::SwitchedOn));
            inline constexpr uint16_t oe = (1 << getStatusWordIndex(StatusWord::OperationEnabled));
            inline constexpr uint16_t f = (1 << getStatusWordIndex(StatusWord::Fault));
            inline constexpr uint16_t qs = (1 << getStatusWordIndex(StatusWord::QuickStop));
            inline constexpr uint16_t sod = (1 << getStatusWordIndex(StatusWord::SwitchonDisabled));
        }

        const State detectCurrentState(const uint16_t& status_word, State& last_state);

        inline constexpr uint16_t automaticTransitionSet = 0;
        inline constexpr uint16_t automaticTransitionReset = 0;

        inline constexpr uint16_t shutdownSet = ((1 << getControlWordIndex(ControlWord::QuickStop)) | (1 << getControlWordIndex(ControlWord::EnableVoltage)));
        inline constexpr uint16_t shutdownReset = ((1 << getControlWordIndex(ControlWord::FaultReset)) | (1 << getControlWordIndex(ControlWord::SwitchOn)));
        
        inline constexpr uint16_t switchOnSet = ((1 << getControlWordIndex(ControlWord::QuickStop)) | (1 << getControlWordIndex(ControlWord::EnableVoltage)) | (1 << getControlWordIndex(ControlWord::SwitchOn)));
        inline constexpr uint16_t switchOnReset = ((1 << getControlWordIndex(ControlWord::FaultReset)) | (1 << getControlWordIndex(ControlWord::EnableOperation)));

        inline constexpr uint16_t disableVoltageSet = 0;
        inline constexpr uint16_t disableVoltageReset = ((1 << getControlWordIndex(ControlWord::FaultReset)) | (1 << getControlWordIndex(ControlWord::EnableVoltage)));
        
        inline constexpr uint16_t quickStopSet = (1 << getControlWordIndex(ControlWord::EnableVoltage));
        inline constexpr uint16_t quickStopReset = ((1 << getControlWordIndex(ControlWord::FaultReset)) | (1 << getControlWordIndex(ControlWord::QuickStop)));

        inline constexpr uint16_t enableOperationSet = ((1 << getControlWordIndex(ControlWord::QuickStop)) | (1 << getControlWordIndex(ControlWord::EnableVoltage)) | (1 << getControlWordIndex(ControlWord::SwitchOn)) | (1 << getControlWordIndex(ControlWord::EnableOperation)));
        inline constexpr uint16_t enableOperationReset = (1 << getControlWordIndex(ControlWord::FaultReset));

        inline constexpr uint16_t faultResetSet = (1 << getControlWordIndex(ControlWord::FaultReset));
        inline constexpr uint16_t faultResetReset = 0;

        const std::unordered_map<State, std::string> StateStrings = {
            {State::Unknown, "Unknown"},
            {State::Start, "Start"},
            {State::NotReadyToSwitchOn, "Not Ready To Switch On"},
            {State::SwitchOnDisabled, "Switch On Disabled"},
            {State::ReadyToSwitchOn, "Ready To Switch On"},
            {State::SwitchedOn, "Switched On"},
            {State::OperationEnabled, "Operation Enabled"},
            {State::QuickStopActive, "Quick Stop Active"},
            {State::FaultReactionActive, "Fault Reaction Active"},
            {State::Fault, "Fault"}
        };

        class StateMachine
        {
            public:

            StateMachine();

            ~StateMachine();

            bool init();

            inline uint16_t getCW()
            {
                return m_LastControlWord;
            }

            std::optional<uint16_t> getControlWord(
                const uint16_t& status_word,
                const State& target_state
            );

            bool getRequiredCommand(
                const Transition& transition,
                uint16_t& control_word
            );

            State m_LastState = State::Unknown;

            private:

            uint16_t m_LastControlWord = 0x00;

            TransitionTable m_TransitionTable;

            inline void addTransition(const State& from, const State& to, const Command& command)
            {
                m_TransitionTable[std::make_pair(from, to)] = std::move(command);
            }

            inline std::optional<State> enableOperationSubMachine(const State& current_state)
            {
                switch (current_state)
                {
                case State::Start:
                    return State::NotReadyToSwitchOn;    
                case State::Fault:
                    case State::NotReadyToSwitchOn:
                        return State::SwitchOnDisabled;
                case State::SwitchOnDisabled:
                    return State::ReadyToSwitchOn;
                case State::ReadyToSwitchOn:
                    return State::SwitchedOn;
                case State::SwitchedOn:
                    case State::QuickStopActive:
                    case State::OperationEnabled:
                        return State::OperationEnabled;
                case State::FaultReactionActive:
                    return State::Fault;
                default:
                    return std::nullopt;
                }

                return std::nullopt;
            }
            
        };  

    } // namespace CIA402

    } // namespace state_machine
} // namespace ethercat_interface

#endif // STATE_MACHINE_HPP
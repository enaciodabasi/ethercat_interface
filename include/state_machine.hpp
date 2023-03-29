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

#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/state.hpp>
#include <boost/statechart/event.hpp>
#include <boost/mpl/list.hpp>

namespace ethercat_interface
{   
    namespace state_machine
    {
        
        namespace CIA402
        {
            enum class State;
            enum class Command;
            
            class StateMachine
            {
                public:
                
                StateMachine();
                StateMachine(uint16_t* slave_status_ptr);
                ~StateMachine();

                std::unique_ptr<uint16_t> m_SlaveStatusPtr; 

                inline void setStatusPtr(uint16_t* slave_status_ptr)
                {
                    m_SlaveStatusPtr = std::make_unique<uint16_t>(slave_status_ptr);
                }
                
                private:

                // Forward declarations of the states:
                // Starting state: Drive Disabled
                struct DriveDisabled_m;
                // Inner states of the Drive Disabled state.
                struct NotReadyToSwitchOn;
                struct SwitchOnDisabled;
                struct ReadyToSwitchOn;

                // Drive Active
                struct DriveActive_m;
                // Inner states of the Drive Active State;
                struct SwitchedOn;
                struct OperationEnabled;
                struct QuickStopActive;

                // Fault
                struct Fault_m;
                struct FaultReactionActive;
                struct Fault;

                // -------------------- Events --------------------
                // ------------------------------------------------

                struct MoveToDriveDisabledState : boost::statechart::event<MoveToDriveDisabledState>{};

                struct MoveToDriveActiveState : boost::statechart::event<MoveToDriveActiveState>{};

                struct MoveToFaultState : boost::statechart::event<MoveToFaultState>{};

                struct MoveToReadyToSwitchOnState : boost::statechart::event<MoveToReadyToSwitchOnState>{};

                // -------------------- Definitions of the States ---------------------
                // --------------------------------------------------------------------

                struct m_StateMachine : boost::statechart::state_machine<m_StateMachine, DriveDisabled_m>
                {

                };

                struct DriveDisabled_m : boost::statechart::state<DriveDisabled_m, m_StateMachine, NotReadyToSwitchOn>
                {

                };

                struct NotReadyToSwitchOn : boost::statechart::state<NotReadyToSwitchOn, DriveDisabled_m>
                {

                };

                struct SwitchOnDisabled : boost::statechart::state<SwitchOnDisabled, DriveDisabled_m>
                {

                };

                struct DriveActive_m : boost::statechart::state<DriveActive_m, m_StateMachine, SwitchedOn>
                {

                };

                struct SwitchedOn : boost::statechart::state<SwitchedOn, DriveActive_m>
                {
                 
                };
                
                struct OperationEnabled : boost::statechart::state<OperationEnabled, DriveActive_m>
                {
                 
                };

                struct QuickStopActive : boost::statechart::state<QuickStopActive, DriveActive_m>
                {
                 
                };

                struct Fault_m : boost::statechart::state<Fault_m, m_StateMachine, FaultReactionActive>
                {

                };

                struct FaultReactionActive : boost::statechart::state<FaultReactionActive, Fault_m>
                {
                 
                };
                
                struct Fault : boost::statechart::state<Fault, Fault_m>
                {
                 
                };
                
            };
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
        }
    }
}
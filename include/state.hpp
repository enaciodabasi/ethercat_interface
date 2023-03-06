/**
 * @file defs.hpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */

/*

----------------------------------------
----------------------------------------
Source for the information regarding the meanings of the bits are taken from the docuemnt in the following link:
https://download.lenze.com/TD/E94AxHE__Technology%20Application%20CiA402%20Device%20profile__v1-2__EN.pdf
----------------------------------------
----------------------------------------
*/

#ifndef DEFS_HPP
#define DEFS_HPP

#include <iostream>
#include <bitset>

namespace ethercat_interface
{
    namespace slave
    {

        typedef uint16_t SlaveStatus;

        typedef uint16_t SlaveCommand;

        /**
         * @brief Control commands to send to the control word of the slave.
         * Each enum holds the value of its bit pattern regarding to the CIA402 Standart.
         */
        enum class ControlCommand : uint16_t
        {
            Shutdown = 0x06, // Changes the device status from "Switch On disabled" to "Ready to switch on". 
            SwitchOn = 0x07, // Deactivates the switch on inhibit., 
            EnableOperation = 0x000f, // Enables the operation and actives quick stop again. 
            QuickStop = 0x02, // Activates Quick Stop.
            DisableOperation = 0x01f, // Disables the operation
            DisableVoltage = 0x01, // Inhibits the output stages of the controller.
            ResetFault = 0x80 // Acknowledges an existing error message. Requires a 0 to 1 rising edge.
        };

        inline const uint16_t getCommandValue(const ControlCommand& ctrl_cmd)
        {
            return static_cast<std::underlying_type<ControlCommand>::type>(ctrl_cmd);
        }

        /**
         * @brief Status Words of the slave device.
         * Each enum holds the value of its bit pattern regarding to the CIA402 Standart.
         */
        enum class StatusType : uint16_t
        {
            NotReadyToSwitchOn = 0x00,
            SwitchOnDisabled = 0x28,
            ReadyToSwitchOn = 0x21,
            SwitchedOn = 0x23,
            OperationEnabled = 0x27,
            QuickStopActive = 0x07,
            FaultResponseActive = 0x0F,
            Fault = 0x008
        };

        inline uint16_t getStatusValue(const StatusType& status)
        {
            return static_cast<std::underlying_type<StatusType>::type>(status);
        }

        

    }
}

#endif
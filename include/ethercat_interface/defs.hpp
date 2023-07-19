/**
 * @file defs.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef DEFS_HPP_
#define DEFS_HPP_

#include <iostream>

namespace ethercat_interface
{
    enum class SlaveType
    {
        Coupler,
        Driver,
        DigitalInput,
        DigitalOutput,
        AnalogInput,
        AnalogOutput
    };

    namespace state_machine
    {

        namespace CIA402
        {
            enum class StatusWord
                {
                    ReadyToSwitchOn=0,
                    SwitchedOn=1,
                    OperationEnabled=2,
                    Fault=3,
                    VoltageEnabled=4,
                    QuickStop=5,
                    SwitchonDisabled=6,
                    Warning=7,
                    ManufacturerSpecific_0=8,
                    Remote=9,
                    TargetReached=10,
                    InternalLimit=11,
                    OperationModeSpecific_0=12,
                    OperationModeSpecific_1=13,
                    ManufacturerSpecific_1=14,
                    ManufacturerSpecific_2=15
                };

                inline constexpr int getStatusWordIndex(const StatusWord& sw)
                {
                    return static_cast<typename std::underlying_type<StatusWord>::type>(sw);
                }

                enum class ControlWord
                {
                    SwitchOn = 0,
                    EnableVoltage = 1,
                    QuickStop = 2,
                    EnableOperation = 3,
                    OperationModeSpecific0 = 4,
                    OperationModeSpecific1 = 5,
                    OperationModeSpecific2 = 6,
                    FaultReset = 7,
                    Halt = 8,
                    Operation_mode_specific3 = 9,
                    Reserved_0 = 10,
                    ManufacturerSpecific_0 = 11,
                    ManufacturerSpecific_1 = 12,
                    ManufacturerSpecific_2 = 13,
                    ManufacturerSpecific_3 = 14,
                    ManufacturerSpecific_4 = 15
                };

                inline constexpr int getControlWordIndex(const ControlWord& cw)
                {
                    return static_cast<typename std::underlying_type<ControlWord>::type>(cw);
                }

                enum class State
                {
                    Unknown = 0,
                    Start = 0,
                    NotReadyToSwitchOn = 1,
                    SwitchOnDisabled = 2,
                    ReadyToSwitchOn = 3,
                    SwitchedOn = 4,
                    OperationEnabled = 5,
                    QuickStopActive = 6,
                    FaultReactionActive = 7,
                    Fault = 8
                };
        }
    } 

} // End of namespace ethercat_interface

#endif
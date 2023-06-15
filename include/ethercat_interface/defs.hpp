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
}

#endif
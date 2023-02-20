/**
 * @file utils.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <string>
#include <vector>

namespace ethercat_interface
{
    namespace utilities
    {
        struct SlaveInfo
        {
            std::string slaveName;
            int vendorID;
            int productCode;
            int position;
            int alias;
            std::vector<int> inputPorts;
            std::vector<int> outputPorts;
        };
    } // End of namespace utilities

} // End of namespace ethercat_interface

#endif // UTILITIES_HPP
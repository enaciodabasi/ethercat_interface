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

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>

namespace ethercat_interface
{
    namespace utils
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
    } // End of namespace utils

} // End of namespace ethercat_interface

#endif // UTILS_HPP
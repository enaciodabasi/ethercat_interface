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

#include <iostream>
#include <string>
#include <vector>

#include <yaml-cpp/yaml.h>

namespace ethercat_interface
{
    namespace utilities
    {   
        struct PdoEntryInfo
        {
            std::vector<uint32_t> indexes;
            std::vector<uint8_t> subindexes;
            std::vector<uint16_t> bitLengths;

            void toString()
            {
                std::cout << "PDO Entry Indexes:" << std::endl;
                for(auto i : indexes)
                {
                    std::cout << i << std::endl; 
                }
                std::cout << "PDO Entry Subindexes:" << std::endl;
                for(auto i : subindexes)
                {   
                    std::cout << i << std::endl; 
                }
                std::cout << "PDO Entry Bit Lengths:" << std::endl;
                for(auto i : bitLengths)
                {   
                    std::cout << i << std::endl; 
                }
            }
        };
        struct SlaveInfo
        {
            std::string slaveName;
            int vendorID;
            int productCode;
            int position;
            int alias;
            
            PdoEntryInfo pdoEntryInfo;

            void toString()
            {
                std::cout << "Slave Name: " << slaveName << std::endl;
                std::cout << "Vendor ID: " << vendorID << std::endl;
                std::cout << "Product Code: " << productCode << std::endl;
                std::cout << "Slave Position: " << position << std::endl;
                std::cout << "Slave Alias: " << alias << std::endl;
                
                pdoEntryInfo.toString();
                
            }
        };

        SlaveInfo parse_config_file(
            const std::string& file_name,
            const std::string& slave_name
        );
    } // End of namespace utilities

} // End of namespace ethercat_interface

#endif // UTILITIES_HPP
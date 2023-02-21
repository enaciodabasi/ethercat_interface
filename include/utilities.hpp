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
#include <variant>
#include <optional>

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

        std::vector<std::optional<int>> detect_null_diffs(const std::vector<std::string>& diffs_with_nulls)
        {
            std::vector<std::optional<int>> newDiffs;
            newDiffs.resize(diffs_with_nulls.size());

            for(std::size_t i = 0; i < diffs_with_nulls.size(); i++)
            {
                if(diffs_with_nulls[i] == "NULL" || diffs_with_nulls[i] == "null")
                {
                    newDiffs[i] = std::nullopt;
                }
                else
                {
                    newDiffs[i] = std::stoi(diffs_with_nulls[i]);
                }
            }

            return newDiffs;
        }
        
        struct SlaveSyncInfo
        {
            
            std::size_t numSyncManagers;

            std::vector<int> syncManagerDirections;

            std::vector<int> numPDOs;

            //std::vector<std::string> pdoIndexDiff;
            std::vector<std::optional<int>> pdoIndexDiff;

            std::vector<int> watchdogModes;

            void toString()
            {      
                std::cout << "Number of sync managers: " << numSyncManagers << std::endl;

                std::cout << "Direction of each sync manager: " << std::endl;
                for(auto i : syncManagerDirections)
                {
                    std::cout << i << std::endl; 
                }
                std::cout << "Number of PDOs: " << std::endl;
                for(auto i : numPDOs)
                {   
                    std::cout << i << std::endl; 
                }
                std::cout << "PDO Entry Bit Lengths: " << std::endl;
                //for(auto i : pdoIndexDiff)
                //{   
                //    std::cout << i << std::endl; 
                //}
                std::cout << "Watchdog mode for each sync manager:" << std::endl;
                for(auto i : watchdogModes)
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

            SlaveSyncInfo slaveSyncInfo;

            void toString()
            {
                std::cout << "Slave Name: " << slaveName << std::endl;
                std::cout << "Vendor ID: " << vendorID << std::endl;
                std::cout << "Product Code: " << productCode << std::endl;
                std::cout << "Slave Position: " << position << std::endl;
                std::cout << "Slave Alias: " << alias << std::endl;
                
                pdoEntryInfo.toString();
                slaveSyncInfo.toString();
            }
        };

        SlaveInfo parse_config_file(
            const std::string& file_name,
            const std::string& slave_name
        );
    } // End of namespace utilities

} // End of namespace ethercat_interface

#endif // UTILITIES_HPP
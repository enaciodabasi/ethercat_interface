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

#include "ecrt.h"


namespace ethercat_interface
{
    namespace utilities
    {   
        struct PdoEntryInfo
        {
            std::vector<uint16_t> indexes;
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

        struct IoMappingInfo
        {
            uint16_t RxPDO_Address;
            uint16_t TxPDO_Address;
            unsigned int RxPDO_Size;
            unsigned int TxPDO_Size;

            void toString()
            {
                std::cout << "RxPDO start address: " << RxPDO_Address << std::endl;
                std::cout << "TxPDO start address: " << TxPDO_Address << std::endl;
                std::cout << "RxPDO size: " << RxPDO_Size << std::endl;
                std::cout << "TxPDO size: " << TxPDO_Size << std::endl;
            }
        };

        std::vector<std::optional<int>> detect_null_diffs(const std::vector<std::string>& diffs_with_nulls);
        
        std::vector<ec_direction_t> intToEcDirectionEnum(const std::vector<int>& directions);

        std::vector<ec_watchdog_mode_t> intToEcWatchdogEnum(const std::vector<int>& watchdog_modes);

        std::vector<uint8_t> toHexadecimal(const std::vector<uint8_t>& to_fix);

        struct SlaveSyncInfo
        {
            
            std::size_t numSyncManagers;

            std::vector<int> syncManagerDirections;

            std::vector<uint> numPDOs;

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
            IoMappingInfo ioMappingInfo;
            SlaveSyncInfo slaveSyncInfo;

            void toString()
            {
                std::cout << "Slave Name: " << slaveName << std::endl;
                std::cout << "Vendor ID: " << vendorID << std::endl;
                std::cout << "Product Code: " << productCode << std::endl;
                std::cout << "Slave Position: " << position << std::endl;
                std::cout << "Slave Alias: " << alias << std::endl;
                
                pdoEntryInfo.toString();
                ioMappingInfo.toString();
                slaveSyncInfo.toString();
            }
        };

        SlaveInfo parse_config_file(
            const std::string& file_name,
            const std::string& slave_name
        );

        struct DC_Info
        {
            uint16_t assign_activate;
            uint32_t sync0_cycle;
            int32_t sync0_shift;
            uint32_t sync1_cycle;
            int32_t sync1_shift;
        };

        DC_Info getDcInfo(
            const std::string& file_name,
            const std::string& slave_name
        );

    } // End of namespace utilities

} // End of namespace ethercat_interface

#endif // UTILITIES_HPP
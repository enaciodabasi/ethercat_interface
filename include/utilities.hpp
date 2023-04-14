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
#include <string_view>
#include <vector>
#include <variant>
#include <optional>
#include <map>

#include <time.h>
#include <stdint.h>
#include <sys/time.h>

#include <yaml-cpp/yaml.h>

#include "ecrt.h"


namespace ethercat_interface
{

    struct SlaveSyncInfo
        {
            SlaveSyncInfo();

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

        enum class SlaveType
        {
            Coupler,
            Driver,
            IO
        };

        struct DC_Info
        {
            DC_Info();
            uint16_t assign_activate;
            uint32_t sync0_cycle;
            int32_t sync0_shift;
            uint32_t sync1_cycle;
            int32_t sync1_shift;
        };

        struct IoMappingInfo
        {
            IoMappingInfo();
            uint16_t RxPDO_Address;
            uint16_t TxPDO_Address;
            unsigned int RxPDO_Size;
            unsigned int TxPDO_Size;
            std::vector<uint16_t> RxPDO_Indexes;
            std::vector<uint16_t> TxPDO_Indexes;
            void toString()
            {
                std::cout << "RxPDO Indexes:";
                for(auto r : RxPDO_Indexes)
                {
                    std::cout << "\n" << r << " ";
                }
                std::cout << "TxPDO Indexes:";
                for(auto t : TxPDO_Indexes)
                {
                    std::cout << "\n" << t << " ";
                }
            }
        };

    struct PdoEntryInfo
    {
        PdoEntryInfo();
        
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

    struct ControllerInfo
    {

        std::vector<std::string> domainNames;
        uint numOfDomains;
        std::string logDirPath;
    };

    struct StartupInfo
    {

    };

    struct SlaveInfo
    {
        SlaveInfo();
        std::string slaveName;
        SlaveType slaveType;
        int vendorID;
        int productCode;
        int position;
        int alias;
        
        std::string domainName;

        PdoEntryInfo pdoEntryInfo;
        IoMappingInfo ioMappingInfo;
        SlaveSyncInfo slaveSyncInfo;

        std::vector<std::string> pdoNames;
        
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
    
    namespace utilities
    {   
        
        namespace parser
        {   
            std::optional<ControllerInfo> parse_controller_config(std::string_view config_file_path);

            std::optional<StartupInfo> parse_startup_configs(); 
        }

        std::vector<std::optional<int>> detect_null_diffs(const std::vector<std::string>& diffs_with_nulls);
        
        std::vector<ec_direction_t> intToEcDirectionEnum(const std::vector<int>& directions);

        std::vector<ec_watchdog_mode_t> intToEcWatchdogEnum(const std::vector<int>& watchdog_modes);

        class toHexaHelper
        {
            public:

            toHexaHelper();

            
            private:

            std::map<char, uint8_t> charHexMap;

        };

        std::vector<uint8_t> toHexadecimal(const std::vector<uint>& to_fix);

        std::vector<uint8_t> fixSubindexes(const std::vector<uint>& to_fix);

        

        DC_Info getDcInfo(
            const std::string& file_name,
            const std::string& slave_name
        );

        SlaveInfo parse_config_file(
            const std::string& file_name,
            const std::string& slave_name
        );

        std::optional<std::vector<SlaveInfo>> parse_config_file(
            std::string_view file_name
        );

        // Contains functions and other helpers for debugging the EtherCAT Interface.
        namespace debug 
        {   
            // 
            auto constexpr NANOSEC_PER_SEC = 1000000000L;

            struct TimeMeasureInfo_s
            {
                struct timespec startTime{};
                struct timespec endTime{};
                struct timespec lastStartTime{};

                uint32_t period_ns{0};
                uint32_t period_min_ns{0};
                uint32_t period_max_ns{0};
                
                uint32_t latency_ns{0};
                uint32_t latency_min_ns{0};
                uint32_t latency_max_ns{0};
                
                uint32_t exec_ns{0};
                uint32_t exec_min_ns{0};
                uint32_t exec_max_ns{0};

                int usedClock = CLOCK_MONOTONIC;

                void updateMinMax();
                void resetMinMax();
                std::string getTimingStats();

                void updateEndTime();
            };

            void measureTime(TimeMeasureInfo_s& tmInfo_s, timespec wakeupTime);

            long timeDiff(timespec t1, timespec t2);

        } // End of namespace debug.

    } // End of namespace utilities

} // End of namespace ethercat_interface

#endif // UTILITIES_HPP
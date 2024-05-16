/**
 * @file common_defs.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef COMMON_DEFS_HPP_
#define COMMON_DEFS_HPP_

#include <string>
#include <vector>
#include <optional>
#include <map>

#include "ecrt.h"

namespace ethercat_interface
{

  enum class ReturnType
  {
    OK,
    UninitializedMasterAndDomain,
    MasterRequestError,
    MasterActivationError,
    EmptyConfigFile,
    NoSlaveInfoInConfigFile,
    SlaveAlreadyRegistered,
    ConfigurationFileParseError,
    ProcessDomainObjectRegisterError,
    DomainDataCreationError
  };

  enum class DataType
  {
      UINT8,
      INT8,
      UINT16,
      INT16,
      UINT32,
      INT32,
      UINT64,
      INT64,
      FLOAT,
      DOUBLE,
      UNKNOWN
  };
  
  struct ProgramConfig
  { 
    
    using DomainInfo = std::pair<std::string, double>; 
    uint16_t cyclePeriod;  
    std::vector<DomainInfo> domainInfo;   
    std::vector<SlaveInfo> slaveConfigurations;
  };

  enum class SlaveType
    {
        Driver,
        PLC,
        Coupler,
        IO
  
  };

  enum class PDO_Type
    {
        RxPDO,
        TxPDO
    };

  struct PDO
    {
        PDO_Type pdoType;

        uint16_t pdoAddress;

        std::vector<PDO_Entry> entries;
    };  

    struct PDO_Entry
    {

        std::string entryName;

        uint16_t index;
        
        uint8_t subindex;

        uint8_t bitlength;

        DataType type;
    };

    struct DistributedClockConfig
    {
        uint16_t assignActivate;

        uint32_t sync0Activate;

        int32_t sync0Shift;

        uint32_t sync1Activate;

        int32_t sync1Shift;

    };

    struct SyncManagerConfig
    {
        uint8_t index;
        
        ec_direction_t syncManagerDirection;

        ec_watchdog_mode_t watchdogMode;
    };

    /**
     * @brief Struct that holds all the information about the slave.
     * 
     */
    struct SlaveInfo
    {
        std::string slaveName;

        std::string domainName;

        SlaveType slaveType;

        uint16_t alias;

        uint16_t position;

        uint32_t vendorID;

        uint32_t productCode;
        
        std::vector<PDO> rxPDOs;

        std::vector<PDO> txPDOs;

        std::vector<SyncManagerConfig> syncManagerConfig;

        std::optional<DistributedClockConfig> distributedClockConfig;

        const std::string toString() const
        {
            std::string str;
            
            str += "Slave name: " + slaveName + " \n";
            str += "Alias: " + std::to_string(alias) + " Position: " + std::to_string(position) + "\n";
            str += "Vendor ID: " + std::to_string(vendorID) + " Product Code: " + std::to_string(productCode) + "\n";
            
            str += "RxPDOs: \n";
            for(const auto& rxPDO : rxPDOs)
            {
                str += "PDO Mapping Address: " + std::to_string(rxPDO.pdoAddress) + "\n";
                for(const auto& entry : rxPDO.entries)
                {
                    str += "Entry name: " + entry.entryName +"\n";
                    str += "Index: " + std::to_string(entry.index) + " Subindex: " + std::to_string((uint16_t)entry.subindex) + " Bit length: " +  std::to_string((uint16_t)entry.bitlength) + "\n";
                } 
            }

            str += "TxPDOs: \n";
            for(const auto& txPDO : txPDOs)
            {
                str += "PDO Mapping Address: " + std::to_string(txPDO.pdoAddress) + "\n";
                for(const auto& entry : txPDO.entries)
                {
                    str += "Entry name: " + entry.entryName + "\n";
                    str += "Index: " + std::to_string(entry.index) + " Subindex: " + std::to_string((uint16_t)entry.subindex) + " Bit length: " + std::to_string((uint16_t)entry.bitlength) + "\n";
                } 
            }

            if(distributedClockConfig){
                str += "Distributed Clock configuration: \n";
                const auto dc = distributedClockConfig.value();
                str += "Assign Activate: " + std::to_string(dc.assignActivate) + "\n";
                str += "Sync0 Cycle: " + std::to_string(dc.sync0Activate) + " Sync0 Shift: " + std::to_string(dc.sync0Shift) + "\n";
                str += "Sync1 Cycle: " + std::to_string(dc.sync1Activate) + " Sync1 Shift: " + std::to_string(dc.sync1Shift) + "\n";   
            }

            return str;

        }

    };

  
}

#endif // COMMON_DEFS_HPP_
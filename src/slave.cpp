/**
 * @file slave.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ethercat_interface/slave.hpp"

namespace ethercat_interface
{

  namespace slave
  {

    Slave::Slave()
       : m_DcEnabled(false)
    {

    }

    Slave::Slave(const Slave& other)
    {

    }

    Slave Slave::operator=(const Slave& other)
    {

    }

    Slave::Slave(Slave&& other)
    {

    }

    Slave& Slave::operator=(Slave&& other)
    {

    }

    Slave::~Slave()
    {

    }
    
    ReturnType Slave::init(
      const SlaveInfo& info,
      ec_master_t* master_ptr,
      ec_domain_t* domain_ptr
    )
    {
      m_SlaveInformation = info;
      if(master_ptr && domain_ptr)
        return configureSlave(master_ptr, domain_ptr);
      return ReturnType::UninitializedMasterAndDomain;
    }
    ReturnType Slave::configureSlave(
      ec_master_t* master_ptr,
      ec_domain_t* domain_ptr)
    {
      // Get slave configuration pointer from the master:
      m_SlaveConfigPtr = ecrt_master_slave_config(
        master_ptr, m_SlaveInformation.alias, m_SlaveInformation.position, m_SlaveInformation.vendorID, m_SlaveInformation.productCode
      );

      if(m_SlaveConfigPtr == NULL)
      {
        return;
      }

      // Configure PDOs

      auto configuredPdos = configureProcessDataObjects();

      auto syncManagerConfig = configureSyncManager(configuredPdos.value());

      auto slaveConfigRes = ecrt_slave_config_pdos(
        m_SlaveConfigPtr,
        EC_END,
        syncManagerConfig.value()
      );
      if(slaveConfigRes != 0)
      {

      }
      if(m_SlaveInformation.distributedClockConfig.has_value())
      { const auto dcInfo = m_SlaveInformation.distributedClockConfig.value();
          ecrt_slave_config_dc(
          m_SlaveConfigPtr,
          dcInfo.assignActivate,
          dcInfo.sync0Activate,
          dcInfo.sync0Shift,
          dcInfo.sync1Activate,
          dcInfo.sync1Shift
        );
        m_DcEnabled = true;
      }

      for(auto rxPdoMapping : m_SlaveInformation.rxPDOs)
      {
        for(auto entry : rxPdoMapping.entries)
        {
          Entry_RW_Functions entryFunctions;
          switch (entry.type)
          {
          case ethercat_interface::DataType::INT8 :
            entryFunctions.readFunction = readInt8;
            entryFunctions.writeFunction = writeInt8;
            break;
          case ethercat_interface::DataType::UINT8 :
            entryFunctions.readFunction = readUInt8;
            entryFunctions.writeFunction = writeUInt8;
          break;
          case ethercat_interface::DataType::INT16 :
            entryFunctions.readFunction = readInt16;
            entryFunctions.writeFunction = writeInt16;
          break;
          case ethercat_interface::DataType::UINT16 :
            entryFunctions.readFunction = readUInt16;
            entryFunctions.writeFunction = writeUInt16;
          break;
          case ethercat_interface::DataType::INT32 :
            entryFunctions.readFunction = readInt32;
            entryFunctions.writeFunction = writeInt32;
          break;
          case ethercat_interface::DataType::UINT32 :
            entryFunctions.readFunction = readUInt32;
            entryFunctions.writeFunction = writeUInt32;
          break;
          case ethercat_interface::DataType::INT64 :
            entryFunctions.readFunction = readInt64;
            entryFunctions.writeFunction = writeInt64;
          break;
          case ethercat_interface::DataType::UINT64 :
            entryFunctions.readFunction = readUInt64;
            entryFunctions.writeFunction = writeUInt64;
          break;
          default:
            break;
          }

          m_EntryRWFunctionMap[entry.entryName] = entryFunctions;
        }

        for(auto txPdoMapping : m_SlaveInformation.txPDOs)
        {
        for(auto entry : txPdoMapping.entries)
        {
          Entry_RW_Functions entryFunctions;
          switch (entry.type)
          {
          case ethercat_interface::DataType::INT8 :
            entryFunctions.readFunction = readInt8;
            entryFunctions.writeFunction = writeInt8;
            break;
          case ethercat_interface::DataType::UINT8 :
            entryFunctions.readFunction = readUInt8;
            entryFunctions.writeFunction = writeUInt8;
          break;
          case ethercat_interface::DataType::INT16 :
            entryFunctions.readFunction = readInt16;
            entryFunctions.writeFunction = writeInt16;
          break;
          case ethercat_interface::DataType::UINT16 :
            entryFunctions.readFunction = readUInt16;
            entryFunctions.writeFunction = writeUInt16;
          break;
          case ethercat_interface::DataType::INT32 :
            entryFunctions.readFunction = readInt32;
            entryFunctions.writeFunction = writeInt32;
          break;
          case ethercat_interface::DataType::UINT32 :
            entryFunctions.readFunction = readUInt32;
            entryFunctions.writeFunction = writeUInt32;
          break;
          case ethercat_interface::DataType::INT64 :
            entryFunctions.readFunction = readInt64;
            entryFunctions.writeFunction = writeInt64;
          break;
          case ethercat_interface::DataType::UINT64 :
            entryFunctions.readFunction = readUInt64;
            entryFunctions.writeFunction = writeUInt64;
          break;
          default:
            break;
          }

          m_EntryRWFunctionMap[entry.entryName] = entryFunctions;
        }
      }

      return ReturnType::OK;
    }
    }

    std::optional<ec_pdo_info_t*> Slave::configureProcessDataObjects()
    {
      const std::size_t rxPdoSize = m_SlaveInformation.rxPDOs.size(); 
      const std::size_t txPdoSize = m_SlaveInformation.txPDOs.size();

      ec_pdo_info_t* pdoInfoPtr = nullptr;

      if(rxPdoSize || txPdoSize)
      {
        pdoInfoPtr = new ec_pdo_info_t[rxPdoSize + txPdoSize];
      }

      // Configure RxPDOs
      if(rxPdoSize)
      {
        for(std::size_t i = 0; i < rxPdoSize; i++)
        {   
            // Get the current PDO info
            auto& pdo = m_SlaveInformation.rxPDOs.at(i);
            const std::size_t numEntries = pdo.entries.size();
            // Create temp mapping
            std::pair<uint16_t, ec_pdo_entry_info_t*> mapping;
            mapping.first = pdo.pdoAddress;
            // Resize the mapping pointer
            mapping.second = new ec_pdo_entry_info_t[numEntries];
            for(std::size_t j = 0; j < numEntries; j++)
            {
                // Get the current PDO entry info
                const auto currEntry = pdo.entries.at(j);
                // Populate the value at index j
                mapping.second[j] = {
                    currEntry.index,
                    currEntry.subindex,
                    currEntry.bitlength
                };
                // Add the PDO entry to the Offset map
                m_Offsets.insert_or_assign(currEntry.entryName, uint());
            
            // Save the PDO mapping inside the ec_pdo_info_t pointer
            pdoInfoPtr[i].index = mapping.first;
            pdoInfoPtr[i].n_entries = (unsigned int)pdo.entries.size();
            pdoInfoPtr[i].entries = mapping.second;
        }
      }

      if(txPdoSize)
      { 
        std::size_t infoPtrIt = rxPdoSize;
        for(std::size_t i = 0; i < txPdoSize; i++)
        {
          // Get the current PDO info
          auto& pdo = m_SlaveInformation.txPDOs.at(i);
          const std::size_t numEntries = pdo.entries.size();
          // Create temp mapping
          std::pair<uint16_t, ec_pdo_entry_info_t*> mapping;
          mapping.first = pdo.pdoAddress;
          // Resize the mapping pointer
          mapping.second = new ec_pdo_entry_info_t[numEntries];
          for(std::size_t j = 0; j < numEntries; j++)
          {
              // Get the current PDO entry info
              const auto currEntry = pdo.entries.at(j);
              // Populate the value at index j
              mapping.second[j] = {
                  currEntry.index,
                  currEntry.subindex,
                  currEntry.bitlength
              };
              // Add the PDO entry to the Offset map
              m_Offsets.insert_or_assign(currEntry.entryName, uint());
          }
          // Save the PDO mapping inside the ec_pdo_info_t pointer
          pdoInfoPtr[infoPtrIt].index = mapping.first;
          pdoInfoPtr[infoPtrIt].n_entries = (unsigned int)pdo.entries.size();
          pdoInfoPtr[infoPtrIt].entries = mapping.second; 
          infoPtrIt += 1;
        }
      }
    }

      return pdoInfoPtr;
    }

    std::optional<ec_sync_info_t*> Slave::configureSyncManager(ec_pdo_info_t* pdos)
    { 
      const std::size_t syncManagerSize = m_SlaveInformation.syncManagerConfig.size();
      const auto syncManagerConfigInfo = m_SlaveInformation.syncManagerConfig;
      ec_sync_info_t* syncManagerConfig = new ec_sync_info_t[syncManagerSize + 1];
      syncManagerConfig[4] = {0xff};
      syncManagerConfig[0] = {
          0,
          syncManagerConfigInfo.at(0).syncManagerDirection,
          0,
          NULL,
          syncManagerConfigInfo.at(0).watchdogMode
      };
      
      syncManagerConfig[1] = {
          1,
          syncManagerConfigInfo.at(1).syncManagerDirection,
          0,
          NULL,
          syncManagerConfigInfo.at(1).watchdogMode
      };
      
      syncManagerConfig[2] = {
          2,
          syncManagerConfigInfo.at(2).syncManagerDirection,
          (unsigned int)m_SlaveInformation.rxPDOs.size(),
          pdos,
          syncManagerConfigInfo.at(2).watchdogMode
      };  
      //std::cout << "Rx 0: " << m_TxMappings.size() << std::end
      syncManagerConfig[3] = {
          3,
          syncManagerConfigInfo.at(3).syncManagerDirection,
          (unsigned int)m_SlaveInformation.txPDOs.size(),
          pdos + m_SlaveInformation.rxPDOs.size(),
          syncManagerConfigInfo.at(3).watchdogMode
        };

      return syncManagerConfig;
    }

  } // End of namespace slave

} // End of namespace ethercat_interface
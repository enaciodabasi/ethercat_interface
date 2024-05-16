/**
 * @file master.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//#include "ethercat_interface/domain.hpp"
#include "ethercat_interface/master.hpp"

namespace ethercat_interface
{
  namespace master
  {

    ReturnType Master::init(const std::string& config_file_path)
    {
      if(config_file_path.empty())
      {
        return ReturnType::EmptyConfigFile;
      }
      m_MasterPtr = ecrt_request_master(0);
      if(!m_MasterPtr)
      {
        return ReturnType::MasterRequestError;
      }

      const auto programConfigExp = parser::parseConfigFile(config_file_path);
      if(!programConfigExp.has_value())
      {
        return ReturnType::ConfigurationFileParseError;
      }

      const auto& programConfig = programConfigExp.value();

      for(const auto& domainConfig : programConfig.domainInfo)
      {
        m_DomainMap[domainConfig.first] = domain::Domain(domainConfig.second);
        m_DomainMap.at(domainConfig.first).domainPtr = ecrt_master_create_domain(m_MasterPtr);
      }

      // Register and initialize slaves
      for(const auto& slaveConfig : programConfig.slaveConfigurations)
      { 
        // Check if domain is already created
        if(auto isDomainInMap = m_DomainMap.find(slaveConfig.domainName); isDomainInMap != m_DomainMap.end())
        {
          // If the domain is already created, initialize the slave
          // and insert it to the m_SlaveSPtrMap and the domain it belongs to
          std::shared_ptr<slave::Slave> newSlave = std::make_shared<slave::Slave>(slave::Slave());
          m_SlaveSPtrMap[slaveConfig.slaveName] = newSlave;
          m_DomainMap.at(slaveConfig.domainName).addSlaveToDomain(slaveConfig.slaveName, m_SlaveSPtrMap.at(slaveConfig.slaveName));
        }
        else
        {
          // Domain is not yet created
          // Create domain
          domain::Domain newDomain;
          newDomain.frequency = 50.0; // Opt out to default 50 Hz.
          m_DomainMap[slaveConfig.domainName] = newDomain;
          std::shared_ptr<slave::Slave> newSlave = std::make_shared<slave::Slave>(slave::Slave());
          m_SlaveSPtrMap[slaveConfig.slaveName] = newSlave;
          m_DomainMap.at(slaveConfig.domainName).addSlaveToDomain(slaveConfig.slaveName, m_SlaveSPtrMap.at(slaveConfig.slaveName));
        }

        m_SlaveSPtrMap.at(slaveConfig.slaveName)->init(
          slaveConfig,
          m_MasterPtr,
          m_DomainMap.at(slaveConfig.domainName).domainPtr
        );
      }

      // Register domain entries:

      for(auto& [name, domain] : m_DomainMap)
      {
        int32_t domainEntrySize = 0;
        
        // Calculate domain entry size
        // entry size = sum of each entry of each pdo mapping of each slave
        for(const auto& [name, slave] : domain.slaveMap)
        {
          const auto slaveInfo = slave->getSlaveInformation();

          for(const auto& pdo : slaveInfo.rxPDOs)
          {
            domainEntrySize += pdo.entries.size();
          }

          for(const auto& pdo : slaveInfo.txPDOs)
          {
            domainEntrySize += pdo.entries.size();
          }
        }

        if(domainEntrySize != 0)
        {
          domain.domainEntries = new ec_pdo_entry_reg_t[domainEntrySize + 1];
        }
        else
        {

        }
        int numOfCurrentEntries = 0;
        for(const auto& [name, slave] : domain.slaveMap)
        {
          const auto& slaveInfo = slave->getSlaveInformation();
          for(const auto& rxPDO : slaveInfo.rxPDOs)
          {
            for(const auto& entry : rxPDO.entries)
            {
              uint* entryOffsetPtr = slave->getOffsetOfEntry(entry.entryName);
              if(entryOffsetPtr == nullptr)
              {

              }
              ec_pdo_entry_reg_t entryRegistry;
              entryRegistry.alias = slaveInfo.alias;
              entryRegistry.position = slaveInfo.position;
              entryRegistry.vendor_id = slaveInfo.vendorID;
              entryRegistry.product_code = slaveInfo.productCode;
              entryRegistry.index = entry.index;
              entryRegistry.subindex = entry.subindex;
              entryRegistry.offset = entryOffsetPtr;
              domain.domainEntries[numOfCurrentEntries] = entryRegistry;
              numOfCurrentEntries += 1;
            }
          }

          for(const auto& txPDO : slaveInfo.txPDOs)
          {
            for(const auto& entry : txPDO.entries)
            {
              uint* entryOffsetPtr = slave->getOffsetOfEntry(entry.entryName);
              if(entryOffsetPtr == nullptr)
              {

              }
              ec_pdo_entry_reg_t entryRegistry;
              entryRegistry.alias = slaveInfo.alias;
              entryRegistry.position = slaveInfo.position;
              entryRegistry.vendor_id = slaveInfo.vendorID;
              entryRegistry.product_code = slaveInfo.productCode;
              entryRegistry.index = entry.index;
              entryRegistry.subindex = entry.subindex;
              entryRegistry.offset = entryOffsetPtr;
              domain.domainEntries[numOfCurrentEntries] = entryRegistry;
              numOfCurrentEntries += 1;
            }
          }
        }
        // Set last entry to an empty struct
        domain.domainEntries[domainEntrySize + 1] = {};
      }
      
      // Register PDOs of slaves to domains
      for(auto& [name, domain] : m_DomainMap)
      {
        if(!domain.registerPDOs())
        {
          return ReturnType::ProcessDomainObjectRegisterError;
        }
      }

      if(ecrt_master_activate(m_MasterPtr) < 0)
      {
        return ReturnType::MasterActivationError; 
      }

      for(auto& [name, domain] : m_DomainMap)
      {
        if(!domain.createDomainData())
        {
          return ReturnType::DomainDataCreationError;
        }
      }

      for(auto& [name, domain] : m_DomainMap)
      {
        for(auto& [slaveName, slave] : domain.slaveMap)
        {
          slave->setDomainDataPointer(domain.domainDataPtr);
        }
      }

      return ReturnType::OK;

    }

        
    
  } // End of namespace master
  
} // End of namespace ethercat_interface

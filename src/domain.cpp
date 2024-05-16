/**
 * @file domain.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ethercat_interface/domain.hpp"

namespace ethercat_interface
{
  
  namespace domain
  {
    Domain::Domain()
    {
      domainPtr = nullptr;
      domainDataPtr = nullptr;
      domainEntries = nullptr;
    }

    Domain::Domain(double freq)
      : frequency(freq)
    {
      domainPtr = nullptr;
      domainDataPtr = nullptr;
      domainEntries = nullptr;
    }

    Domain::~Domain()
    {
    
    }

    bool Domain::registerPDOs()
    {   
        if(!domainEntries){
            return false;
        }

        if(!domainPtr){
            return false;
        }
        return ecrt_domain_reg_pdo_entry_list(domainPtr, domainEntries);
    }

    bool Domain::createDomainData()
    {
        domainDataPtr = ecrt_domain_data(domainPtr);
        if(!domainDataPtr){
            return false;
        }
        return true;
    }

    ReturnType Domain::addSlaveToDomain(const std::string& slave_name, std::shared_ptr<slave::Slave> slave_ptr)
    {
      if(auto isSlaveInMap = slaveMap.find(slave_name); isSlaveInMap != slaveMap.end())
      {
        slaveMap[slave_name] = slave_ptr;
        return ReturnType::OK;
      }      

      return ReturnType::SlaveAlreadyRegistered;
    }
  } // End of namespace domain
  

} // End of namespace ethercat_interface

/**
 * @file domain.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef DOMAIN_HPP_
#define DOMAIN_HPP_

#include <memory>
#include <vector>
#include <string>
#include <string_view>
#include <thread>

#include "ecrt.h"

#include "ethercat_interface/common_defs.hpp"
#include "ethercat_interface/slave.hpp"
#include "ethercat_interface/master.hpp"

namespace ethercat_interface
{
  namespace domain
  {
    struct Domain
    {
      public:
      friend class ethercat_interface::master::Master;
      private:
      ec_domain_t* domainPtr;
      
      uint8_t* domainDataPtr;
      
      double frequency;
      std::map<std::string, std::shared_ptr<slave::Slave>> slaveMap;
      ec_pdo_entry_reg_t* domainEntries;
      
      public:
      
      Domain();
      Domain(double frequency);
      ~Domain();
      bool registerPDOs();
      bool createDomainData();
      ReturnType addSlaveToDomain(const std::string& slave_name, std::shared_ptr<slave::Slave> slave_ptr);

    };
  
  } // End of namespace domain
}

#endif // DOMAIN_HPP_
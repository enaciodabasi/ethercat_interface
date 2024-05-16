/**
 * @file master.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <map>
#include <unordered_map>
#include <memory>
#include <string>

#include "ethercat_interface/slave.hpp"
#include "ethercat_interface/domain.hpp"
#include "ethercat_interface/common_defs.hpp"
#include "ethercat_interface/parser.hpp"

#include "ecrt.h"

namespace ethercat_interface
{

  namespace master
  {

    class Master
    {
      public:

      Master();

      ~Master();

      ReturnType init(const std::string& config_file_path);
      
      private:

      ec_master_t* m_MasterPtr;

      std::unordered_map<std::string, domain::Domain> m_DomainMap;

      std::map<std::string, std::shared_ptr<slave::Slave>> m_SlaveSPtrMap;


      

    };

  } // End of namespace master

} // End of namespace ethercat_interface
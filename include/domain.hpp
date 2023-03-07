/**
 * @file domain.hpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef DOMAIN_HPP
#define DOMAIN_HPP

#include <iostream>
#include <string>

#include "ecrt.h"

namespace ethercat_interface
{   
    // Forward declaration of the class Master. In order to use 
    namespace master
    {
        class Master;
    }

    namespace domain
    {
        class Domain
        {
            
            friend class master::Master;

        public:
        
            Domain(
                const std::string& domain_name
            );
        
            ~Domain();

            void updateDomainState();

        protected:

        /**
         * @brief Request Domain Process Data from EtherCAT, should be called after activating the EtherCAT master.
         * 
         * @return true: if Domain PD pointer is not a nullptr.
         * @return false: if Domain PD Pointer is a nullptr.
         */
        bool receiveDomainData();
        
        private:

            std::string m_DomainName;

            ec_domain_t* m_EthercatDomain;

            ec_domain_state_t m_EthercatDomainState;

            uint8_t* m_DomainProcessData = nullptr;

            bool createDomain(ec_master_t* master_ptr);

            bool ENABLE_LOGGING = false;

        };
        
        
        
    }
}

#endif // DOMAIN_HPP
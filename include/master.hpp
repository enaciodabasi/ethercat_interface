/**
 * @file master.hpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef MASTER_HPP
#define MASTER_HPP

#include <iostream>
#include <unordered_map>

#include "domain.hpp"

#include "ecrt.h"

namespace ethercat_interface
{
    namespace master
    {
        class Master
        {
            public:

            Master(
                unsigned int master_index
            );

            virtual ~Master();

            void updateMasterState();

            bool activateMaster();

            inline void registerDomain(domain::Domain* domain)
            {
                m_RegisteredDomains[domain->getDomainName()] = domain;
            }

            protected:

            ec_master_t* m_EthercatMaster;

            // States

            ec_master_state_t m_EthercatMasterState;

            private:

            unsigned int m_MasterIndex;

            bool ENABLE_LOGGING = false;

            std::unordered_map<std::string, domain::Domain*> m_RegisteredDomains;

        };
    }
}

#endif // MASTER_HPP
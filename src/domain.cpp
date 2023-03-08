/**
 * @file domain.cpp
 * @author Eren Naci Odabaşı (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "domain.hpp"

namespace ethercat_interface
{
    namespace domain
    {
        Domain::Domain(
            const std::string& domain_name
        )   : m_DomainName(domain_name)
        {
            m_EthercatDomainState = {};

            configureSlaves(); // Configure each slave

            if(m_NumOfPdoEntryRegistries != 0)
            {
                // + 1 is for adding the empty entry registry at the end of the array.
                m_DomainPdoEntryRegistries = new ec_pdo_entry_reg_t[m_NumOfPdoEntryRegistries + 1]; 
            }


        }

        Domain::~Domain()
        {

        }

        void Domain::updateDomainState()
        {
            ec_domain_state_t state;
            ecrt_domain_state(m_EthercatDomain, &state);

            if(ENABLE_LOGGING)
            {
                if(state.working_counter != m_EthercatDomainState.working_counter)
                {
                    std::cout << "Domain WC:" << state.working_counter << std::endl; 
                }

                if(state.wc_state != m_EthercatDomainState.wc_state)
                {
                    std::cout << "Domain State: " << state.wc_state << std::endl;
                }
            }

            m_EthercatDomainState = state;
        }

        bool Domain::receiveDomainData()
        {
            if(!(m_DomainProcessData = ecrt_domain_data(m_EthercatDomain)))
            {   
                // Log
                return false;
            }

            return true;
        }

        bool Domain::createDomain(ec_master_t* master_ptr)
        {
            m_EthercatDomain = ecrt_master_create_domain(master_ptr);

            // If ethercat domain pointer is empty return false.
            if(!m_EthercatDomain)
            {
                // TODO: Logger.
                return false;
            }

            // Logger.

            return true;
        }

        void Domain::configureSlaves()
        {
            
            std::size_t tempPdoNum = 0;

            for(const auto& s : m_RegisteredSlaves)
            {
                s.second->configure_slave();
                tempPdoNum += s.second->getSlaveInfo().pdoEntryInfo.indexes.size();
            }

            m_NumOfPdoEntryRegistries = tempPdoNum;
        }

        void Domain::setupSlaves(ec_master_t* master_ptr, ec_slave_config_t* slave_config_ptr)
        {
            for(const auto& s : m_RegisteredSlaves)
            {
                s.second->setupSlave(master_ptr, m_EthercatDomain, slave_config_ptr);
            }
        }

        ec_pdo_entry_reg_t* Domain::createDomainPdoEntryRegistries()
        {   
            std::size_t i = 0;
            for(const auto& s : m_RegisteredSlaves)
            {
                auto info = s.second->getSlaveInfo();
                
                for(std::size_t j = 0; j < info.pdoEntryInfo.indexes.size(); j++)
                {
                    m_DomainPdoEntryRegistries[i] = {
                        (uint16_t)info.alias,
                        (uint16_t)info.position,
                        (uint32_t)info.vendorID,
                        (uint32_t)info.productCode,
                        info.pdoEntryInfo.indexes[j],
                        info.pdoEntryInfo.subindexes[j],
                        s.second->getOffset()->getData(s.second->getOffset()->m_OffsetNameIndexes[j])
                    };

                    i += 1;
                }
            }

            m_DomainPdoEntryRegistries[m_NumOfPdoEntryRegistries] = {};
        }
    }
}
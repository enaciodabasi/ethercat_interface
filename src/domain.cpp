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
            for(const auto& s : m_RegisteredSlaves)
            {
                s.second->configure_slave();
            }
        }

        void Domain::setupSlaves(ec_master_t* master_ptr, ec_slave_config_t* slave_config_ptr)
        {
            for(const auto& s : m_RegisteredSlaves)
            {
                s.second->setupSlave(master_ptr, m_EthercatDomain, slave_config_ptr);
            }
        }
    }
}
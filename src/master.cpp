/**
 * @file master.cpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "master.hpp"

namespace ethercat_interface
{
    namespace master
    {

        Master::Master(unsigned int master_index)
            : m_MasterIndex(master_index)
        {
            m_EthercatMaster = ecrt_request_master(master_index);
            if(!m_EthercatMaster)
            {
                // Log.
                exit(0);
            }
        }

        Master::~Master()
        {
            ecrt_release_master(m_EthercatMaster);
        }


        bool Master::activateMaster()
        {
            if(ecrt_master_activate(m_EthercatMaster))
            {
                // Log.
                return false;
            }

            for(const auto& d : m_RegisteredDomains)
            {
                d.second->receiveDomainData();
            }

            return true;
        }

        ec_domain_t* Master::getDomainPtr(const std::string& domain_name)
        {
            if(m_RegisteredDomains.find(domain_name) == m_RegisteredDomains.end())
            {
                return nullptr;
            }

            return m_RegisteredDomains.at(domain_name)->getDomainPtr();
        }

        void Master::updateMasterState()
        {
            ec_master_state_t state;
            ecrt_master_state(m_EthercatMaster, &state);

            if(ENABLE_LOGGING)
            {
                if(state.slaves_responding != m_EthercatMasterState.slaves_responding)
                {
                    std::cout << state.slaves_responding << " slaves are responding." << std::endl;
                }

                if(state.link_up != m_EthercatMasterState.link_up)
                {
                    std::cout << "Link is " << (state.link_up ? "up" : "down") << std::endl;
                }

                if(state.al_states != m_EthercatMasterState.al_states)
                {
                    std::cout << "AL States: " << state.al_states << std::endl;
                } 
            }

            m_EthercatMasterState = state;
        }

        void Master::updateSlaveStates()
        {
        
            ec_slave_config_state_t state;
            ecrt_slave_config_state(
                this->m_SlaveConfig,
                &state
            );
            
            if(ENABLE_LOGGING)
            {
                if(state.al_state != m_SlaveStates.al_state)
                {
                    std::cout << "Slave: State " << state.al_state << std::endl;
                }
                if(state.online != m_SlaveStates.online)
                {
                    std::cout << "Slave: " << (state.online ? "online" : "offline") << std::endl;
                }
                if(state.operational != m_SlaveStates.operational)
                {
                    std::cout << "Slave is " << (state.operational ? "" : "not") << "operational" << std::endl;
                }
            }

            m_SlaveStates = state;
            
        }

        void Master::configureDomains()
        {
            for(const auto& d : m_RegisteredDomains)
            {
                d.second->createDomain(m_EthercatMaster);
                d.second->configureSlaves();
            }
        }

        void Master::setupDomains()
        {
            for(const auto& d : m_RegisteredDomains)
            {
                d.second->setupSlaves(m_EthercatMaster, m_SlaveConfig);
                if(ecrt_domain_reg_pdo_entry_list(d.second->m_EthercatDomain, d.second->m_DomainPdoEntryRegistries))
                {
                    std::cout << "Failed during PDO entry registries check." << std::endl;
                }
            }
        }
    }
}
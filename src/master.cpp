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

    using namespace logger;
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
            m_SlaveStates = {};
        }

        Master::Master(unsigned int master_index, std::shared_ptr<Logger> logger)
            : m_MasterIndex(master_index)
        {
            
            m_Logger = logger;

            m_EthercatMaster = ecrt_request_master(m_MasterIndex);
            if(!m_EthercatMaster)
            {
                m_Logger->log(FATAL, std::string("Master " + m_MasterIndex), "Can't request master.");
            }
            else
            {
                m_Logger->log(INFO, std::string("Master " + m_MasterIndex), "Successfully requested master.");
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
                m_Logger->log(FATAL, std::string("Master " + m_MasterIndex), "Can't activate master.");
                return false;
            }

            for(const auto& d : m_RegisteredDomains)
            {
                d.second->receiveDomainData();
            }

            return true;
        }

        void Master::receive(const std::string& domain_name)
        {
            if(m_RegisteredDomains.find(domain_name) == m_RegisteredDomains.end())
            {
                m_Logger->log(ERROR, std::string("Master " + m_MasterIndex), "Can't find a registered domain with the name " + domain_name);
                return;
            }

            ecrt_master_receive(m_EthercatMaster);
            ecrt_domain_process(m_RegisteredDomains.at(domain_name)->getDomainPtr());
        }

        void Master::send(const std::string& domain_name)
        {
            if(m_RegisteredDomains.find(domain_name) == m_RegisteredDomains.end())
            {
                m_Logger->log(ERROR, std::string("Master " + m_MasterIndex), "Can't find a registered domain with the name " + domain_name);
                return;
            }

            ecrt_domain_queue(m_RegisteredDomains.at(domain_name)->getDomainPtr());
            ecrt_master_send(m_EthercatMaster);
        }

        void Master::setMasterTime(const uint64_t &app_time)
        {
            ecrt_master_application_time(m_EthercatMaster, app_time);
        }

        void Master::syncMasterClock(const uint64_t &sync_time)
        {
            ecrt_master_sync_reference_clock_to(m_EthercatMaster, sync_time);
            ecrt_master_sync_slave_clocks(m_EthercatMaster);
        }

        bool Master::enableSlaves()
        {
            bool areAllEnabled = false;

            for(const auto& d : m_RegisteredDomains)
            {
                bool isEnabled = d.second->enableSlaves();

                if(isEnabled)
                {
                    areAllEnabled = true;
                }
                else
                {
                    areAllEnabled = false;
                }
            }

            return areAllEnabled;
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
                    m_Logger->log(INFO, std::string("Master " + m_MasterIndex), state.slaves_responding + " slaves are responding");
                }

                if(state.link_up != m_EthercatMasterState.link_up)
                {
                    m_Logger->log(INFO, std::string("Master " + m_MasterIndex), "Link is" + std::string((state.link_up ? "up" : "down")));
                }

                if(state.al_states != m_EthercatMasterState.al_states)
                {
                    m_Logger->log(INFO, std::string("Master " + m_MasterIndex), "AL States: " + state.al_states);
                } 
            }

            m_EthercatMasterState = state;
        }

        void Master::updateDomainStates()
        {
            for(const auto& d : m_RegisteredDomains)
            {
                d.second->updateDomainState();
            }
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
                    m_Logger->log(INFO, std::string("Master " + m_MasterIndex), "Slave State: + state.al_state");
                }
                if(state.online != m_SlaveStates.online)
                {
                    m_Logger->log(INFO, std::string("Master " + m_MasterIndex), "Slave: " + std::string((state.online ? "online" : "offline")));
                }
                if(state.operational != m_SlaveStates.operational)
                {
                    m_Logger->log(INFO, std::string("Master " + m_MasterIndex), "Slave is " + std::string((state.operational ? " " : "not ")) + "operational.");
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
                d.second->setupSlaves(m_EthercatMaster, &m_SlaveConfig);
                if(ecrt_domain_reg_pdo_entry_list(d.second->m_EthercatDomain, d.second->m_DomainPdoEntryRegistries) != 0)
                {
                    m_Logger->log(FATAL, std::string("Master " + m_MasterIndex), "Failed during PDO entry registries check.");
                }
            }
        }
    }
}
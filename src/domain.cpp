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

#include "ethercat_interface/domain.hpp"

namespace ethercat_interface
{
    using namespace logger;

    namespace domain
    {
        Domain::Domain(
            const std::string& domain_name
        )   : m_DomainName(domain_name)
        {

            m_EthercatDomainState = {};
            std::cout << m_DomainName << std::endl;

            //configureSlaves(); // Configure each slave
            
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
                    m_Logger->log(INFO, m_DomainName, "Domain WC:" + std::to_string(state.working_counter)); 
                }

                if(state.wc_state != m_EthercatDomainState.wc_state)
                {
                    m_Logger->log(INFO, m_DomainName, "Domain State: " + std::to_string(state.wc_state));
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

            for(const auto& s : m_RegisteredSlaves)
            {
                s.second->setDomainProcessDataPtr(m_DomainProcessData);
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
            //m_Logger->log(INFO, m_DomainName, "Configuring slaves of the domain.");
            for(const auto& s : m_RegisteredSlaves)
            { 
                  
                s.second->setLogger(m_Logger);
                if(s.second->getSlaveType() == SlaveType::Coupler)
                {   
                    //m_Logger->log(INFO, m_DomainName, s.second->getSlaveName() + " Slave's type: Coupler, skipping.");
                    continue;
                }
                //auto inf = s.second->getSlaveInfo();
                //inf.toString();
                s.second->configure_slave();
                tempPdoNum += s.second->getSlaveInfo().pdoEntryInfo.indexes.size();

                
            }
            
            m_NumOfPdoEntryRegistries = tempPdoNum;

            //std::cout << "Configured slaves for domain " << m_DomainName << std::endl;

            
        }

        void Domain::setupSlaves(ec_master_t* master_ptr, ec_slave_config_t** slave_config_ptr)
        {   
/*             m_Logger->log(INFO, m_DomainName, "Setting up slaves of the domain.");
 */         for(const auto& s : m_RegisteredSlaves)
            {
                s.second->setupSlave(master_ptr, m_EthercatDomain, slave_config_ptr);
            }

            if(m_NumOfPdoEntryRegistries != 0)
            {   
                    std::cout << "Number of PDO registries: " << m_NumOfPdoEntryRegistries << std::endl;
/*                 m_Logger->log(INFO, m_DomainName, "Registering PDO entries of the domain.");
 */                // + 1 is for adding the empty entry registry at the end of the array.
                m_DomainPdoEntryRegistries = new ec_pdo_entry_reg_t[m_NumOfPdoEntryRegistries + 1];
                this->createDomainPdoEntryRegistries(); 
            }
            else
            {  
/*                 m_Logger->log(FATAL, m_DomainName, "Can't register PDO entries.");
 */                
            }
        }

        bool Domain::enableSlaves()
        {
            bool areAllEnabled = false;

            for(const auto& s : m_RegisteredSlaves)
            {
                if(s.second->getSlaveInfo().slaveType == SlaveType::Coupler)
                {
                    continue;
                }
                bool isEnabled = s.second->enableOperation();
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

        bool Domain::shutdownSlaves()
        {
            bool areAllShutdown = false;

            for(const auto& s : m_RegisteredSlaves)
            {
                bool isShutdown = s.second->shutdown();
                isShutdown ? areAllShutdown = true : areAllShutdown = false;
            }
        }

        void Domain::createDomainPdoEntryRegistries()
        {   
            ec_pdo_entry_reg_t* temp = new ec_pdo_entry_reg_t[m_NumOfPdoEntryRegistries + 1];
            int i = 0;
            for(const auto& s : m_RegisteredSlaves)
            {
                auto info = s.second->getSlaveInfo();
                if(info.slaveType == SlaveType::Coupler)
                {
                    continue;
                }


                for(std::size_t j = 0; j < info.pdoEntryInfo.indexes.size(); j++)
                {  
                    const std::string currDataName = s.second->getOffset()->getDataName(j);
                    /* std::cout << currDataName << " " <<
                        info.pdoEntryInfo.indexes[j] << " " <<  (uint16_t)info.pdoEntryInfo.subindexes[j] << std::endl;
                 */
                    
                    if(s.second->getOffset()->getDataOffset(currDataName) != std::nullopt)
                        *(temp + i) = {
                            (uint16_t)info.alias,
                            (uint16_t)info.position,
                            (uint32_t)info.vendorID,
                            (uint32_t)info.productCode,
                            info.pdoEntryInfo.indexes[j],
                            info.pdoEntryInfo.subindexes[j],
                            s.second->getOffset()->getDataOffset(currDataName).value()
                        };
                    
                    i+= 1;
                }
                
            }
            *(temp + m_NumOfPdoEntryRegistries -1) = {};
            
            m_DomainPdoEntryRegistries = temp;
        }
    }
}
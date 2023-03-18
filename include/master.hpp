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
#include "logger.hpp"

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

            Master(
                unsigned int master_index,
                std::shared_ptr<logger::Logger> logger
            );

            virtual ~Master();

            void updateMasterState();

            void updateSlaveStates();

            void updateDomainStates();

            bool activateMaster();

            inline void registerDomain(domain::Domain* domain)
            {
                m_RegisteredDomains[domain->getDomainName()] = domain;
            }

            inline ec_master_t* getEthercatMasterPtr()
            {
                return m_EthercatMaster;
            }

            template<typename T>
            void write(
                const std::string& domain_name,
                const std::string& slave_name,
                const std::string& value_to_write_name,
                const T& value_to_write,
                int bit_position = NULL
            );

            template<typename T>
            auto read(
                const std::string& domain_name,
                const std::string& slave_name,
                const std::string& value_to_read_name,
                int bit_position = NULL
            );

            void receive(const std::string& domain_name);

            void send(const std::string& domain_name);

            void setMasterTime(const uint64_t& app_time);

            void syncMasterClock(const uint64_t& sync_time);

            ec_domain_t* getDomainPtr(const std::string& domain_name);

            protected:

            ec_master_t* m_EthercatMaster;

            // States

            ec_master_state_t m_EthercatMasterState;

            // Slave config pointer.

            ec_slave_config* m_SlaveConfig;

            ec_slave_config_state_t m_SlaveStates;

            unsigned int m_MasterIndex;

            std::shared_ptr<logger::Logger> m_Logger;

            bool ENABLE_LOGGING = true;

            std::unordered_map<std::string, domain::Domain*> m_RegisteredDomains;

            public:
            // Calls the configureSlaves function for each registered domain.
            void configureDomains();

            // Calls the setupSlaves function each registered domain.
            void setupDomains();

        };

        template<typename T>
        void Master::write(
            const std::string& domain_name,
            const std::string& slave_name,
            const std::string& value_to_write_name,
            const T& value_to_write,
            int bit_position
        )
        {
            if(m_RegisteredDomains.find(domain_name) == m_RegisteredDomains.end())
            {
                // TODO: Throw exception
                return;
            }

            if(bit_position == NULL)
            {
                m_RegisteredDomains.at(domain_name)->write<T>(
                    slave_name,
                    value_to_write_name,
                    value_to_write
                );
            }
            else
            {
                m_RegisteredDomains.at(domain_name)->write<T>(
                    slave_name,
                    value_to_write_name,
                    value_to_write,
                    bit_position
                );
            }

        }

        template<typename T>
        auto Master::read(
            const std::string& domain_name,
            const std::string& slave_name,
            const std::string& value_to_read_name,
            int bit_position
        )
        {
            if(m_RegisteredDomains.find(domain_name) == m_RegisteredDomains.end())
            {
                // TODO: Throw exception
                return;
            }

            if(bit_position == NULL)
            {
                return m_RegisteredDomains.at(domain_name)->read<T>(
                    slave_name,
                    value_to_read_name
                );
            }

            return m_RegisteredDomains.at(domain_name)->read<T>(
                slave_name,
                value_to_read_name,
                bit_position
            );
        }
    }

    
}

#endif // MASTER_HPP
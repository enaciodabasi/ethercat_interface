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

extern "C"
{
    #include "dc_sync.h"
}

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

            ~Master();

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
                std::optional<int> bit_position = std::nullopt
            );

            template<typename T>
            std::optional<T> read(
                const std::string& domain_name,
                const std::string& slave_name,
                const std::string& value_to_read_name,
                std::optional<int> bit_position = std::nullopt
            );

            /**
             * @brief 
             * 
             * @tparam T 
             * @param slave_position 
             * @param sdo_info 
             * @param value_to_write 
             * @return true 
             * @return false 
             */
            template<typename T>
            bool sdo_write(
                const uint16_t& slave_position,
                const SDO_Info& sdo_info,
                const T value_to_write
            );

            template<typename T>
            std::optional<T> sdo_read(
                const uint16_t& slave_position,
                const SDO_Info& sdo_info
            );

            void receive(const std::string& domain_name);

            void send(const std::string& domain_name);

            void setMasterTime(const uint64_t& app_time);

            void syncMasterClock(const uint64_t& sync_time, DistributedClockHelper& dc_helper);

            bool enableSlaves();

            bool shutdownSlaves();

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

            bool addSlaveToDomain(const SlaveInfo& slave_config);

        };

        template<typename T>
        void Master::write(
            const std::string& domain_name,
            const std::string& slave_name,
            const std::string& value_to_write_name,
            const T& value_to_write,
            std::optional<int> bit_position
        )
        {
            if(m_RegisteredDomains.find(domain_name) == m_RegisteredDomains.end())
            {
                // TODO: Throw exception
                return;
            }

            m_RegisteredDomains.at(domain_name)->write<T>(
                slave_name,
                value_to_write_name,
                value_to_write,
                bit_position
            );


        }

        template<typename T>
        std::optional<T> Master::read(
            const std::string& domain_name,
            const std::string& slave_name,
            const std::string& value_to_read_name,
            std::optional<int> bit_position
        )
        {
            if(m_RegisteredDomains.find(domain_name) == m_RegisteredDomains.end())
            {
                // TODO: Throw exception
                
            }

            return m_RegisteredDomains.at(domain_name)->read<T>(
                slave_name,
                value_to_read_name,
                bit_position
            );
        }

        template<typename T>
        bool Master::sdo_write(
            const uint16_t& slave_position,
            const SDO_Info& sdo_info,
            const T value_to_write
        )
        {
            uint32_t abort_code;

            uint8_t* data_address = (uint8_t*)&value_to_write;
            size_t data_size = sizeof(T) / sizeof(uint8_t);

            int res = ecrt_master_sdo_download(
                this->m_EthercatMaster,
                slave_position,
                sdo_info.first, // SDO Index
                sdo_info.second,  // SDO Subindex
                data_address,
                data_size,
                &abort_code
            );
            
            return (res < 0) ? false : true;
        }

        template<typename T>
        std::optional<T> Master::sdo_read(
            const uint16_t& slave_position,
            const SDO_Info& sdo_info
        )
        {
            uint32_t abort_code;

            T target_data;

            size_t target_data_size = sizeof(T);
            size_t result_size;

            int res = ecrt_master_sdo_upload(
                this->m_EthercatMaster,
                slave_position,
                sdo_info.first,
                sdo_info.second,
                (uint8_t*)(&target_data),
                target_data_size,
                &result_size,
                &abort_code
            );

            if(res < 0 || sizeof(result_size) != target_data_size)
            {
                return std::nullopt;
            }

            
            return (T)target_data;

        }

    }   
}

#endif // MASTER_HPP
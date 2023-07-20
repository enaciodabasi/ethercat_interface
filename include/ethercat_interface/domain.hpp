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
#include <unordered_map>
#include <string>
#include <map>

#include "slave.hpp"
#include "logger.hpp"

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

            inline void setLogger(std::shared_ptr<logger::Logger> shared_logger)
            {
                m_Logger = shared_logger;
                ENABLE_LOGGING = true;
            }

            bool enableSlaves();

            bool shutdownSlaves();

            inline const std::optional<uint16_t> getControlWord(const std::string& slave_name) const
            {
                if(m_RegisteredSlaves.find(slave_name) == m_RegisteredSlaves.end())
                    return std::nullopt;
                    
                return m_RegisteredSlaves.at(slave_name)->getControlWord();
            }

            /**
             * @brief 
             * 
             */
            void updateDomainState();

            /**
             * @brief 
             * 
             * @tparam T 
             * @param slave_name 
             * @param value_to_write_name 
             * @param value_to_write 
             * @param bit_position 
             */
            template<typename T>
            void write(
                const std::string& slave_name,
                const std::string& value_to_write_name,
                const T& value_to_write,
                std::optional<int> bit_position = std::nullopt
            );

            template<typename T>
            std::optional<T> read(
                const std::string& slave_name,
                const std::string& value_to_read_name,
                std::optional<int> bit_position = std::nullopt
            );

            template<typename T, std::size_t SIZE>
            std::optional<std::vector<T>> readArray(
                const std::string& slave_name,
                const std::string& pdo_name
            );

            template <typename T>
            void writeArray(
                const std::string& slave_name,
                const std::string& pdo_name,
                const std::vector<T>& value_to_write
            );

            /**
             * @brief: Stores the slave pointers inside the m_RegisteredSlaves Map.
             * 
             * @param slave: Pointer to the slave to register. 
             */
            inline void registerSlave(slave::Slave* slave)
            {
                m_RegisteredSlaves[slave->getSlaveName()] = slave;
                std::cout << slave->getSlaveName() << std::endl;
            }

            inline void registerSlaves(std::vector<slave::Slave*> slaves)
            {
                for(std::size_t i = 0; i < slaves.size(); i++)
                {
                    m_RegisteredSlaves[slaves[i]->getSlaveName()] = slaves[i];

                }
            }

            inline ec_domain_t* getDomainPtr()
            {
                return m_EthercatDomain;
            }

            inline const std::string getDomainName() const
            {
                return m_DomainName;
            }

        protected:

        /**
         * @brief Request Domain Process Data from EtherCAT, should be called after activating the EtherCAT master.
         * 
         * @return true: if Domain PD pointer is not a nullptr.
         * @return false: if Domain PD Pointer is a nullptr.
         */
        bool receiveDomainData();
        
        protected:
            
            std::string m_DomainName;

            ec_domain_t* m_EthercatDomain;

            ec_domain_state_t m_EthercatDomainState;

            ec_pdo_entry_reg_t* m_DomainPdoEntryRegistries;

            std::size_t m_NumOfPdoEntryRegistries = 0;

            uint8_t* m_DomainProcessData = nullptr;

            /**
             * @brief Holds the pointers to the slaves that are registered in this domain.
             * 
             */
            std::map<std::string, slave::Slave*> m_RegisteredSlaves;

            bool createDomain(ec_master_t* master_ptr);

            void configureSlaves();

            void setupSlaves(ec_master_t* master_ptr, ec_slave_config_t** slave_config_ptr);

            bool ENABLE_LOGGING = false;

            std::shared_ptr<logger::Logger> m_Logger;

            void createDomainPdoEntryRegistries();

        };

        template<typename T>
        void Domain::write(
            const std::string& slave_name,
                const std::string& value_to_write_name,
                const T& value_to_write,
                std::optional<int> bit_position
        )
        {
            // TODO: check if the slave with the name slave_name exists and throw exception.

            
            if(m_RegisteredSlaves.find(slave_name) == m_RegisteredSlaves.end())
            {
                // throw exception
                return;
            }
/* 
            if(bit_position == NULL)
            {
                m_RegisteredSlaves.at(slave_name)->writeToSlave<T>(
                    value_to_write_name,
                    value_to_write
                );
            }
            else */
                m_RegisteredSlaves.at(slave_name)->writeToSlave<T>(
                    value_to_write_name,
                    value_to_write,
                    bit_position
                );

        }

        template<typename T>
        std::optional<T> Domain::read(
            const std::string& slave_name,
            const std::string& value_to_read_name,
            std::optional<int> bit_position

        )
        {
            /* if(m_RegisteredSlaves.find(slave_name) == m_RegisteredSlaves.end())
            {

               return -1;
            } */
/*             if(bit_position == NULL)
            {
                return m_RegisteredSlaves.at(slave_name)->readFromSlave<T>(value_to_read_name);
            } */
            
            return m_RegisteredSlaves.at(slave_name)->readFromSlave<T>(
                value_to_read_name,
                bit_position
            );
        }

        template<typename T, std::size_t SIZE>
        std::optional<std::vector<T>> Domain::readArray(
            const std::string& slave_name,
            const std::string& pdo_name
        )
        {
            if(m_RegisteredSlaves.find(slave_name) == m_RegisteredSlaves.end())
            {
                return std::nullopt;
            }

            return m_RegisteredSlaves.at(slave_name)->readArray<T, SIZE>(
                pdo_name
            );
        }

        template <typename T>
        void Domain::writeArray(
            const std::string& slave_name,
            const std::string& pdo_name,
            const std::vector<T>& value_to_write
        )
        {
            if(m_RegisteredSlaves.find(slave_name) == m_RegisteredSlaves.end())
            {
                return;
            }

            m_RegisteredSlaves.at(slave_name)->writeArray<T>(
                pdo_name, 
                value_to_write
            );
        }   
        
        
    }
}

#endif // DOMAIN_HPP
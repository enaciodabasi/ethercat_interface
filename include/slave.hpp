/**
 * @file slave.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef SLAVE_HPP
#define SLAVE_HPP

#include <iostream>
#include <vector>
#include <memory>
#include <map>

#include "ecrt.h"

#include "offset.hpp"
#include "utilities.hpp"


namespace ethercat_interface
{   

    using namespace utilities;
    namespace slave
    {   
        class Slave;

        ec_pdo_entry_info_t createSlavePdoEntryStruct(
            uint16_t index,
            uint8_t subindex,
            uint8_t bit_length
        );

        ec_pdo_entry_info_t* createSlavePdoEntries(
            std::vector<uint16_t> indexes,
            std::vector<uint8_t> subindexes,
            std::vector<uint16_t> bit_lengths
        );
        
        ec_pdo_info_t* createSlavePDOs(
            ec_pdo_entry_info_t* entriesArray,
            int RxPDO_start,
            int RxPDO_size,
            int TxPDO_start,
            int TxPDO_size
        );

        /**
         * @brief Creates ec_pdo_info_t array, registers PDO entries one by one
         * 
         * @param num_entries: Number of entries in the PDO (RxPDO + TxPDO) 
         * @param entriesArray: Pointer or array to a ec_pdo_entry_info_t struct
         * @param RxPDO_start: Start address of the RxPDO entries
         * @param TxPDO_start: Start address of the TxPDO entries
         * @param RxPDO_size: 
         * @param TxPDO_size:
         * @return Array of ec_pdo_info_t's. 
         */
        ec_pdo_info_t* createSlavePDOs(
            std::size_t num_entries,
            ec_pdo_entry_info_t* entriesArray,
            uint16_t RxPDO_start,
            uint16_t TxPDO_start,
            int RxPDO_size,
            int TxPDO_size
        );

        /**
         * @brief Creates ec_pdo_info_t array, registers PDO entries one by one
         * 
         * @param entries_vector: STL vector containing ec_pdo_entry_info_t structs. 
         * @param RxPDO_start: Start address of the RxPDO entries
         * @param TxPDO_start: Start address of the TxPDO entries 
         * @return Array of ec_pdo_info_t's. 
         */
        ec_pdo_info_t* createSlavePDOs(
            std::vector<ec_pdo_entry_info_t>& entries_vector,
            uint16_t RxPDO_start,
            uint16_t TxPDO_start
        );
        /**
         * @brief Create a ec_pdo_entry_reg_t struct pointer for mass PDO entry registration.
         * 
         * @param slave_alias 
         * @param slave_position 
         * @param slave_vendor_id 
         * @param slave_product_code 
         * @param index 
         * @param subindex 
         * @param offset 
         * @param bit_position 
         * @return ec_pdo_entry_reg_t* 
         */
    
        ec_pdo_entry_reg_t* createDomainRegistries(
            uint16_t slave_alias,
            uint16_t slave_position,
            uint32_t slave_vendor_id,
            uint32_t slave_product_code,
            std::vector<uint16_t> indexes,
            std::vector<uint8_t> subindexes,
            Offset* offset,
            unsigned int* bit_position = nullptr
        );

        ec_sync_info_t* createSlaveSyncs(
            uint8_t num_sync_managers,
            std::vector<ec_direction_t> sync_directions,
            std::vector<uint> number_of_pdos,
            std::vector<std::optional<int>> index_to_add_to_pdo,
            ec_pdo_info_t* pdos,
            std::vector<ec_watchdog_mode_t> watchdog_modes
        );

        /**
         * @brief: Base class for implementing an EtherCAT Slave using the IGH EtherCAT Master Library. 
         * 
         */
        class Slave
        {
            public:


            Slave(const std::string& slave_name, SlaveInfo slave_info, Offset* offset = nullptr, bool enable_logging = false);
            ~Slave();

            virtual void configure_slave();

            /**
             * @brief 
             * 
             * @param masterPtr 
             */
            virtual void setupSlave(
                ec_master_t* masterPtr,
                ec_domain_t* domainPtr
            );

            inline const std::string getSlaveName() const
            {
                return m_SlaveName;
            }

            inline ec_slave_config_t* getEthercatSlaveConfig()
            {
                return m_SlaveConfig;
            }

            inline void setOffsetPtr(Offset* offset)
            {
                m_SlaveOffsets = offset;
            }

            void updateSlaveState();
    
            protected:

            std::string m_SlaveName;

            ec_slave_config_t* m_SlaveConfig;

            ec_slave_config_state_t m_CurrentSlaveState;

            ec_pdo_entry_reg_t* m_SlavePdoEntryRegistries;

            ec_pdo_entry_info_t* m_SlavePdoEntries;

            ec_pdo_info_t* m_SlavePDOs;

            ec_sync_info_t* m_SlaveSyncs;

            SlaveInfo m_SlaveInfo;

            Offset* m_SlaveOffsets; 

            private:

            bool LOGGING_ENABLED = false;

        };

    }
    
} // end of namespace ethercat_interface

#endif // SLAVE_HPP
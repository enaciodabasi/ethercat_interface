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
#include <string>
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
            uint16_t RxPDO_start,
            unsigned int RxPDO_size,
            uint16_t TxPDO_start,
            unsigned int TxPDO_size
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
             * @param masterPtr: Pointer to the EtherCAT master associated with the slave.
             * @param domainPtr: Pointer to the domain which the slave is registered in. 
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

            inline ec_slave_config_state_t getCurrentSlaveState() const
            {
                return m_CurrentSlaveState;
            }

            inline void setOffsetPtr(Offset* offset)
            {
                m_SlaveOffsets = offset;
            }

            void updateSlaveState();

            /**
             * @brief 
             * 
             * @tparam T: Type to read. 
             * @param value_to_read_name: Name of the value to write. Must be the same as the one defined in the Offset.
             * @param domain_process_data_ptr: Pointer to the address of the domain's Process Data.
             * @param bit_position: Used when using the EC_READ_BIT function. Defaults to NULL.
             * @return From the EtherCAT Master retrived value of type T. 
             */
            template<typename T>
            auto readFromSlave(
                const std::string& value_to_read_name, 
                uint8_t* domain_process_data_ptr = nullptr,
                int bit_position = NULL
            );
            
            /**
             * @brief 
             * 
             * @tparam T: Type to write.
             * @param value_to_write_name: Name of the value to write. Must be the same as the one defined in the Offset. 
             * @param new_val: New value of type T to write to the slave.
             * @param domain_process_data_ptr: Pointer to the address of the domain's Process Data.
             * @param bit_position: Used when using the EC_WRITE_BIT function. Defaults to NULL.
             */
            template<typename T>
            void writeToSlave(
                const std::string& value_to_write_name,
                const T& new_val, 
                uint8_t* domain_process_data_ptr = nullptr,
                int bit_position = NULL
            );

            inline void setDomainProcessDataPtr(uint8_t* domain_data_ptr)
            {
                this->m_DomainProcessDataPtr = domain_data_ptr;
            }

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

            uint8_t* m_DomainProcessDataPtr = nullptr;

            private:

            bool LOGGING_ENABLED = false;

        };

        template<typename T>
        auto Slave::readFromSlave(const std::string& value_to_read_name, uint8_t* domain_process_data_ptr, int bit_position)
        {
            if(domain_process_data_ptr != nullptr)
            {
                m_DomainProcessDataPtr = domain_process_data_ptr;
            }

            auto data = m_DomainProcessDataPtr + *m_SlaveOffsets->getData(value_to_read_name);

            /*
                Control statement for determining which type of value is used with the template function.
                Uses the appropriate macro function from the ecrt.h to read according to the template argument.
             */

            if constexpr (std::is_same_v<uint8_t, T>)
            {
               return EC_READ_U8(
                    data
               ); 
            }
            else if constexpr (std::is_same_v<uint16_t, T>)
            {
                return EC_READ_U16(
                    data
                );
            }
            else if constexpr (std::is_same_v<uint32_t, T>)
            {
                return EC_READ_U32(
                    data
                );
            }
            else if constexpr (std::is_same_v<uint64_t, T>)
            {
                return EC_READ_U64(
                    data
                );
            }
            else if constexpr (std::is_same_v<int8_t, T>)
            {
                return EC_READ_S8(
                    data
                );
            }
            else if constexpr (std::is_same_v<int16_t, T>)
            {
                return EC_READ_S16(
                    data
                );
            }
            else if constexpr (std::is_same_v<int32_t, T>)
            {
                return EC_READ_S32(
                    data
                );
            }
            else if constexpr (std::is_same_v<int64_t, T>)
            {
                return EC_READ_S64(
                    data
                );
            }
            else if constexpr (std::is_same_v<float, T>)
            {
                return EC_READ_REAL(
                    data
                );
            }
            else if constexpr (std::is_same_v<double, T>)
            {
                return EC_READ_LREAL(
                    data
                );
            }
            else if constexpr (std::is_same_v<bool, T>)
            {
                return EC_READ_BIT(
                    data,
                    bit_position
                );
            }
            else
            {
                std::cout << "Incorrect type\n";
            }

            
        }

        template<typename T>
        void Slave::writeToSlave(
            const std::string& value_to_write_name,
            const T& new_val,
            uint8_t* domain_process_data_ptr,
            int bit_position
        )
        {

            if(domain_process_data_ptr != nullptr)
            {
                m_DomainProcessDataPtr = domain_process_data_ptr;
            }

            auto data = m_DomainProcessDataPtr + *m_SlaveOffsets->getData(value_to_write_name);

            /*
                Control statement for determining which type of value is used with the template function.
                Uses the appropriate macro function from the ecrt.h to write according to the template argument.
             */
            if constexpr (std::is_same_v<uint8_t, T>)
            {
               EC_WRITE_U8(
                    data,
                    new_val
               ); 
            }
            else if constexpr (std::is_same_v<uint16_t, T>)
            {
                EC_WRITE_U16(
                    data,
                    new_val
                );
            }
            else if constexpr (std::is_same_v<uint32_t, T>)
            {
                EC_WRITE_U32(
                    data,
                    new_val
                );
            }
            else if constexpr (std::is_same_v<uint64_t, T>)
            {
                EC_WRITE_U64(
                    data,
                    new_val
                );
            }
            else if constexpr (std::is_same_v<int8_t, T>)
            {
                EC_WRITE_S8(
                    data,
                    new_val
                );
            }
            else if constexpr (std::is_same_v<int16_t, T>)
            {
                EC_WRITE_S16(
                    data,
                    new_val
                );
            }
            else if constexpr (std::is_same_v<int32_t, T>)
            {
                EC_WRITE_S32(
                    data,
                    new_val
                );
            }
            else if constexpr (std::is_same_v<int64_t, T>)
            {
                EC_WRITE_S64(
                    data,
                    new_val
                );
            }
            else if constexpr (std::is_same_v<float, T>)
            {
                EC_WRITE_REAL(
                    data,
                    new_val
                );
            }
            else if constexpr (std::is_same_v<double, T>)
            {
                EC_WRITE_LREAL(
                    data,
                    new_val
                );
            }
            else if constexpr (std::is_same_v<bool, T>)
            {
                EC_WRITE_BIT(
                    data,
                    bit_position,
                    new_val
                );
            }
            else
            {
                // TODO: Throw an exception.
            }
        }

        

    }
    
} // end of namespace ethercat_interface

#endif // SLAVE_HPP
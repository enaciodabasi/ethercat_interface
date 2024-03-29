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

#ifndef SLAVE_HPP_
#define SLAVE_HPP_

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <variant>
#include <map>
#include <optional>
#include <queue>
#include <deque>

#include "ecrt.h"

#include "offset.hpp"
#include "utilities.hpp"
#include "state.hpp"
#include "logger.hpp"
#include "state_machine.hpp"


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
            ec_pdo_entry_info_t* entriesArray,
            std::vector<uint16_t> rxPdo_indexes,
            std::vector<uint16_t> txPdo_indexes
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

        ec_pdo_info_t* assignDefaultPDOs(const std::vector<uint16_t>& pdo_mapping_indexes);

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

        ec_sync_info_t* createDefaultSlaveSyncInfo(
            ec_pdo_info_t* default_pdos,
            const std::vector<uint16_t>& rxpdo_indexes,
            const std::vector<uint16_t>& txpdo_indexes
        );

        /**
         * @brief: Base class for implementing an EtherCAT Slave using the IGH EtherCAT Master Library. 
         * 
         */
        class Slave
        {
            public:

            Slave(const SlaveInfo& slave_info);

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
                ec_domain_t* domainPtr,
                ec_slave_config_t** slave_config_ptr
            );

            inline const std::string getSlaveName() const
            {
                return m_SlaveName;
            }

            inline const SlaveInfo getSlaveInfo() const
            {
                return m_SlaveInfo;
            }

            inline const SlaveType getSlaveType() const
            {
                return m_SlaveInfo.slaveType;
            }

            offset::DataOffset* getOffset()
            {
                return m_DataOffset;
            }

            inline void setOffsetPtr(offset::DataOffset* offset)
            {
                m_DataOffset = offset;
            }

            inline void setLogger(std::shared_ptr<logger::Logger> shared_logger)
            {
                m_Logger = shared_logger;
            }

            inline uint16_t getControlWord()
            {
                return m_InnerStateMachine.getCW();
            }

            const std::optional<std::string> getStateString(){
                const auto found = state_machine::CIA402::StateStrings.find(m_InnerStateMachine.m_LastState);
                if(found == state_machine::CIA402::StateStrings.end()){
                    return std::nullopt;
                }

                return found->second;
            }

            /* void updateSlaveState(); */

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
            std::optional<T> readFromSlave(
                const std::string& value_to_read_name, 
                std::optional<int> bit_position = std::nullopt
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
                std::optional<int> bit_position = std::nullopt
            );

            template<typename T, std::size_t SIZE>
            std::optional<std::vector<T>> readArray(const std::string& pdo_name);

            template<typename T>
            void writeArray(const std::string& pdo_name, const std::vector<T>& value_array);

            inline void setDomainProcessDataPtr(uint8_t* domain_data_ptr)
            {
                this->m_DomainProcessDataPtr = domain_data_ptr;
            }

            /**
             * @brief Enables to Slave to operate using its state machine.
             * 
             * @returns true if the slave state is Operation Enabled, false otherwise. 
             */
            virtual bool enableOperation();

            virtual bool shutdown();

            protected:

            std::string m_SlaveName;

            /* ec_slave_config_t* m_SlaveConfig;

            ec_slave_config_state_t m_CurrentSlaveState; */

            ec_pdo_entry_info_t* m_SlavePdoEntries;

            ec_pdo_info_t* m_SlavePDOs;

            ec_sync_info_t* m_SlaveSyncs;

            SlaveInfo m_SlaveInfo;

            Offset* m_SlaveOffsets; 

            //std::unique_ptr<offset::DataOffset> m_DataOffset;
            
            offset::DataOffset* m_DataOffset;

            uint8_t* m_DomainProcessDataPtr = nullptr;

            std::shared_ptr<logger::Logger> m_Logger;

            bool LOGGING_ENABLED = false;

            bool ENABLE_DC = true;

            DC_Info m_DcInfo;

            std::shared_ptr<uint16_t> m_SlaveStatus;

            SlaveStatus m_Status; // Status Word read from the EtherCAT master.

            state_machine::CIA402::StateMachine m_InnerStateMachine;

            bool m_StatusWordSeventhBit = false;

            uint16_t m_FaultResetRetries = 0;

            /* state_machine::CIA402::StateMachine* m_StateMachine; */

        };

        template<typename T>
        std::optional<T> Slave::readFromSlave(const std::string& value_to_read_name, std::optional<int> bit_position)
        {

            //auto data = m_DomainProcessDataPtr + *m_SlaveOffsets->getData(value_to_read_name);
            if(m_DataOffset->getDataOffset(value_to_read_name) == std::nullopt)
            {
                return std::nullopt;
            }        
        
            auto data = m_DomainProcessDataPtr + *m_DataOffset->getDataOffset(value_to_read_name).value();
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
                if(bit_position == std::nullopt)
                {
                    return std::nullopt;
                }
                return EC_READ_BIT(
                    data,
                    bit_position.value()
                );
            }

            return std::nullopt;
            
        }
        
        template<typename T>
        void Slave::writeToSlave(
            const std::string& value_to_write_name,
            const T& new_val,
            std::optional<int> bit_position
        )
        {

            //auto data = m_DomainProcessDataPtr + *m_SlaveOffsets->getData(value_to_write_name);
            if(m_DataOffset->getDataOffset(value_to_write_name) == std::nullopt)
            {
                return;
            }        
        
            auto data = m_DomainProcessDataPtr + *m_DataOffset->getDataOffset(value_to_write_name).value();
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
                if(bit_position != std::nullopt)
                {
                    EC_WRITE_BIT(data, bit_position.value(), new_val);
                }
            }
            else
            {
                // TODO: Throw an exception.
            }
        }


        template<typename T, std::size_t SIZE>
        std::optional<std::vector<T>> Slave::readArray(const std::string& pdo_name)
        {
            if(m_DataOffset->getDataOffset(pdo_name) == std::nullopt)
            {
                return std::nullopt;
            }        

            
            uint8_t* data = m_DomainProcessDataPtr + *m_DataOffset->getDataOffset(pdo_name).value();
            if(!data)
            {
                std::cout << "Data ptr = nullptr" << std::endl;
            }
            std::vector<T> byteArray;
            
            bool firstByte = true;
            std::size_t memoryOffset = 0;
            
            while(byteArray.size() != SIZE)
            {
                if(firstByte)
                {
                    memoryOffset = 0;
                }
                else
                {
                    memoryOffset += 1;
                }
                T byte = 20;
                firstByte = false;
                if constexpr (std::is_same_v<uint8_t, T> || std::is_same_v<int8_t, T>)
                {   
                    /* std::cout << "Addr: " << *((uint16_t*)(data)) << std::endl; */
                    byte = ((T) *((uint8_t*)(data + memoryOffset)));
                }
                else if constexpr (std::is_same_v<uint16_t, T> || std::is_same_v<int16_t, T>)
                {
                    byte = ((T) *(uint16_t*)((void*)(data + memoryOffset)));
                }
                else if constexpr (std::is_same_v<uint32_t, T> || std::is_same_v<int32_t, T>)
                {
                    byte = ((T) *(uint32_t*)((void*)(data + memoryOffset)));
                }
                else if constexpr (std::is_same_v<uint64_t, T> || std::is_same_v<int64_t, T>)
                {
                    byte = ((T) *(uint64_t*)((void*)(data + memoryOffset)));
                }
                else
                {
                    break;
                }
                byteArray.emplace_back(byte);
                
            }
            
            if(byteArray.size() != SIZE || byteArray.empty())
            {
                return std::nullopt;
            } 

            return byteArray;
        }

        template<typename T>
        void Slave::writeArray(const std::string& pdo_name, const std::vector<T>& value_array)
        {
            if(m_DataOffset->getDataOffset(pdo_name) == std::nullopt)
            {   
                return;
            }        
        
            auto data = m_DomainProcessDataPtr + *m_DataOffset->getDataOffset(pdo_name).value();

            int16_t remainingBytes = value_array.size();
            bool initialWrite = true;
            std::queue<T> valueQueue;
            for(const auto val : value_array)
                valueQueue.push(val);

            std::size_t memoryOffset = 0;

            while(!valueQueue.empty())
            {
                
                if(initialWrite)
                {
                    memoryOffset = 0;
                    initialWrite = false;
                }
                else
                {
                    memoryOffset = sizeof(T);
                }
                
                const auto value = valueQueue.front();

                if constexpr (std::is_same_v<uint8_t, T> || std::is_same_v<int8_t, T>)
                {
                    
                    *((uint8_t*)(data + memoryOffset)) = ((uint8_t)(value));
                }
                else if constexpr (std::is_same_v<uint16_t, T> || std::is_same_v<int16_t, T>)
                {
                    *((uint16_t*)(data + memoryOffset)) = ((uint16_t)(value));
                }
                else if constexpr (std::is_same_v<uint32_t, T> || std::is_same_v<int32_t, T>)
                {
                    *((uint32_t*)(data + memoryOffset)) = ((uint32_t)(value));
                }
                else if constexpr (std::is_same_v<uint64_t, T> || std::is_same_v<int64_t, T>)
                {
                    *((uint64_t*)(data + memoryOffset)) = ((uint64_t)(value));
                }
                else
                {
                    break;
                }

                valueQueue.pop();
                remainingBytes -= 1;
                
            }

        }

    }
    
} // end of namespace ethercat_interface

#endif // SLAVE_HPP
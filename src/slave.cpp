#include "slave.hpp"
#include <optional>

namespace ethercat_interface
{   
    
    namespace slave
    {
        Slave::Slave(const std::string& slave_name, SlaveInfo slave_info, Offset* offset)
            : m_SlaveName(slave_name), m_SlaveInfo(slave_info)
        {
            
            if(offset != nullptr)
            {
                m_SlaveOffsets = offset;
            }
    
        }

        Slave::~Slave()
        {
            delete m_SlaveOffsets;
        }

        void Slave::configure_slave()
        {
            
        }

        void Slave::checkSlaveState()
        {
            ec_slave_config_state_t state;
            ecrt_slave_config_state(
                this->m_EthercatSlavePtr,
                &state
            );
            
            this->m_SlaveConfigState = state;
        }

        /*
        -------------------------------------------------------
        -------------------------------------------------------
        Function definitions of namespace ethercat_interface::slave 
        -------------------------------------------------------
        ------------------------------------------------------- 
         */
    
        ec_pdo_entry_info_t createSlavePdoEntryStruct(
            uint16_t index,
            uint8_t subindex,
            uint8_t bit_length
        )
        {
            
            ec_pdo_entry_info_t pdoEntry;

            pdoEntry.index = index;
            pdoEntry.subindex = subindex;
            pdoEntry.bit_length = bit_length;

            return pdoEntry;
        }

        ec_pdo_entry_info_t* createSlavePdoEntries(
            std::vector<uint16_t> indexes,
            std::vector<uint8_t> subindexes,
            std::vector<uint16_t> bit_lengths
        )
        {
            std::size_t numEntries = indexes.size();

            ec_pdo_entry_info_t* entries = new ec_pdo_entry_info_t[numEntries];

            for(std::size_t i = 0; i < numEntries; i++)
            {
                *(entries + i) = createSlavePdoEntryStruct(
                    indexes[i],
                    subindexes[i],
                    bit_lengths[i]
                );
            }

            return entries;
        }

        std::vector<ec_pdo_entry_info_t> createSlavePdoEntriesVector(
            std::vector<uint16_t> indexes,
            std::vector<uint8_t> subindexes,
            std::vector<uint8_t> bit_lengths
        )
        {
            std::size_t numEntries = indexes.size();
            std::vector<ec_pdo_entry_info_t> entriesVec;

            for(std::size_t i = 0; i < numEntries; i++)
            {
                entriesVec.emplace_back(createSlavePdoEntryStruct(
                    indexes[i],
                    subindexes[i],
                    bit_lengths[i]
                ));
            }
        }

        ec_pdo_info_t* createSlavePDOs(
            ec_pdo_entry_info_t* entriesArray,
            int RxPDO_start,
            int RxPDO_size,
            int TxPDO_start,
            int TxPDO_size
        )
        {           

            ec_pdo_info_t slavePDOs[] = {
                {RxPDO_start, RxPDO_size, entriesArray + 0},
                {TxPDO_start, TxPDO_size, entriesArray + RxPDO_size}
            };
            
            return slavePDOs;
        }                                                              

        // -----------------------------------
        ec_pdo_info_t* createSlavePDOs(
            std::size_t num_entries,
            ec_pdo_entry_info_t* entriesArray,
            uint16_t RxPDO_start,
            uint16_t TxPDO_start,
            int RxPDO_size,
            int TxPDO_size
        )
        {   
            
            ec_pdo_info_t* slavePDOs = new ec_pdo_info_t[num_entries];
            int t_counter = 0;
            for(std::size_t i = 0; i < RxPDO_size + TxPDO_size; i++)
            {   
                if(i < RxPDO_size)
                    *(slavePDOs + i) = {(uint16_t)(RxPDO_start + i), 1, (entriesArray + i)};
                else
                {
                    *(slavePDOs + i) = {(uint16_t)(TxPDO_start + t_counter), 1, (entriesArray + i)};
                    t_counter += 1;
                }
                    
            }

            return slavePDOs;
        }


        // -----------------------------------
        ec_pdo_info_t* createSlavePDOs(
            std::vector<ec_pdo_entry_info_t>& entries_vector,
            uint16_t RxPDO_start,
            uint16_t TxPDO_start
        )
        {
            ec_pdo_info_t* slavePDOs = new ec_pdo_info_t[entries_vector.size()];

            for(std::size_t i = 0; i < entries_vector.size(); i++)
            {
                *(slavePDOs + i) = {(uint16_t)(RxPDO_start + i), 1, &entries_vector[i]};
            }
        }

        // -----------------------------------
        ec_pdo_entry_reg_t* createDomainRegistries(
            uint16_t slave_alias,
            uint16_t slave_position,
            uint32_t slave_vendor_id,
            uint32_t slave_product_code,
            std::vector<uint16_t> indexes,
            std::vector<uint8_t> subindexes,
            Offset* offset,
            unsigned int* bit_position
        )
        {   
            std::size_t numRegistries = indexes.size();
            ec_pdo_entry_reg_t* domain_registries = new ec_pdo_entry_reg_t[numRegistries];

            for(std::size_t i = 0; i < numRegistries; i++)
            {
                *(domain_registries + i) = {
                    slave_alias,
                    slave_position,
                    slave_vendor_id,
                    slave_product_code,
                    indexes.at(i),
                    subindexes.at(i),
                    offset->getData(offset->m_OffsetNameIndexes[i]),
                    NULL
                };
            }
        }

        // -----------------------------------
        /**
         * @brief Create a Slave Syncs object
         * 
         * @param num_sync_managers 
         * @param sync_directions 
         * @param number_of_pdos 
         * @param index_to_add_to_pdo 
         * @param pdos 
         * @param watchdog_modes 
         * @return ec_sync_info_t* 
         */
        ec_sync_info_t* createSlaveSyncs(
            uint8_t num_sync_managers,
            std::vector<ec_direction_t> sync_directions,
            std::vector<uint> number_of_pdos,
            std::vector<std::optional<int>> index_to_add_to_pdo,
            ec_pdo_info_t* pdos,
            std::vector<ec_watchdog_mode_t> watchdog_modes
        )
        {
            ec_sync_info_t* slaveSyncs = new ec_sync_info_t[num_sync_managers + 1];

            for(uint8_t i = 0; i < num_sync_managers; i++)
            {   
                if(index_to_add_to_pdo[i] == std::nullopt)
                {
                    *(slaveSyncs + 1) = {
                        i,
                        sync_directions[i],
                        number_of_pdos[i],
                        NULL,
                        watchdog_modes[i]
                    };
                }
                else
                    *(slaveSyncs + 1) = {
                        i,
                        sync_directions[i],
                        number_of_pdos[i],
                        pdos + index_to_add_to_pdo[i].value(),
                        watchdog_modes[i]
                    };
            }

            *(slaveSyncs + num_sync_managers) = {0xff};

            return slaveSyncs;
        }

    }

} // End of namespace ethercat_interface
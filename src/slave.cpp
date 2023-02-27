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
                std::cout << "Created Offset." << std::endl;
            }

            m_SlaveSyncs = new ec_sync_info_t[m_SlaveInfo.slaveSyncInfo.numSyncManagers + 1];
            m_SlavePdoEntryRegistries = new ec_pdo_entry_reg_t[m_SlaveInfo.pdoEntryInfo.indexes.size() + 1];
            m_SlavePdoEntries = new ec_pdo_entry_info_t[m_SlaveInfo.pdoEntryInfo.indexes.size()];
            m_SlavePDOs = new ec_pdo_info_t[2];
        }

        Slave::~Slave()
        {   
            delete m_SlaveConfig;
            delete m_SlavePdoEntryRegistries;
            delete m_SlavePdoEntries;
            delete m_SlavePDOs;
            delete m_SlaveSyncs;
            delete m_SlaveOffsets;
        }

        void Slave::configure_slave()
        {
            m_SlavePdoEntryRegistries = ethercat_interface::slave::createDomainRegistries(
            m_SlaveInfo.alias,
            m_SlaveInfo.position,
            m_SlaveInfo.vendorID,
            m_SlaveInfo.productCode,
            m_SlaveInfo.pdoEntryInfo.indexes,
            m_SlaveInfo.pdoEntryInfo.subindexes,
            m_SlaveOffsets);

        m_SlavePdoEntries = ethercat_interface::slave::createSlavePdoEntries(
            m_SlaveInfo.pdoEntryInfo.indexes,
            m_SlaveInfo.pdoEntryInfo.subindexes,
            m_SlaveInfo.pdoEntryInfo.bitLengths);

        m_SlavePDOs = ethercat_interface::slave::createSlavePDOs(
            m_SlavePdoEntries,
            m_SlaveInfo.ioMappingInfo.RxPDO_Address,
            m_SlaveInfo.ioMappingInfo.RxPDO_Size,
            m_SlaveInfo.ioMappingInfo.TxPDO_Address,
            m_SlaveInfo.ioMappingInfo.TxPDO_Size);

        m_SlaveSyncs = ethercat_interface::slave::createSlaveSyncs(
            m_SlaveInfo.slaveSyncInfo.numSyncManagers,
            ethercat_interface::utilities::intToEcDirectionEnum(m_SlaveInfo.slaveSyncInfo.syncManagerDirections),
            m_SlaveInfo.slaveSyncInfo.numPDOs,
            m_SlaveInfo.slaveSyncInfo.pdoIndexDiff,
            m_SlavePDOs,
            ethercat_interface::utilities::intToEcWatchdogEnum(m_SlaveInfo.slaveSyncInfo.watchdogModes));

            std::cout << "Configured slave " << m_SlaveName << std::endl;
        }

        void Slave::setupSlave(ec_master_t *masterPtr, ec_domain_t* domainPtr)
        {

            m_SlaveConfig = ecrt_master_slave_config(
                masterPtr,
                m_SlaveInfo.alias,
                m_SlaveInfo.position,
                m_SlaveInfo.vendorID,
                m_SlaveInfo.productCode
            );

        
            if(!m_SlaveConfig)
            {
                std::cout << "Can't create slave config" << std::endl;
            }

            if(ecrt_slave_config_pdos(m_SlaveConfig, EC_END, m_SlaveSyncs) != 0)
            {
                std::cout << "Failed to create Slave Config PDOs." << std::endl;
            }
            else
            {
                std::cout << "Creation of slave config pdos is successful" << std::endl;
            }
            
            std::cout << "Checking PDO Entry Registries" << std::endl;
            if(ecrt_domain_reg_pdo_entry_list(domainPtr, m_SlavePdoEntryRegistries))
            {
                std::cout << "Failed during PDO entry registries check." << std::endl;
            }

            std::cout << "Slave config setup complete." << std::endl;
        }

        void Slave::checkSlaveState()
        {
            ec_slave_config_state_t state;
            ecrt_slave_config_state(
                this->m_SlaveConfig,
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
            std::cout << RxPDO_start << " " << RxPDO_size << " " << TxPDO_start << " " << TxPDO_size << std::endl;
            ec_pdo_info_t slavePDOs[] = {
                {RxPDO_start, RxPDO_size, entriesArray + 0},
                {TxPDO_start, TxPDO_size, entriesArray + RxPDO_size}
            };

            ec_pdo_info_t* pdos = new ec_pdo_info_t[2];
            pdos[0] = {(uint16_t)RxPDO_start, (uint)RxPDO_size, entriesArray + 0};
            pdos[1] = {(uint16_t)TxPDO_start, (uint)TxPDO_size, entriesArray + RxPDO_size};
            
            return pdos;
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
            ec_pdo_entry_reg_t* domain_registries = new ec_pdo_entry_reg_t[numRegistries + 1];
            
            for(std::size_t i = 0; i <= numRegistries; i++)
            {
                if(i == numRegistries)
                {
                    domain_registries[i] = {};
                    break; 
                }
                //std::cout << offset->m_OffsetNameIndexes[i] << std::endl;
                unsigned int* op = new unsigned int();
                op = offset->getData(offset->m_OffsetNameIndexes.at(i));
                domain_registries[i] = {
                    slave_alias,
                    slave_position,
                    slave_vendor_id,
                    slave_product_code,
                    indexes.at(i),
                    subindexes.at(i),
                    op
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
            for(int i = 0; i < num_sync_managers + 1; i++)
            {

            }
            for(int i = 0; i < (int)num_sync_managers; i++)
            {   
                if(index_to_add_to_pdo[i] == std::nullopt)
                {   
                    ec_sync_info_t syncInfo;
                    syncInfo.index = (uint8_t)i;
                    syncInfo.dir = sync_directions[i];
                    syncInfo.n_pdos = number_of_pdos[i];
                    syncInfo.pdos = NULL;
                    syncInfo.watchdog_mode = watchdog_modes[i];
                    slaveSyncs[i] = syncInfo;
                }
                else
                {
                    ec_sync_info_t syncInfo;
                    syncInfo.index = (uint8_t)i;
                    syncInfo.dir = sync_directions[i];
                    syncInfo.n_pdos = number_of_pdos[i];
                    syncInfo.watchdog_mode = watchdog_modes[i];
                    syncInfo.pdos = pdos + index_to_add_to_pdo[i].value();
                    slaveSyncs[i] = syncInfo;

                }
                    

                    
            }

            *(slaveSyncs + (int)num_sync_managers) = {0xff};

            return slaveSyncs;
        }

    }

} // End of namespace ethercat_interface
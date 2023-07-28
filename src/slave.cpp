/**
 * 
 */

#include "ethercat_interface/slave.hpp"

#include <optional>

namespace ethercat_interface
{   
    using namespace logger;
    namespace slave
    {

        Slave::Slave(const SlaveInfo& slave_info)
            : m_SlaveInfo{slave_info}
        {
            
            m_SlaveName = m_SlaveInfo.slaveName;

            if(m_SlaveInfo.slaveType != SlaveType::Coupler || !m_SlaveInfo.ioMappingInfo.useDefaultPdoMapping)
            {
                if(!m_SlaveInfo.pdoNames.empty())
                {
                    m_DataOffset = new offset::DataOffset(m_SlaveInfo.pdoNames);
                }

                m_SlaveSyncs = new ec_sync_info_t[m_SlaveInfo.slaveSyncInfo.numSyncManagers + 1];

                m_SlavePdoEntries = new ec_pdo_entry_info_t[m_SlaveInfo.pdoEntryInfo.indexes.size()];

                m_SlavePDOs = new ec_pdo_info_t[m_SlaveInfo.ioMappingInfo.RxPDO_Indexes.size() + m_SlaveInfo.ioMappingInfo.TxPDO_Indexes.size()];

                m_DataOffset = new offset::DataOffset(m_SlaveInfo.pdoNames);
                if(std::holds_alternative<bool>(m_SlaveInfo.dcInfo) || m_SlaveInfo.slaveType == SlaveType::PLC)
                {
                    ENABLE_DC = false;
                }

                if(ENABLE_DC && std::holds_alternative<DC_Info>(m_SlaveInfo.dcInfo))
                {
                    m_DcInfo = std::get<DC_Info>(m_SlaveInfo.dcInfo);
                }

                m_InnerStateMachine.init();
            }

            //m_DataOffset = std::make_unique<offset::DataOffset>(m_SlaveInfo.pdoNames);
            
        
        }

        Slave::~Slave()
        {   
            //delete m_SlaveOffsets;
            delete m_SlavePdoEntries;
            delete m_SlavePDOs;
            delete m_SlaveSyncs;
            delete m_DomainProcessDataPtr;
        }

        void Slave::configure_slave()
        {

            
            m_SlavePdoEntries = ethercat_interface::slave::createSlavePdoEntries(
                m_SlaveInfo.pdoEntryInfo.indexes,
                m_SlaveInfo.pdoEntryInfo.subindexes,
                m_SlaveInfo.pdoEntryInfo.bitLengths)
            ;

            //if(m_SlaveInfo.ioMappingInfo.useDefaultPdoMapping)
            //{
            //    /* std::vector<uint16_t> defaultMappings = m_SlaveInfo.ioMappingInfo.RxPDO_Indexes;
            //    defaultMappings.insert(
            //        defaultMappings.end(),
            //        m_SlaveInfo.ioMappingInfo.TxPDO_Indexes.begin(),
            //        m_SlaveInfo.ioMappingInfo.TxPDO_Indexes.end()
            //    );
//
            //    m_SlavePDOs = assignDefaultPDOs(
            //        defaultMappings
            //    );
//
            //    m_SlaveSyncs = createDefaultSlaveSyncInfo(
            //        m_SlavePDOs,
            //        m_SlaveInfo.ioMappingInfo.RxPDO_Indexes,
            //        m_SlaveInfo.ioMappingInfo.TxPDO_Indexes
            //    ); */
//
//
            //    
            //}
            if(m_SlaveInfo.ioMappingInfo.RxPDO_Indexes.size() == 1 && m_SlaveInfo.ioMappingInfo.TxPDO_Indexes.size() == 1)
            {   
                m_SlavePDOs = createSlavePDOs(
                    m_SlavePdoEntries,
                    m_SlaveInfo.ioMappingInfo.RxPDO_Address,
                    m_SlaveInfo.ioMappingInfo.RxPDO_Size,
                    m_SlaveInfo.ioMappingInfo.TxPDO_Address,
                    m_SlaveInfo.ioMappingInfo.TxPDO_Size
                );
            }
            else
            {
                m_SlavePDOs = ethercat_interface::slave::createSlavePDOs(
                    m_SlavePdoEntries,
                    m_SlaveInfo.ioMappingInfo.RxPDO_Indexes,
                    m_SlaveInfo.ioMappingInfo.TxPDO_Indexes
                );
            }

            if(!m_SlaveInfo.ioMappingInfo.useDefaultPdoMapping)
            {
                m_SlaveSyncs = ethercat_interface::slave::createSlaveSyncs(
                    m_SlaveInfo.slaveSyncInfo.numSyncManagers,
                    m_SlaveInfo.slaveSyncInfo.syncManagerDirections,
                    m_SlaveInfo.slaveSyncInfo.numPDOs,
                    m_SlaveInfo.slaveSyncInfo.pdoIndexDiff,
                    m_SlavePDOs,
                    m_SlaveInfo.slaveSyncInfo.watchdogModes
                );

            }
/*             m_Logger->log(INFO, m_SlaveName, "Loaded slave configuration.");
 */        }

        void Slave::setupSlave(ec_master_t *masterPtr, ec_domain_t* domainPtr, ec_slave_config_t** slave_config_ptr)
        {
            
            (*slave_config_ptr) = ecrt_master_slave_config(
                masterPtr,
                m_SlaveInfo.alias,
                m_SlaveInfo.position,
                m_SlaveInfo.vendorID,
                m_SlaveInfo.productCode
            );
        
            if(!slave_config_ptr)
            { 
/*              m_Logger->log(ERROR, m_SlaveName, "Can't create EC slave config.");
 */         }

            if(m_SlaveInfo.slaveType == SlaveType::Coupler)
            {
                return;
            }

            /* if(m_SlaveInfo.ioMappingInfo.useDefaultPdoMapping)
            {   

                if(ecrt_slave_config_pdos((*slave_config_ptr), 2, m_SlaveSyncs) != 0)
                {
                    std::cout << "Error while configuring PDOs" << std::endl;
                }
                else
                {
                    std::cout << "Configured PDOs" << std::endl;
                }
            } */
            
            if((!m_SlaveInfo.ioMappingInfo.useDefaultPdoMapping))
            {
                std::cout << "Configuring custom PDOs." << std::endl;
                if(ecrt_slave_config_pdos((*slave_config_ptr), EC_END, m_SlaveSyncs) != 0)
                {
                
                }
                else
                {

                }
            }
            

            if(ENABLE_DC && m_SlaveInfo.slaveType != SlaveType::PLC)
            {
                ecrt_slave_config_dc(
                    (*slave_config_ptr),
                    m_DcInfo.assign_activate,
                    m_DcInfo.sync0_cycle,
                    m_DcInfo.sync0_shift,
                    m_DcInfo.sync1_cycle,
                    m_DcInfo.sync1_shift
                );  
            }

            /* for(const auto p : m_SlaveInfo.pdoNames)
            {
                std::cout << *m_DataOffset->getDataOffset(p).value() << std::endl;
            } */

/*             m_Logger->log(INFO, m_SlaveName, "Successfully set up the slave.");
 */        }


        bool Slave::enableOperation()
        {
            
            for(const auto p : m_SlaveInfo.pdoNames)
            {
                std::cout  << p << ": " << *m_DataOffset->getDataOffset(p).value() << std::endl;
            }
            if(m_SlaveInfo.slaveType  == SlaveType::PLC)
            {

                return true;
            
            }    
            const auto status_word = readFromSlave<uint16_t>("status_word");
            
            if(!status_word)
            {
                return false;
            }
            
            m_Status = status_word.value();
/*             std::cout << "Status Word: " << m_Status << std::endl;
 */            if(state_machine::CIA402::detectCurrentState(status_word.value()) == state_machine::CIA402::State::OperationEnabled)
            {
                /* std::cout << "Operation Enabled state achieved\n"; */
                return true;
            }
        
            const auto control_word = m_InnerStateMachine.getControlWord(
                status_word.value(),
                state_machine::CIA402::State::OperationEnabled
            );

            if(!control_word)
            {
                return false;
            }

            writeToSlave<uint16_t>("ctrl_word", control_word.value());
/*             std::cout << "Control Word: " << control_word.value() << std::endl;
 */
            return false;
            
        }

        bool Slave::shutdown()
        {
            const auto ctrl_word = m_InnerStateMachine.getControlWord(
                m_Status,
                state_machine::CIA402::State::SwitchOnDisabled
            );

            if(!ctrl_word)
            {
                return false;
            }

            writeToSlave<int8_t>("op_mode", 0x00);
            writeToSlave<uint16_t>("ctrl_word", ctrl_word.value());

            return true;
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
                entries[i] = {
                    indexes[i],
                    subindexes[i], 
                    (uint8_t)bit_lengths[i]};
            }

            return entries;
        }

        ec_pdo_info_t* createSlavePDOs(
            ec_pdo_entry_info_t* entriesArray,
            uint16_t RxPDO_start,
            unsigned int RxPDO_size,
            uint16_t TxPDO_start,
            unsigned int TxPDO_size
        )
        {           
            ec_pdo_info_t* pdos = new ec_pdo_info_t[2];
            pdos[0] = {RxPDO_start, RxPDO_size, entriesArray + 0};
            pdos[1] = {TxPDO_start, TxPDO_size, entriesArray + RxPDO_size};
            std::cout << "Create slave PDOs: " << RxPDO_start << " " << RxPDO_size << " ," << TxPDO_start << " " << TxPDO_size << std::endl;
            return pdos;
        }                                                              

        // -----------------------------------
        ec_pdo_info_t* createSlavePDOs(
            ec_pdo_entry_info_t* entriesArray,
            std::vector<uint16_t> rxPdo_indexes,
            std::vector<uint16_t> txPdo_indexes
        )
        {   

            std::size_t pdoSize = rxPdo_indexes.size() + txPdo_indexes.size();
            ec_pdo_info_t* slavePDOs = new ec_pdo_info_t[pdoSize];
            int t_counter = 0;
            for(std::size_t i = 0; i < rxPdo_indexes.size(); i++)
            {   
                *(slavePDOs + t_counter) = {rxPdo_indexes[i], 1, entriesArray + t_counter};
                
                t_counter += 1;
            }

            for(std::size_t i = 0; i < txPdo_indexes.size(); i++)
            {
                *(slavePDOs + t_counter) = {txPdo_indexes[i], 1, entriesArray + t_counter};
                
                t_counter += 1;
            }

            return slavePDOs;
        }

        ec_pdo_info_t* assignDefaultPDOs(const std::vector<uint16_t>& pdo_mapping_indexes)
        {
            const std::size_t pdoSize = pdo_mapping_indexes.size();
            
            ec_pdo_info_t* pdos = new ec_pdo_info_t[pdoSize];
            for(std::size_t i = 0; i < pdoSize; i++)
            {   
                *(pdos + i) = {pdo_mapping_indexes.at(i)};
            }

            return pdos;

        }

        // -----------------------------------
        /* ec_pdo_info_t* createSlavePDOs(
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
        } */

        // -----------------------------------
        ec_pdo_entry_reg_t* createDomainRegistries(
            uint16_t slave_alias,
            uint16_t slave_position,
            uint32_t slave_vendor_id,
            uint32_t slave_product_code,
            std::vector<uint16_t> indexes,
            std::vector<uint8_t> subindexes,
            offset::DataOffset* offset,
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
                const std::string currentDataName = offset->getDataName((uint)i);   
                std::cout << currentDataName << std::endl;     
                //unsigned int* op = new unsigned int();
                //op = offset->getData(offset->m_OffsetNameIndexes.at(i));
                if(offset->getDataOffset(currentDataName) != std::nullopt)
                    domain_registries[i] = {
                        slave_alias,
                        slave_position,
                        slave_vendor_id,
                        slave_product_code,
                        indexes.at(i),
                        subindexes.at(i),
                        //offset->getData(offset->m_OffsetNameIndexes.at(i))
                        offset->getDataOffset(currentDataName).value()
                    };
            }

            return domain_registries;
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

        ec_sync_info_t* createDefaultSlaveSyncInfo(
            ec_pdo_info_t* default_pdos,
            const std::vector<uint16_t>& rxpdo_indexes,
            const std::vector<uint16_t>& txpdo_indexes
        )
        {
            ec_sync_info_t* defaultSyncs = new ec_sync_info_t[2];
            // RxPDOs:
            defaultSyncs[0] = {3, EC_DIR_INPUT, (uint)rxpdo_indexes.size(), default_pdos + 0, };
            //TxPDOs:
            defaultSyncs[1] = {4, EC_DIR_OUTPUT, (uint)txpdo_indexes.size(), default_pdos + rxpdo_indexes.size(), };

            return defaultSyncs; 
        }

    }

} // End of namespace ethercat_interface
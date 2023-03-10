#include "slave.hpp"
#include <optional>


#define VendorID 0x000000fb
#define ProductCode 0x65520000

#define SMBSlavePos 0,0
#define SMB VendorID,ProductCode


namespace ethercat_interface
{   
    
    namespace slave
    {
        Slave::Slave(const std::string& slave_name, SlaveInfo slave_info, Offset* offset, bool enable_logging)
            : m_SlaveName(slave_name), m_SlaveInfo(slave_info), LOGGING_ENABLED(enable_logging)
        {
            
            if(offset != nullptr)
            {
                m_SlaveOffsets = offset;
                std::cout << "Created Offset." << std::endl;
            }

            m_SlaveSyncs = new ec_sync_info_t[m_SlaveInfo.slaveSyncInfo.numSyncManagers + 1];
            //m_SlavePdoEntryRegistries = new ec_pdo_entry_reg_t[m_SlaveInfo.pdoEntryInfo.indexes.size() + 1];
            m_SlavePdoEntries = new ec_pdo_entry_info_t[m_SlaveInfo.pdoEntryInfo.indexes.size()];
            m_SlavePDOs = new ec_pdo_info_t[2];
        }

        Slave::Slave(const std::string& slave_name, const std::string& config_file_path, Offset* offset, bool enable_logging)
            : m_SlaveName(slave_name)
        {
            m_SlaveInfo = utilities::parse_config_file(config_file_path, slave_name);

            if(offset != nullptr)
            {
                m_SlaveOffsets = offset;
            }

            m_SlaveSyncs = new ec_sync_info_t[m_SlaveInfo.slaveSyncInfo.numSyncManagers + 1];

            m_SlavePdoEntries = new ec_pdo_entry_info_t[m_SlaveInfo.pdoEntryInfo.indexes.size()];
            m_SlavePDOs = new ec_pdo_info_t[2];
        }

        Slave::~Slave()
        {   
            //delete m_SlaveOffsets;
        }

        void Slave::configure_slave()
        {

            /* m_SlavePdoEntryRegistries = ethercat_interface::slave::createDomainRegistries(
                m_SlaveInfo.alias,
                m_SlaveInfo.position,
                m_SlaveInfo.vendorID,
                m_SlaveInfo.productCode,
                m_SlaveInfo.pdoEntryInfo.indexes,
                m_SlaveInfo.pdoEntryInfo.subindexes,
                m_SlaveOffsets
            ); */

            /* m_SlavePdoEntryRegistries[0] = {SMBSlavePos, SMB, 0x6040, 0, &off.ctrl_word};
            m_SlavePdoEntryRegistries[1] = {SMBSlavePos, SMB, 0x60FF, 0, &off.target_velocity};
            m_SlavePdoEntryRegistries[2] = {SMBSlavePos, SMB, 0x60B1, 0, &off.velocity_offset};
            m_SlavePdoEntryRegistries[3] = {SMBSlavePos, SMB, 0x6060, 0, &off.operation_mode};
            m_SlavePdoEntryRegistries[4] = {SMBSlavePos, SMB, 0x60FE, 1, &off.digital_output};
	        m_SlavePdoEntryRegistries[5] = {SMBSlavePos, SMB, 0x6041, 0, &off.status_word};
            m_SlavePdoEntryRegistries[6] = {SMBSlavePos, SMB, 0x6064, 0, &off.current_position}; 
	        m_SlavePdoEntryRegistries[7] = {SMBSlavePos, SMB, 0x606C, 0, &off.current_velocity};
	        m_SlavePdoEntryRegistries[8] = {SMBSlavePos, SMB, 0x6077, 0, &off.current_torque};
            m_SlavePdoEntryRegistries[9] = {SMBSlavePos, SMB, 0x6061, 0, &off.mode_display};
            m_SlavePdoEntryRegistries[10] = {SMBSlavePos, SMB, 0x60FD, 0, &off.digital_input};
            m_SlavePdoEntryRegistries[11] = {}; */
            
            m_SlavePdoEntries = ethercat_interface::slave::createSlavePdoEntries(
                m_SlaveInfo.pdoEntryInfo.indexes,
                m_SlaveInfo.pdoEntryInfo.subindexes,
                m_SlaveInfo.pdoEntryInfo.bitLengths)
            ;

            /* m_SlavePdoEntries[0] = {0x6040, 0x00, 16};
            m_SlavePdoEntries[1] = {0x60ff, 0x00, 32};
            m_SlavePdoEntries[2] = {0x60b1, 0x00, 32};
            m_SlavePdoEntries[3] = {0x6060, 0x00, 8};
            m_SlavePdoEntries[4] = {0x60fe, 0x01, 32};
            m_SlavePdoEntries[5] = {0x6041, 0x00, 16};
            m_SlavePdoEntries[6] = {0x6064, 0x00, 32};
            m_SlavePdoEntries[7] = {0x606c, 0x00, 32};
            m_SlavePdoEntries[8] = {0x6077, 0x00, 16};
            m_SlavePdoEntries[9] = {0x6061, 0x00, 8};
            m_SlavePdoEntries[10] = {0x60fd, 0x00, 32}; */

            m_SlavePDOs = ethercat_interface::slave::createSlavePDOs(
                m_SlavePdoEntries,
                m_SlaveInfo.ioMappingInfo.RxPDO_Address,
                m_SlaveInfo.ioMappingInfo.RxPDO_Size,
                m_SlaveInfo.ioMappingInfo.TxPDO_Address,
                m_SlaveInfo.ioMappingInfo.TxPDO_Size
            );

            /* m_SlavePDOs[0]={0x1600, 5, m_SlavePdoEntries + 0};
            m_SlavePDOs[1]={0x1a00, 6, m_SlavePdoEntries + 5}; */

            m_SlaveSyncs = ethercat_interface::slave::createSlaveSyncs(
                m_SlaveInfo.slaveSyncInfo.numSyncManagers,
                ethercat_interface::utilities::intToEcDirectionEnum(m_SlaveInfo.slaveSyncInfo.syncManagerDirections),
                m_SlaveInfo.slaveSyncInfo.numPDOs,
                m_SlaveInfo.slaveSyncInfo.pdoIndexDiff,
                m_SlavePDOs,
                ethercat_interface::utilities::intToEcWatchdogEnum(m_SlaveInfo.slaveSyncInfo.watchdogModes)
            );

                /* m_SlaveSyncs[0] = {0, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE};
                m_SlaveSyncs[1] = {1, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE};
                m_SlaveSyncs[2] = {2, EC_DIR_OUTPUT, 1, m_SlavePDOs + 0, EC_WD_ENABLE};
                m_SlaveSyncs[3] = {3, EC_DIR_INPUT, 1, m_SlavePDOs + 1, EC_WD_DISABLE};
                m_SlaveSyncs[4] = {0xff};
 */

                std::cout << "Configured slave " << m_SlaveName << std::endl;
        }

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
                std::cout << "Can't create slave config" << std::endl;
            }

            if(ecrt_slave_config_pdos((*slave_config_ptr), EC_END, m_SlaveSyncs) != 0)
            {
                std::cout << "Failed to create Slave Config PDOs." << std::endl;
            }
            else
            {
                
                std::cout << "Creation of slave config pdos is successful" << std::endl;
            }
            
            std::cout << "Checking PDO Entry Registries" << std::endl;
            /* if(ecrt_domain_reg_pdo_entry_list(domainPtr, m_SlavePdoEntryRegistries))
            {
                std::cout << "Failed during PDO entry registries check." << std::endl;
            } */
            //slave_config_ptr = tempConf;
            //delete tempConf;
            std::cout << "Slave config setup complete." << std::endl;
        }

        /* void Slave::updateSlaveState()
        {
            ec_slave_config_state_t state;
            ecrt_slave_config_state(
                this->m_SlaveConfig,
                &state
            );
            
            if(this->LOGGING_ENABLED)
            {
                if(state.al_state != m_CurrentSlaveState.al_state)
                {
                    std::cout << "Slave: State " << state.al_state << std::endl;
                }
                if(state.online != m_CurrentSlaveState.online)
                {
                    std::cout << "Slave: " << (state.online ? "online" : "offline") << std::endl;
                }
                if(state.operational != m_CurrentSlaveState.operational)
                {
                    std::cout << "Slave is " << (state.operational ? "" : "not") << "operational" << std::endl;
                }
            }

            this->m_CurrentSlaveState = state;
            
        } */

        bool Slave::enableOperation()
        {
            m_Status = this->readFromSlave<uint16_t>("status_word");
            //if(m_Status == 0) return false;
            //std::cout << m_Status << std::endl;
            if(m_Status & getStatusValue(StatusType::OperationEnabled) == 39)
            {   
                return true;
            }

            /*
                Switch on disabled -> Ready to switch on : Shutdown command.
                Ready to switch on -> Switched on: Switch on command.
                Switched on -> Operation enabled: Enable operation command. 
             */
            // Check if there is a registered fault in the Slave
            if(!(m_Status & getStatusValue(StatusType::Fault)))
            {   
                //std::cout << "no fault\n";
                // If state is Switch on disabled send the Shutdown command.
                if((m_Status & getStatusValue(StatusType::SwitchOnDisabled)))
                {   
                    std::cout << "switch on disabled\n";
                    writeToSlave<uint16_t>("ctrl_word", getCommandValue(ControlCommand::Shutdown));
                    return false;
                }
                //// If state is Ready to Switch On send the Switch On command.
                else if(m_Status & getStatusValue(StatusType::ReadyToSwitchOn) == 4)
                {
                    std::cout << "ready to switch on\n";
                    writeToSlave<uint16_t>("ctrl_word", getCommandValue(ControlCommand::SwitchOn));
                    return false;
                }
                //// If state is Switched On send the Enable Operation command.
                else if(m_Status & getStatusValue(StatusType::SwitchedOn))
                {
                    std::cout << "switched on\n";
                    writeToSlave<uint16_t>("ctrl_word", getCommandValue(ControlCommand::EnableOperation));

                    return false;
                }
                
            }
            else // Try to reset the fault.
            {
                if(m_Status & getStatusValue(StatusType::Fault))
                {
                    // Send ResetFault to the control word.
                    std::cout << "fault exists\n";
                    writeToSlave<uint16_t>("ctrl_word", getCommandValue(ControlCommand::ResetFault));
                    return false;

                }
            }

            return false;
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
                std::cout << indexes[i] << " " << subindexes[i] << " " << bit_lengths[i] << std::endl;
                /* if(subindexes[i] == '0')
                {   
                    std::cout << "sifir";
                    entries[i] = {
                    indexes[i],
                    0x00, 
                    (uint8_t)bit_lengths[i]};
                }
                else if(subindexes[i] == '1')
                {
                    std::cout << "bir";
                    entries[i] = {
                    indexes[i],
                    0x01, 
                    (uint8_t)bit_lengths[i]};
                } */

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
            /* ec_pdo_info_t slavePDOs[] = {
                {RxPDO_start, RxPDO_size, entriesArray + 0},
                {TxPDO_start, TxPDO_size, entriesArray + RxPDO_size}
            }; */
            std::cout << "PDO_INFO\n";
            ec_pdo_info_t* pdos = new ec_pdo_info_t[2];
            pdos[0] = {RxPDO_start, RxPDO_size, entriesArray + 0};
            pdos[1] = {TxPDO_start, TxPDO_size, entriesArray + RxPDO_size};
            std::cout << (uint16_t)RxPDO_start << " " << TxPDO_start << " " << (uint)RxPDO_size << " " << (uint)TxPDO_size << std::endl;
            
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
                std::cout << subindexes[i] << std::endl;
                unsigned int* op = new unsigned int();
                op = offset->getData(offset->m_OffsetNameIndexes.at(i));
                domain_registries[i] = {
                    slave_alias,
                    slave_position,
                    slave_vendor_id,
                    slave_product_code,
                    indexes.at(i),
                    subindexes.at(i),
                    offset->getData(offset->m_OffsetNameIndexes.at(i))
                };
            }
            /* auto v = slave_vendor_id;
            auto p = slave_product_code;
            ec_pdo_entry_reg_t* domain_regs = new ec_pdo_entry_reg_t[numRegistries + 1];
            domain_regs[0] = {0, 0, v, p, 0x6040, 0, &offsett.ctrl_word};
            domain_regs[1] = {0, 0, v, p, 0x60FF, 0, &offsett.target_velocity};
            domain_regs[2] = {0, 0, v, p, 0x60B1, 0, &offsett.velocity_offset};
            domain_regs[3] = {0, 0, v, p, 0x6060, 0, &offsett.operation_mode};
            domain_regs[4] = {0, 0, v, p, 0x60FE, 1, &offsett.digital_output};
	        domain_regs[5] = {0, 0, v, p, 0x6041, 0, &offsett.status_word};
            domain_regs[6] = {0, 0, v, p, 0x6064, 0, &offsett.current_position}; 
	        domain_regs[7] = {0, 0, v, p, 0x606C, 0, &offsett.current_velocity};
	        domain_regs[8] = {0, 0, v, p, 0x6077, 0, &offsett.current_torque};
            domain_regs[9] = {0, 0, v, p, 0x6061, 0, &offsett.mode_display};
            domain_regs[10] = {0, 0, v, p, 0x60FD, 0, &offsett.digital_input};
            domain_regs[11] = {}; */

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

    }

} // End of namespace ethercat_interface
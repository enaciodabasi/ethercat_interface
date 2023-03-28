#include "slave.hpp"
#include <optional>


#define VendorID 0x000000fb
#define ProductCode 0x65520000

#define SMBSlavePos 0,0
#define SMB VendorID,ProductCode


namespace ethercat_interface
{   
    using namespace logger;
    namespace slave
    {

        Slave::Slave(
            const std::string& slave_name,
            const std::string& config_file_path,
            Offset* offset,
            std::shared_ptr<Logger> logger,
            bool enable_dc
        )   : m_SlaveName(slave_name), ENABLE_DC(enable_dc)
        {   
            if(logger != nullptr)
            {
                m_Logger = logger;
                LOGGING_ENABLED = true;
            }

            if(offset != nullptr)
            {
                m_SlaveOffsets = offset;
            }

            if(enable_dc)
            {
                m_DcInfo = utilities::getDcInfo(
                    config_file_path,
                    slave_name
                );
            }

            m_SlaveInfo = utilities::parse_config_file(config_file_path, slave_name);
            if(m_SlaveInfo.slaveType != SlaveType::Coupler)
            {
                m_SlaveSyncs = new ec_sync_info_t[m_SlaveInfo.slaveSyncInfo.numSyncManagers + 1];

                m_SlavePdoEntries = new ec_pdo_entry_info_t[m_SlaveInfo.pdoEntryInfo.indexes.size()];

                m_SlavePDOs = new ec_pdo_info_t[m_SlaveInfo.ioMappingInfo.RxPDO_Indexes.size() + m_SlaveInfo.ioMappingInfo.TxPDO_Indexes.size()];
            }
            
        }

        Slave::~Slave()
        {   
            delete m_SlaveOffsets;
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

            m_SlavePDOs = ethercat_interface::slave::createSlavePDOs(
                m_SlavePdoEntries,
                m_SlaveInfo.ioMappingInfo.RxPDO_Indexes,
                m_SlaveInfo.ioMappingInfo.TxPDO_Indexes
            );


            m_SlaveSyncs = ethercat_interface::slave::createSlaveSyncs(
                m_SlaveInfo.slaveSyncInfo.numSyncManagers,
                ethercat_interface::utilities::intToEcDirectionEnum(m_SlaveInfo.slaveSyncInfo.syncManagerDirections),
                m_SlaveInfo.slaveSyncInfo.numPDOs,
                m_SlaveInfo.slaveSyncInfo.pdoIndexDiff,
                m_SlavePDOs,
                ethercat_interface::utilities::intToEcWatchdogEnum(m_SlaveInfo.slaveSyncInfo.watchdogModes)
            );

            m_Logger->log(INFO, m_SlaveName, "Loaded slave configuration.");
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

            if(m_SlaveInfo.slaveType == SlaveType::Coupler)
            {
                return;
            }
        
            if(!slave_config_ptr)
            {
                m_Logger->log(ERROR, m_SlaveName, "Can't create EC slave config.");
            }

            if(ecrt_slave_config_pdos((*slave_config_ptr), EC_END, m_SlaveSyncs) != 0)
            {
                m_Logger->log(ERROR, m_SlaveName, "Can't specify PDO configuration for the slave.");
            }
            else
            {
                
                m_Logger->log(INFO, m_SlaveName, "PDOs configured.");
            }

            if(ENABLE_DC)
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

            m_Logger->log(INFO, m_SlaveName, "Successfully set up the slave.");
        }


        bool Slave::enableOperation()
        {
            m_Status = this->readFromSlave<uint16_t>("status_word");

            /*
                Switch on disabled -> Ready to switch on : Shutdown command.
                Ready to switch on -> Switched on: Switch on command.
                Switched on -> Operation enabled: Enable operation command. 
             */
 
            if(isStatusCorrect(m_Status, StatusType::OperationEnabled))
            {
                return true;
            }

            if (!isStatusCorrect(m_Status, StatusType::Fault))
            {
                if(isStatusCorrect(m_Status, StatusType::SwitchOnDisabled))
                {   
                    //std::cout << "Switch on disabled\n";
                    writeToSlave<uint16_t>("ctrl_word", getCommandValue(ControlCommand::Shutdown));
                    return false;
                }
                else if(isStatusCorrect(m_Status, StatusType::ReadyToSwitchOn))
                {
                    //std::cout << "Ready To switch on\n";
                    writeToSlave<uint16_t>("ctrl_word", getCommandValue(ControlCommand::SwitchOn));
                    return false;
                }
                else if (isStatusCorrect(m_Status, StatusType::SwitchedOn))
                {
                    //std::cout << "Switched on\n";
                    writeToSlave<uint16_t>("ctrl_word", getCommandValue(ControlCommand::EnableOperation));
                    return false;
                }
                
            }
            else
            {
                if(!isStatusCorrect(m_Status, StatusType::FaultResponseActive))
                    writeToSlave<uint16_t>("ctrl_word", getCommandValue(ControlCommand::ResetFault));
                return false;
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
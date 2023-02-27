
#include "slave_example.hpp"

EPOS4::EPOS4(const std::string& slave_name, ethercat_interface::utilities::SlaveInfo slave_info, ethercat_interface::Offset* offset)
    : ethercat_interface::slave::Slave(slave_name, slave_info, offset)
{
    std::cout << "Creating " << m_SlaveName << " Slave." << std::endl;
}

void EPOS4::configure_slave()
{
    m_SlavePdoEntryRegistries = ethercat_interface::slave::createDomainRegistries(
        m_SlaveInfo.alias,
        m_SlaveInfo.position,
        m_SlaveInfo.vendorID,
        m_SlaveInfo.productCode,
        m_SlaveInfo.pdoEntryInfo.indexes,
        m_SlaveInfo.pdoEntryInfo.subindexes,
        m_SlaveOffsets
    );
    
    
    m_SlavePdoEntries = ethercat_interface::slave::createSlavePdoEntries(
        m_SlaveInfo.pdoEntryInfo.indexes,
        m_SlaveInfo.pdoEntryInfo.subindexes,
        m_SlaveInfo.pdoEntryInfo.bitLengths
    );

    m_SlavePDOs = ethercat_interface::slave::createSlavePDOs(
        m_SlavePdoEntries,
        m_SlaveInfo.ioMappingInfo.RxPDO_Address,
        m_SlaveInfo.ioMappingInfo.RxPDO_Size,
        m_SlaveInfo.ioMappingInfo.TxPDO_Address,
        m_SlaveInfo.ioMappingInfo.TxPDO_Size
    );

    m_SlaveSyncs = ethercat_interface::slave::createSlaveSyncs(
        m_SlaveInfo.slaveSyncInfo.numSyncManagers,
        ethercat_interface::utilities::intToEcDirectionEnum(m_SlaveInfo.slaveSyncInfo.syncManagerDirections),
        m_SlaveInfo.slaveSyncInfo.numPDOs,
        m_SlaveInfo.slaveSyncInfo.pdoIndexDiff,
        m_SlavePDOs,
        ethercat_interface::utilities::intToEcWatchdogEnum(m_SlaveInfo.slaveSyncInfo.watchdogModes)
    );
    
    std::cout << "Configured slave " << m_SlaveName << std::endl;


}

ec_sync_info_t* EPOS4::configure_syncs()
{

}
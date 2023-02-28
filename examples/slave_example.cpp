
#include "slave_example.hpp"

EPOS4::EPOS4(const std::string& slave_name, ethercat_interface::utilities::SlaveInfo slave_info, ethercat_interface::Offset* offset)
    : ethercat_interface::slave::Slave(slave_name, slave_info, offset, true)
{
    std::cout << "Creating " << m_SlaveName << " Slave." << std::endl;
}

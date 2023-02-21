
#include "slave_example.hpp"

EPOS4::EPOS4(const std::string& slave_name, ethercat_interface::utilities::SlaveInfo slave_info, ethercat_interface::Offset* offset = nullptr)
    : ethercat_interface::slave::Slave(slave_name, slave_info, offset)
{
    std::cout << "Created EPOS4 Slave." << std::endl;
}

void EPOS4::configure_slave()
{
    std::cout << "Configuring slave " << this->m_SlaveName << std::endl;
}

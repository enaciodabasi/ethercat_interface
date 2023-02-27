
#include "controller_example.hpp"

Master::Master()
    : ethercat_interface::controller::Controller()
{

}

void Master::cyclic_task()
{

}

void Master::loadSlaves()
{

    offset = new ServoOffset();
    auto epos4Info = ethercat_interface::utilities::parse_config_file("/home/naci/Projects/igh_master/examples/epos4_config.yaml", "EPOS4_0");
    EPOS4_0 = std::make_unique<EPOS4>("epos4_0", epos4Info, offset);
    EPOS4_0->configure_slave();
    //m_Slaves.push_back(EPOS4_0);

    EPOS4_0->setupSlave(m_EthercatMaster, m_Domain);

}

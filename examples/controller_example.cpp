
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

    auto epos4Info = ethercat_interface::utilities::parse_config_file("/home/naci/Projects/igh_master/examples/epos4_config.yaml", "EPOS4_0");
    epos4Info.toString();
    EPOS4_0 = std::make_unique<EPOS4>("epos4_0", epos4Info, new ServoOffset());
    EPOS4_0->configure_slave();
    //m_Slaves.push_back(EPOS4_0);

}

/* void Master::loadSlaves(const std::vector<std::pair<std::string, std::string>>& slaveConfigFileNames)
{

} */
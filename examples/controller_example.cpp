
#include "controller_example.hpp"

Master::Master()
    : ethercat_interface::controller::Controller()
{

}

void Master::cyclic_task()
{

}

void Master::loadSlaves(const std::vector<std::pair<std::string, std::string>> &slaveConfigFileNames)
{

    auto epos4Info = ethercat_interface::utilities::parse_config_file("epos4_config", "EPOS4");
    EPOS4_0 = std::make_unique<EPOS4>("epos4_0", epos4Info, new ServoOffset());
    

}

/* void Master::loadSlaves(const std::vector<std::pair<std::string, std::string>>& slaveConfigFileNames)
{

} */
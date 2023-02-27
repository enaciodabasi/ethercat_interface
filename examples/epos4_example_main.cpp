#include "controller_example.hpp"

int main()
{
    Master m = Master();
    //std::vector<std::pair<std::string, std::string>> configs;
    //configs.emplace_back(std::make_pair("epos4_config.yaml", "EPOS4_0"));
    m.loadSlaves();
    //auto inf = ethercat_interface::utilities::parse_config_file("/home/naci/Projects/igh_master/examples/epos4_config.yaml", "EPOS4_0");
    //inf.toString();

    m.start();

}
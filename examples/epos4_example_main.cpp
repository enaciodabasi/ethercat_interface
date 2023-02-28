#include "controller_example.hpp"
#include "signal.h"
static int run1 = 1;

void signal_handler(int sig){run1 = 0;}

int main()
{
    Master m = Master();
    //std::vector<std::pair<std::string, std::string>> configs;
    //configs.emplace_back(std::make_pair("epos4_config.yaml", "EPOS4_0"));
    m.loadSlaves();
    //auto inf = ethercat_interface::utilities::parse_config_file("/home/naci/Projects/igh_master/examples/epos4_config.yaml", "EPOS4_0");
    //inf.toString();

    m.start();
    m.exchangeDomainPD();

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    run1 = 1;
    while(run1)
    {   
        usleep(100000/100);
        m.cyclic_task();
    }
    usleep(500000);
    
    //system("ethercat download -t int32 -p 0 0x60ff 00 00");

	printf("\n\n*********************************\n\n");

	printf("The motor has stopped!\n\n");

	printf("*********************************\n\n");

	return EXIT_SUCCESS;
	return 0;
}
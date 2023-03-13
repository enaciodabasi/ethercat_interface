#include "epos4.hpp"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/version.h>
#include <linux/module.h>
#include <limits.h>
#include "ecrt.h"
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <termio.h>

epos4_controller::epos4_controller()
{
    master = new ethercat_interface::master::Master(
        0
    );
    
    dom = new ethercat_interface::domain::Domain("domain_0");
    master->registerDomain(dom);
    epos4 = new ethercat_interface::slave::Slave(
        "epos4_0",
        "/home/naci/Projects/igh_master/examples/EPOS4/epos4_config.yaml",
        new ServoOffset(),
        true
    );

    dom->registerSlave(epos4);
    master->configureDomains();
    master->setupDomains();
    if(!master->activateMaster())
        std::cout << "cant activate master\n";


}

void epos4_controller::cyclic_task()
{
    static uint16_t command = 0x0000;
	static uint16_t command_old = 0x000;
	static uint16_t status_old= 0x0000;
	uint16_t ctrl1;
	uint16_t status;
	int8_t opmode;
	int32_t current_velocity;
	static int counter3 = 0;
	static bool flip;
    master->receive("domain_0");
    master->updateMasterState();
    master->updateDomainStates();
    master->updateSlaveStates();



    bool enabled = epos4->enableOperation();

    dom->write<int8_t>("epos4_0", "operation_mode", 9, 0);

    //
    if(enabled)
    {   
        //std::cout << "Operation enabled\n";
        dom->write<uint16_t>("epos4_0", "ctrl_word", 0x000f, 0);
        dom->write<int32_t>("epos4_0", "target_velocity", 1500, 0);
    }

    master->send("domain_0");

}

int main(int argc, char** argv)
{
    epos4_controller epos4 = epos4_controller();

    while(true)
    {   
        usleep(100000/100);
        
        epos4.cyclic_task();
    }

    return 0;
}
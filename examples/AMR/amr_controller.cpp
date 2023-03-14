#include "amr_controller.hpp"
#include "offset_el7221_9014.hpp"

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
#include <pthread.h>
#include <fcntl.h>
#include <linux/joystick.h>

#define FREQUENCY 500
#define CLOCK_TO_USE CLOCK_MONOTONIC

#define NSEC_PER_SEC (1000000000L)
#define PERIOD_NS (NSEC_PER_SEC / FREQUENCY)
const struct timespec cycletime = {0, PERIOD_NS};
#define TIMESPEC2NS(T) ((uint64_t) (T).tv_sec * NSEC_PER_SEC + (T).tv_nsec)

int run_cyclic = 1;

void signal_handler(int isg)
{
    run_cyclic = 0;
}

AMR_Controller::AMR_Controller()
{
    ek1100 = new slave::Slave(
        "EK1100_0",
        "/home/naci/Projects/igh_master/examples/AMR/amr_config.yaml",
        nullptr,
        true,
        false
    );
    
    el7221_9014_0 = new slave::Slave(
        "EL7221_9014_0",
        "/home/naci/Projects/igh_master/examples/AMR/amr_config.yaml",
        new EL7221_9014_Offset(),
        true,
        true
    );
    el7221_9014_1 = new slave::Slave(
        "EL7221_9014_1",
        "/home/naci/Projects/igh_master/examples/AMR/amr_config.yaml",
        new EL7221_9014_Offset(),
        true,
        true
    );
    m = new master::Master(
        0
    );

    dom = new domain::Domain(
        "dc_domain"
    );
    m->registerDomain(dom);

    
    dom->registerSlave(ek1100);
    dom->registerSlave(el7221_9014_0);
    dom->registerSlave(el7221_9014_1);

    m->configureDomains();
    m->setupDomains();
    if(!m->activateMaster())
    {
        std::cout << "Can't activate master" << std::endl;
    }

}

struct timespec timespec_add(struct timespec time1, struct timespec time2)
{
    struct timespec result;

    if ((time1.tv_nsec + time2.tv_nsec) >= NSEC_PER_SEC) {
        result.tv_sec = time1.tv_sec + time2.tv_sec + 1;
        result.tv_nsec = time1.tv_nsec + time2.tv_nsec - NSEC_PER_SEC;
    } else {
        result.tv_sec = time1.tv_sec + time2.tv_sec;
        result.tv_nsec = time1.tv_nsec + time2.tv_nsec;
    }

    return result;
}


void AMR_Controller::cyclic_task()
{

    struct timespec wakeupTime, time;
    clock_gettime(CLOCK_TO_USE, &wakeupTime);
    while(run_cyclic)
    {
        wakeupTime = timespec_add(wakeupTime, cycletime);
        clock_nanosleep(CLOCK_TO_USE, TIMER_ABSTIME, &wakeupTime, NULL);
        ecrt_master_application_time(m->getEthercatMasterPtr(), TIMESPEC2NS(wakeupTime));
        m->receive("dc_domain");

        m->updateMasterState();
        m->updateDomainStates();
        m->updateSlaveStates();

        m->write<int8_t>(
            "dc_domain",
            "EL7221_9014_0",
            "op_mode",
            0x09
        );
        m->write<int8_t>(
            "dc_domain",
            "EL7221_9014_1",
            "op_mode",
            0x09
        );

        bool el0_enabled = el7221_9014_0->enableOperation();
        
        bool el1_enabled = el7221_9014_1->enableOperation();
        if(el0_enabled && el1_enabled)
        {
            m->write<int32_t>(
                "dc_domain",
                "EL7221_9014_0",
                "target_velocity",
                300000
            );
            m->write<int32_t>(
                "dc_domain",
                "EL7221_9014_1",
                "target_velocity",
                300000
            );
        }
        ecrt_master_sync_reference_clock_to(m->getEthercatMasterPtr(), TIMESPEC2NS(time));
        ecrt_master_sync_slave_clocks(
            m->getEthercatMasterPtr()
        );

        m->send("dc_domain");
    }
}

int main()
{
    AMR_Controller* amr_controller = new AMR_Controller();

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

   amr_controller->cyclic_task();

    return 0;
}
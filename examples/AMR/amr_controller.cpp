#include "amr_controller.hpp"
#include "offset_el7221_9014.hpp"
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

int main()
{
    AMR_Controller* amr_controller = new AMR_Controller();


    return 0;
}
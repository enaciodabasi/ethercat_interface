
#ifndef CONTROLLER_EXAMPLE_HPP
#define CONTROLLER_EXAMPLE_HPP

#include "../include/controller.hpp"
#include "../include/state.hpp"
#include "offset_example.hpp"
#include "slave_example.hpp"

class Master : public ethercat_interface::controller::Controller
{
    public:

    Master();
    ~Master(){EPOS4_0->writeToSlave<int32_t>("target_velocity",0);
	ecrt_domain_queue(m_Domain);
	ecrt_master_send(m_EthercatMaster);};

    void cyclic_task() override;

    void loadSlaves() override;
    int32_t target_velocity = 500;
    int counter1 = 0;
    double counter2 = 0.0;
    void exchangeDomainPD()
    {
        EPOS4_0->setDomainProcessDataPtr(m_DomainProcessData);
    }
    
    private:

    std::unique_ptr<ethercat_interface::slave::Slave> EPOS4_0;

    ethercat_interface::Offset* offset; 

    
};

#endif
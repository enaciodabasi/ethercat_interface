#ifndef EPOS4_HPP
#define EPOS4_HPP

#include "../../include/slave.hpp"
#include "../../include/domain.hpp"
#include "../../include/master.hpp"
#include "offset_example.hpp"

class epos4_controller
{

    public:

    epos4_controller();
    ~epos4_controller()
    {
        delete master;

    }

    ethercat_interface::master::Master* master;

    ethercat_interface::domain::Domain* dom;

    ethercat_interface::slave::Slave* epos4;


    void cyclic_task();


};

#endif
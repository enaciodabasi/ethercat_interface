
#ifndef CONTROLLER_EXAMPLE_HPP
#define CONTROLLER_EXAMPLE_HPP

#include "../include/controller.hpp"
#include "offset_example.hpp"
#include "slave_example.hpp"

class Master : public ethercat_interface::controller::Controller
{
    public:

    Master();
    ~Master(){};

    void cyclic_task() override;

    void loadSlaves() override;

    private:

    std::unique_ptr<ethercat_interface::slave::Slave> EPOS4_0;

    ethercat_interface::Offset* offset; 

};

#endif
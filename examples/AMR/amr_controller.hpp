#ifndef AMR_CONTROLLER_HPP
#define AMR_CONTROLLER_HPP

#include "../../include/slave.hpp"
#include "../../include/master.hpp"
#include "../../include/domain.hpp"

using namespace ethercat_interface;

class AMR_Controller
{
    public:

    AMR_Controller();
    ~AMR_Controller();

    master::Master* m;
    domain::Domain* dom;

    slave::Slave* ek1100;
    slave::Slave* el7221_9014_0;
    slave::Slave* el7221_9014_1;


    private:


};


#endif //AMR_CONTROLLER_HPP
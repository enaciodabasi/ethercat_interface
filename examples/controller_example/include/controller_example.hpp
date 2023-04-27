#ifndef CONTROLLER_EXAMPLE_HPP
#define CONTROLLER_EXAMPLE_HPP

#include "ethercat_interface/controller.hpp"

class ControllerExample : public ethercat_interface::controller::Controller
{

    public:

    ControllerExample(const std::string& config_file_path);

    ~ControllerExample(){
        this->joinThread();
    };

    void startTask() override;

    private:

    void cyclicTask() override;


};


#endif // CONTROLLER_EXAMPLE_HPP
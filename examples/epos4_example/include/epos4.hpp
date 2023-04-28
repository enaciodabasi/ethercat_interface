#ifndef EPOS4_HPP
#define EPOS4_HPP


#include "ethercat_interface/controller.hpp"

class EPOS4_Controller : public ethercat_interface::controller::Controller
{
    public:

    EPOS4_Controller(const std::string& config_file_path);

    ~EPOS4_Controller()
    {
        this->joinThread();
    }

    void startTask() override;

    private:

    void cyclicTask() override;
    
};

#endif // EPOS4_HPP

#ifndef NODE_HPP_
#define NODE_HPP_


#include "ethercat_interface/controller.hpp"

class SomanetNode : public ethercat_interface::controller::Controller
{
    public:

    SomanetNode(const std::string& config_file_path);

    ~SomanetNode(){
        this->joinThread();
    };

    void startTask() override;

    private:

    void cyclicTask() override;
};

#endif // NODE_HPP_
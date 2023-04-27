#include "../include/controller_example.hpp"

ControllerExample::ControllerExample(const std::string& config_file_path)
    : Controller(config_file_path)
{

}

void ControllerExample::cyclicTask()
{   
    
}

void ControllerExample::startTask()
{
    this->m_CyclicTaskThread = std::thread(
        &ControllerExample::cyclicTask,
        this
    );
    this->updateThreadInfo();
}

int main()
{
    std::unique_ptr<ControllerExample> cont = std::make_unique<ControllerExample>("/home/naci/Projects/igh_master/examples/controller_example/config.yaml");
    
    if(!cont->setup())
        return -1;

    cont->startTask();
     

    return 0;
}
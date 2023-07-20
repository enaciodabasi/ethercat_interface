/**
 * @file hamal_test.cpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-20
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "include/hamal/hamal_test.hpp"

HamalController::HamalController(const std::string& config_file_path) : Controller(config_file_path) {}

HamalController::~HamalController()
{
    this->joinThread();
}

void HamalController::startTask()
{
    this->m_CyclicTaskThread = std::thread(
        &HamalController::cyclicTask,
        this
    );
    this->updateThreadInfo();
}

void HamalController::cyclicTask()
{

}

int main(int argc, char** argv)
{
    std::unique_ptr<HamalController> controller = std::make_unique<HamalController>(
        "/home/naci/Projects/igh_master/examples/hamal/config/hamal_config.yaml"
    );

    if(!controller->setup())
    {
        std::cout << "Failed during EtherCAT controller setup." << std::endl;
        return -1;
    }

    controller->startTask();

    return 0;
}
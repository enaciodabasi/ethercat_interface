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

#include "../include/hamal/hamal_test.hpp"
bool run = true;
bool dc = true;

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

    if(dc)
    clock_gettime(m_DcHelper.clock, &m_DcHelper.wakeupTime);

    while(run)
    {
        if(dc){
        this->setTaskWakeUpTime();
        sleep_task(
            m_DcHelper.clock,
            TIMER_ABSTIME,
            &m_DcHelper.wakeupTime,
            NULL
        );
        

        m_Master->setMasterTime(timespecToNanoSec(m_DcHelper.wakeupTime));
        }
        m_Master->receive("domain_0");

        m_Master->updateDomainStates();
        m_Master->updateMasterState();
        m_Master->updateSlaveStates();

        bool slavesEnabled = m_Master->enableSlaves();

        this->m_Master->write<int8_t>("domain_0", "somanet_node_0", "op_mode", 0x09);
        this->m_Master->write<int8_t>("domain_0", "somanet_node_1", "op_mode", 0x09);


        if(dc){
        clock_gettime(m_DcHelper.clock, &m_DcHelper.currentTime);
        m_Master->syncMasterClock(timespecToNanoSec(m_DcHelper.currentTime), m_DcHelper);
        }
        m_Master->send("domain_0");

    }

    m_Master->shutdownSlaves();
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
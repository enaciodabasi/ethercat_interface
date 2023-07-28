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

        /* this->m_Master->write<int8_t>("domain_0", "somanet_node_0", "op_mode", 0x09);
        this->m_Master->write<int8_t>("domain_0", "somanet_node_1", "op_mode", 0x09); */

        const auto plcInput = m_Master->readArray<int8_t, 10>("domain_0", "safety_plc", "input_data");
        const auto diag = m_Master->read<uint8_t>("domain_0", "safety_plc", "diag");
        std::vector<uint8_t> o = {55, 2, 33,44, 58, 23, 34, 24, 22, 21};
        m_Master->writeArray<uint8_t>("domain_0", "safety_plc", "output_data", o);
        if(diag)
        std::cout << "Diag: " << (int16_t)diag.value() << std::endl;
        if(plcInput)
        {
            int i = 0;
            for(const auto input : plcInput.value())
            {
                std::cout << "PLC Input " << i << ": " << (uint16_t)input << std::endl;
                i+=1;
            }
        }

        /* if(slavesEnabled)
        {
            const auto status0_o = m_Master->read<uint16_t>("domain_0", "somanet_node_0", "status_word");
            const auto status1_o = m_Master->read<uint16_t>("domain_0", "somanet_node_1", "status_word");

            const auto actPos0_o = m_Master->read<int32_t>("domain_0", "somanet_node_0", "actual_position");
            const auto actPos1_o = m_Master->read<int32_t>("domain_0", "somanet_node_1", "actual_position");

            const auto actVel0_o = m_Master->read<int32_t>("domain_0", "somanet_node_0", "actual_velocity");
            const auto actVel1_o = m_Master->read<int32_t>("domain_0", "somanet_node_1", "actual_velocity");

            const auto errorCode0_o = m_Master->read<uint16_t>("domain_0", "somanet_node_0", "error_code");
            const auto errorCode1_o = m_Master->read<uint16_t>("domain_0", "somanet_node_1", "error_code");
        } */



        /* if(slavesEnabled)
        {
            double angularTarget = 0.2;
            int32_t targetVel = angularTarget * angularToRPM_Constant;
            m_Master->write<int32_t>("domain_0", "somanet_node_0", "target_velocity", targetVel); 
            m_Master->write<int32_t>("domain_0", "somanet_node_1", "target_velocity", targetVel);
        } */

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
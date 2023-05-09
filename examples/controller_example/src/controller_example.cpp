#include "../include/controller_example.hpp"

bool run = true; 

void shutdown_ec_loop(int sig)
{
    run = false;
}

ControllerExample::ControllerExample(const std::string& config_file_path)
    : Controller(config_file_path)
{

}

void ControllerExample::cyclicTask()
{   
    clock_gettime(m_DcHelper.clock, &m_DcHelper.wakeupTime);
    while(run)
    {
        this->setTaskWakeUpTime();
        sleep_task(
            m_DcHelper.clock,
            TIMER_ABSTIME,
            &m_DcHelper.wakeupTime,
            NULL
        );

        m_Master->setMasterTime(timespecToNanoSec(m_DcHelper.wakeupTime));

        m_Master->receive("domain_1");

        m_Master->updateDomainStates();
        m_Master->updateMasterState();
        m_Master->updateSlaveStates();
        /*
            ********************************
            ********** Loop Logic **********
            ********************************
        */  

        bool slavesEnabled = m_Master->enableSlaves();


        this->m_Master->write<int8_t>(
                "domain_1",
                "EL7221_9015_0",
                "op_mode",
                0x09
        );
        this->m_Master->write<int8_t>(
            "domain_1",
            "EL7221_9015_1",
            "op_mode",
            0x09
        );

        
        
        if(slavesEnabled)
        {
                auto leftWheelPosOpt = m_Master->read<int32_t>("domain_1", "EL7221_9015_0", "current_position");
                auto rightWheelPosOpt = m_Master->read<int32_t>("domain_1", "EL7221_9015_1", "current_position");

                if(leftWheelPosOpt != std::nullopt || rightWheelPosOpt != std::nullopt)
                {
                    std::string posOutMsg = "Left Wheel Position: " + std::to_string(leftWheelPosOpt.value()) + " Right Wheel Position: " + std::to_string(rightWheelPosOpt.value());
                    std::cout << posOutMsg << std::endl;
                }

        }


       /*
            ********************************
            ********** End of Loop Logic **********
            ********************************
       */

        clock_gettime(m_DcHelper.clock, &m_DcHelper.currentTime);
        m_Master->syncMasterClock(timespecToNanoSec(m_DcHelper.currentTime), m_DcHelper);
        m_Master->send("domain_1");


    }
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
    signal(SIGTERM, shutdown_ec_loop);
    signal(SIGINT, shutdown_ec_loop);
    std::unique_ptr<ControllerExample> cont = std::make_unique<ControllerExample>("/home/naci/Projects/igh_master/examples/controller_example/config.yaml");
    
    if(!cont->setup())
        return -1;

    cont->startTask();
     

    return 0;
}
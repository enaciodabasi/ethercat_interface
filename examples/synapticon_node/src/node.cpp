#include "../include/node.hpp"
#include <math.h>

bool run = true; 

void shutdown_ec_loop(int sig)
{
    run = false;
}

SomanetNode::SomanetNode(const std::string& config_file_path)
    : Controller(config_file_path)
{

}

void SomanetNode::cyclicTask()
{   
    clock_gettime(m_DcHelper.clock, &m_DcHelper.wakeupTime);
    int i = 0;
    int32_t pos  = 0;
    bool homingDone = false;
    while(run)
    {
        /* std::cout << "---------------------------------------" << std::endl; */
        this->setTaskWakeUpTime();
        sleep_task(
            m_DcHelper.clock,
            TIMER_ABSTIME,
            &m_DcHelper.wakeupTime,
            NULL
        );

        m_Master->setMasterTime(timespecToNanoSec(m_DcHelper.wakeupTime));

        m_Master->receive("domain_0");

        m_Master->updateDomainStates();
        m_Master->updateMasterState();
        m_Master->updateSlaveStates();
        /*
            ********************************
            ********** Loop Logic **********
            ********************************
        */  
        bool slavesEnabled = m_Master->enableSlaves();

        /* const auto currentOpMode = this->m_Master->read<int8_t>("domain_0", "somanet_node", "op_mode_display");
        const auto statusWord = this->m_Master->read<uint16_t>("domain_0", "somanet_node", "status_word");
        const auto error = this->m_Master->read<uint16_t>("domain_0", "somanet_node", "error_code");
        std::cout << "Op Mode: " << (uint16_t)currentOpMode.value() << std::endl;
        std::cout << "Status Word: " << statusWord.value() << std::endl;
        std::cout << "Error Code: " << error.value() << std::endl; */
            
        this->m_Master->write<int8_t>(
            "domain_0",
            "somanet_node",
            "op_mode",
            0x09
        );

        if(slavesEnabled)
        {

            auto status = m_Master->read<uint16_t>("domain_0", "somanet_node", "status_word");
            if(status)
            {
                std::cout << "Current Status Word: " << status.value() << std::endl;
            }

            auto leftWheelPosOpt = m_Master->read<int32_t>("domain_0", "somanet_node", "actual_position");
            auto leftWheelVelOpt = m_Master->read<int32_t>("domain_0", "somanet_node", "actual_velocity");
            if(leftWheelPosOpt != std::nullopt && leftWheelVelOpt)
            {
                double currentPosition = (leftWheelPosOpt.value() / 20480.0) * (2.0 * M_PI) / 24.685;
                std::string posOutMsg = "Current Position: " + std::to_string(leftWheelPosOpt.value());
                std::cout << posOutMsg << std::endl;
                
                pos = leftWheelPosOpt.value();

                double currentVel = (leftWheelVelOpt.value() * 2) / (60 * M_PI * 24.685);
                std::cout << " Current Velocity: " + std::to_string(currentVel); 
                
            }

            auto lifterVel = m_Master->read<int32_t>("domain_0", "somanet_node", "actual_velocity");
            if(lifterVel != std::nullopt)
            {
                auto motorvel = lifterVel.value();
                const auto wheelVel = (double)motorvel;
                //std::string velOutMsg = "Current velocity: " + std::to_string(wheelVel);
                //std::cout << velOutMsg << std::endl;
            }

            auto error_code = m_Master->read<uint16_t>("domain_0", "somanet_node", "error_code");
            if(error_code != std::nullopt)
            {
                std:: cout << std::hex << "Error Code: " << error_code.value() << std::endl;
            }

            /* auto following_error = m_Master->read<int32_t>("domain_0", "somanet_node", "following_error");
            if(following_error != std::nullopt)
            {
                std::cout << std::dec << "Following Error: " << following_error.value() << std::endl;
            } */

        }

        // Write
        if(slavesEnabled)
        {   
            double angularTarget = 0.1;
            int32_t targetVel = ((60 * angularTarget) / 2 * M_PI) * 24.685; 
            m_Master->write<int32_t>("domain_0", "somanet_node", "target_velocity", targetVel);
            /* std::cout << std::to_string(pos+2000) << std::endl; */
            std::cout << "Target Velocity: " << std::to_string(angularTarget) << std::endl;
        }

       /*
            ********************************
            ********** End of Loop Logic **********
            ********************************
       */

        clock_gettime(m_DcHelper.clock, &m_DcHelper.currentTime);
        m_Master->syncMasterClock(timespecToNanoSec(m_DcHelper.currentTime), m_DcHelper);
        m_Master->send("domain_0");
        i += 1;
        /* std::cout << "---------------------------------------" << std::endl; */
    }
}

void SomanetNode::startTask()
{
    this->m_CyclicTaskThread = std::thread(
        &SomanetNode::cyclicTask,
        this
    );
    this->updateThreadInfo();
}

int main()
{
    signal(SIGTERM, shutdown_ec_loop);
    signal(SIGINT, shutdown_ec_loop);
    std::unique_ptr<SomanetNode> cont = std::make_unique<SomanetNode>("/home/naci/Projects/igh_master/examples/synapticon_node/node_config.yaml");
    
    if(!cont->setup())
        return -1;

    cont->startTask();
     

    return 0;
}
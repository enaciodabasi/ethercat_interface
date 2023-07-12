#include "../include/node.hpp"

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


        
        this->m_Master->write<int8_t>(
                "domain_0",
                "somanet_node",
                "op_mode",
                0x08
        );

        

        if(slavesEnabled)
        {

            auto status = m_Master->read<uint16_t>("domain_0", "somanet_node", "status_word");
            if(status)
            {
                std::cout << "Current Status Word: " << status.value() << std::endl;
            }

            auto leftWheelPosOpt = m_Master->read<int32_t>("domain_0", "somanet_node", "actual_position");
            
            if(leftWheelPosOpt != std::nullopt)
            {
                std::string posOutMsg = "Current Position: " + std::to_string(leftWheelPosOpt.value());
                
                
                pos = leftWheelPosOpt.value();
                std::cout << posOutMsg << " Target: " << pos + 500 << std::endl;
            }

            auto lifterVel = m_Master->read<int32_t>("domain_0", "somanet_node", "actual_velocity");
            if(lifterVel != std::nullopt)
            {
                std::string velOutMsg = "Current velocity" + std::to_string(lifterVel.value());
                std::cout << velOutMsg << std::endl;
            }

            auto error_code = m_Master->read<uint16_t>("domain_0", "somanet_node", "error_code");
            if(error_code != std::nullopt)
            {
                std:: cout << std::hex << "Error Code: " << error_code.value() << std::endl;
            }

            auto following_error = m_Master->read<int32_t>("domain_0", "somanet_node", "following_error");
            if(following_error != std::nullopt)
            {
                std::cout << std::dec << "Following Error: " << following_error.value() << std::endl;
            }

        }

        // Write
        if(slavesEnabled)
        {   
             m_Master->write<int32_t>("domain_0", "somanet_node", "target_position", pos+500);
            /* std::cout << std::to_string(pos+2000) << std::endl; */
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
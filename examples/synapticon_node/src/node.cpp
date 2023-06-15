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
                0x09
        );

        
        
        if(slavesEnabled)
        {
            std::cout << "Slaves enabled\n";
            auto leftWheelPosOpt = m_Master->read<int32_t>("domain_0", "somanet_node", "actual_position");
            
            if(leftWheelPosOpt != std::nullopt)
            {
                std::string posOutMsg = "Left Wheel Position: " + std::to_string(leftWheelPosOpt.value());
                std::cout << posOutMsg << std::endl;
            }

        }

        // Write
        if(slavesEnabled)
        {
            m_Master->write<int32_t>("domain_0", "somanet_node", "target_velocity", 100);
        }

       /*
            ********************************
            ********** End of Loop Logic **********
            ********************************
       */

        clock_gettime(m_DcHelper.clock, &m_DcHelper.currentTime);
        m_Master->syncMasterClock(timespecToNanoSec(m_DcHelper.currentTime), m_DcHelper);
        m_Master->send("domain_0");


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
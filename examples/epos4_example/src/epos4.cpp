#include "../include/epos4.hpp"

bool run = true;

void shutdown_ec_loop(int sig)
{
    run = false;
}

EPOS4_Controller::EPOS4_Controller(const std::string& config_file_path)
    : ethercat_interface::controller::Controller(config_file_path){}

void EPOS4_Controller::cyclicTask()
{
    while(run)
    {
        m_Master->receive("domain_1");

        bool slavesEnabled = m_Master->enableSlaves();

        //m_Master->write<int8_t>("domain_1", "epos4", "op_mode", 0x09);

        

        if(slavesEnabled)
        {

        }

        m_Master->send("domain_1");
    }
}

void EPOS4_Controller::startTask()
{
    this->m_CyclicTaskThread = std::thread(
        &EPOS4_Controller::cyclicTask,
        this
    );
    this->updateThreadInfo();
}


int main()
{
    signal(SIGTERM, shutdown_ec_loop);
    signal(SIGINT, shutdown_ec_loop);
    std::unique_ptr<EPOS4_Controller> cont = std::make_unique<EPOS4_Controller>("/home/naci/Projects/igh_master/examples/epos4_example/epos4_config.yaml");
    
    if(!cont->setup())
        return -1;

    cont->startTask();
     

    return 0;
}
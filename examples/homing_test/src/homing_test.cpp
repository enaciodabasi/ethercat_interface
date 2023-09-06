#include "../include/homing_test/homing_test.hpp"

bool run = true;

void shutdown_ec_loop(int sig)
{
    run = false;
}

HomingTest::HomingTest(const std::string& config_file_path)
    : Controller(config_file_path)
{

}

void HomingTest::startTask()
{
    this->m_CyclicTaskThread = std::thread(
        &HomingTest::cyclicTask,
        this
    );

    this->updateThreadInfo();
}

void HomingTest::cyclicTask()
{
    clock_gettime(m_DcHelper.clock, &m_DcHelper.wakeupTime);
    int i = 0;
    int32_t pos  = 0;
    bool homingDone = false;
    bool homingSetup = false;
    bool homingInProgress = false;
    bool homingSuccessful = false;
    bool homingError = false;
    bool targetReached = false;
    bool slavesEnabled = false;
    uint16_t prevCtrlWord = 0x0;
    while(run)
    {
        setTaskWakeUpTime();

        sleep_task(m_DcHelper.clock, TIMER_ABSTIME, &m_DcHelper.wakeupTime, NULL);

        m_Master->setMasterTime(timespecToNanoSec(m_DcHelper.wakeupTime));

        m_Master->receive("domain_0");


        // If homing is not currently active, switch to Operation Enabled state.
        if(!homingInProgress)
            slavesEnabled = m_Master->enableSlaves();
    
        // If slaves are Operation enabled state and the homing parameters are not yet set
        // Set Mode of Operation, Homing Method, Speed and Acceleration.
        if(slavesEnabled && !homingSetup){
            std::cout << "Setting up the homing process\n";
            m_Master->write<int8_t>("domain_0", "somanet_node", "op_mode", 0x06);
            m_Master->write<int8_t>("domain_0", "somanet_node", "homing_method", 0x11);
            m_Master->write<uint32_t>("domain_0", "somanet_node", "homing_speed", 300);
            m_Master->write<uint32_t>("domain_0", "somanet_node", "homing_speed2", 300);
            m_Master->write<uint32_t>("domain_0", "somanet_node", "homing_accel", 600);
            homingSetup = true;
            // Set "Homing In Progress" flag to true to start homing.
            homingInProgress = true;
        }

        bool opModeIsCorrect = false; 
        const auto opModeDisplayOpt = m_Master->read<int8_t>("domain_0", "somanet_node", "op_mode_display");
        if(opModeDisplayOpt){
            //std::cout << (uint16_t)opModeDisplayOpt.value() << std::endl;
            if(opModeDisplayOpt.value() == 0x06){
                opModeIsCorrect = true;
            }
            else{
                opModeIsCorrect = false;
            }
        }

        if(!opModeIsCorrect){
            m_Master->write<int8_t>("domain_0", "somanet_node", "op_mode", 0x06);
        }
        
        const auto currentPosOpt = m_Master->read<int32_t>("domain_0", "somanet_node", "actual_position");
        if(currentPosOpt){
            int32_t currentPos = currentPosOpt.value();
            std::cout << "Current Position: " << currentPos << std::endl; 
        }

        if(homingInProgress && opModeIsCorrect){
            std::cout << "Homing in progress..." << std::endl;
            uint16_t ctrlWord = 0x0;
            auto ctrlWordOpt = m_Master->getControlWord("domain_0", "somanet_node");
            if(ctrlWordOpt){
                ctrlWord = ctrlWordOpt.value();
            }   

            auto statusWordOpt = m_Master->read<uint16_t>("domain_0", "somanet_node", "status_word");
            uint16_t statusWord = 0x0;
            if(statusWordOpt){
                statusWord = statusWordOpt.value();
            }

            if(isBitSet(statusWord, 13)){
                homingError = true;
            }

            if(isBitSet(statusWord, 12)){
                homingSuccessful = true;
            }   

            if(isBitSet(statusWord, 10)){
                targetReached = true;
            }

            // From the Synapticon instructions:
            // Set 8th bit to 0.
            if(isBitSet(ctrlWord, 8))
            {
                resetBitAtIndex(ctrlWord, 8);
            }
            // Set 4th bit to 1.
            if(!isBitSet(ctrlWord, 4))
                setBitAtIndex(ctrlWord, 4);

            if(prevCtrlWord != ctrlWord)            
                m_Master->write<uint16_t>("domain_0", "somanet_node", "ctrl_word", ctrlWord);

            prevCtrlWord = ctrlWord;
        }

        if(homingInProgress){
            if(!homingError && !homingSuccessful && !targetReached){
                std::cout << "Homing is performed" << std::endl;
                homingDone = false;
            }
            else if(!homingError && !homingSuccessful && targetReached){
                std::cout << "Homing is interrupted or not started" << std::endl;
                homingDone = false;
            }
            else if(!homingError && homingSuccessful && !targetReached){
                std::cout << "Homing confirmed, but target not yet reached" << std::endl;
                homingDone = false;
            }
            else if(!homingError && homingSuccessful && targetReached){
                std::cout << "Homing completed" << std::endl;
                homingDone = true;
            }
            else if(homingError && !homingSuccessful && !targetReached){
                std::cout << "Error detected, motor still running" << std::endl;
                homingDone = false;
            }
            else if(homingError && !homingSuccessful && targetReached){
                std::cout << "Error during homing, motor at standstill." << std::endl;
                homingDone = false;
            }
        }

        if(homingDone){
            setBitAtIndex(prevCtrlWord, 8);
            m_Master->write<uint16_t>("domain_0", "somanet_node", "ctrl_word", prevCtrlWord);
            std::cout << "Homing Done" << std::endl;
            homingInProgress = false;
            
            m_Master->write<int32_t>("domain_0", "somanet_node", "target_velocity", 0);
            
        }

        clock_gettime(m_DcHelper.clock, &m_DcHelper.currentTime);
        m_Master->syncMasterClock(timespecToNanoSec(m_DcHelper.currentTime), m_DcHelper);
        m_Master->send("domain_0");

    }
}

int main(int argc, char** argv)
{
    signal(SIGTERM, shutdown_ec_loop);
    signal(SIGINT, shutdown_ec_loop);

    std::unique_ptr<HomingTest> cont = std::make_unique<HomingTest>("/home/naci/Projects/igh_master/examples/homing_test/homing_test.yaml");

    if(!cont->setup())
    {
        return -1;
    }

    cont->startTask();

    return 0;
}
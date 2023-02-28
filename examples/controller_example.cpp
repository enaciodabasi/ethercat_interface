
#include "controller_example.hpp"
#include <math.h>

Master::Master()
    : ethercat_interface::controller::Controller(true)
{

}

void Master::cyclic_task()
{

    static uint16_t command = 0x0000;
	static uint16_t command_old = 0x000;
	static uint16_t status_old= 0x0000;
	uint16_t ctrl1;
	uint16_t status;
	int8_t opmode;
	int32_t current_velocity;
	static int counter3 = 0;
	static bool flip;

    ecrt_master_receive(m_EthercatMaster);
    ecrt_domain_process(m_Domain);

    //std::cout << "Received master and domain" << std::endl;

    updateDomainState();
    updateMasterState();
    EPOS4_0->updateSlaveState();

    //std::cout << "STATES UPDATED\n";

    /* status = EPOS4_0->readFromSlave<uint16_t>("status_word");
    std::cout << status << std::endl;
    opmode = EPOS4_0->readFromSlave<uint8_t>("operation_mode");
    current_velocity = EPOS4_0->readFromSlave<int32_t>("current_velocity");
    ctrl1 = EPOS4_0->readFromSlave<uint16_t>("ctrl_word");
    EPOS4_0->writeToSlave("operation_mode", 9); */
    //std::cout << "Written initial values" << std::endl;

    
    /* 
    if (EPOS4_0->getCurrentSlaveState().operational && (EPOS4_0->getCurrentSlaveState().al_state == 0X08) && (m_CurrentDomainState.working_counter > 2))
    {
        status = EPOS4_0->readFromSlave<uint16_t>("status_word");

        if(command == 0x0000 && !(status & 0x0008))
        {
            if(status & 0x0007)
            {
                EPOS4_0->writeToSlave<uint16_t>("ctrl_word", 0x000f);
                command = 0x0027;
            }
            else
            {
                EPOS4_0->writeToSlave<uint16_t>("ctrl_word", 0x0006);
                command = 0x0006;
            }
        }
        else if (command == 0x0006 && (status & 0x0001) )
	    {
	    	EPOS4_0->writeToSlave<uint16_t>("ctrl_word", 0x0007);
	    	command = 0x000f;
	    }
    
	    else if (command == 0x000f && (status & 0x0003) )
	    {
	    	EPOS4_0->writeToSlave<uint16_t>("ctrl_word", 0x000f);
	    	command = 0x0027;
    
	    }
    
	    else if ( command == 0x0027 && (status & 0x0007) )
	    {
	    	EPOS4_0->writeToSlave<uint16_t>("ctrl_word", 0x000f);
	    	EPOS4_0->writeToSlave<int32_t>("target_velocity" ,target_velocity);
	    	command = 0x0027;
	    	counter1 = counter1 + 5;
	    // Control the motor as a sinusoidal wave which increases and decreases the speed between -3000, +3000 rpm
    
	    	if(counter1 % 100 == 0)
	    	{
	    		target_velocity = std::sin(counter2*M_PI/180.0)*2000.0/(std::sin(M_PI/2.0));
	    		counter2++;
    
	    	} 
	    }
    }
    else
    {
        if(status & 0x0008)
        {
			if (flip)
				EPOS4_0->writeToSlave<uint16_t>("ctrl_word", 0x0080);
			else
			{
				EPOS4_0->writeToSlave<uint16_t>("ctrl_word", 0x0006);
				command=0x0000;
			}
		}
        if(counter3 % 50 == 0)
        {
            flip = !flip;
        } 
        counter3++;
    } */

    ecrt_domain_queue(m_Domain);
    ecrt_master_send(m_EthercatMaster);
    
}

void Master::loadSlaves()
{

    offset = new ServoOffset();
    auto epos4Info = ethercat_interface::utilities::parse_config_file("/home/naci/Projects/igh_master/examples/epos4_config.yaml", "EPOS4_0");
    EPOS4_0 = std::make_unique<EPOS4>("epos4_0", epos4Info, offset);
    EPOS4_0->configure_slave();
    //m_Slaves.push_back(EPOS4_0);

    EPOS4_0->setupSlave(m_EthercatMaster, m_Domain);

}

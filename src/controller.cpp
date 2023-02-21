/**
 * @file controller.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "../include/controller.hpp"


namespace ethercat_interface
{
    namespace controller
    {
        Controller::Controller()
        {
            m_EthercatMaster = std::make_unique<ec_master_t>(ecrt_request_master(0));

            // If call to receive master fails e.g the ec_master_t pointer is null, throw a MasterRequestException
            if(!m_EthercatMaster.get()) 
            {
                //throw exception::MasterRequestException();
            }

            m_Domain = std::make_unique<ec_domain_t>(ecrt_master_create_domain(m_EthercatMaster.get()));

            if(!m_Domain.get())
            {

            }
        }

        Controller::~Controller()
        {
            
        }

        void Controller::configureSlaves()
        {
            for(std::size_t i = 0; i < m_Slaves.size(); i++)
            {
                m_Slaves[i]->configure_slave();
            }
        }
        
    }
}
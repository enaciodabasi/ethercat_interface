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
           /*  m_EthercatMaster = std::make_unique<ec_master_t>(ecrt_request_master(0));

            // If call to receive master fails e.g the ec_master_t pointer is null, throw a MasterRequestException
            if(!m_EthercatMaster.get()) 
            {
                //throw exception::MasterRequestException();
            }

            m_Domain = std::make_unique<ec_domain_t>(ecrt_master_create_domain(m_EthercatMaster.get()));

            if(!m_Domain.get())
            {

            } */

            //initSlaves();

            m_EthercatMaster = ecrt_request_master(0);

            if(!m_EthercatMaster)
            {
                std::cout << "Can't get EtherCAT Master." << std::endl;
            }
            else
            {
                std::cout << "Succesfully requested EtherCAT Master." << std::endl;
            }

            m_Domain = ecrt_master_create_domain(m_EthercatMaster);

            if(!m_Domain)
            {
                std::cout << "Can't create domain." << std::endl; 
            }
            else
            {
                std::cout << "Succesfully created domain." << std::endl;
            }
            //m_DomainProcessData = new uint8_t();
        }

        Controller::~Controller()
        {
            ecrt_release_master(m_EthercatMaster);
            std::cout << "Destroying master" << std::endl;

            delete m_DomainProcessData;
        }

        void Controller::configureSlaves()
        {
            for(std::size_t i = 0; i < m_Slaves.size(); i++)
            {
                m_Slaves[i]->configure_slave();
            }
        }

        void Controller::start()
        {   

            std::cout << "Activating Master..." << std::endl;
            if(ecrt_master_activate(m_EthercatMaster))
            {
                std::cout << "Can't activate EtherCAT Master." << std::endl;
                exit(EXIT_FAILURE);
            }

            std::cout << "EtherCAT Master activated." << std::endl;

            //usleep(500000);
            
            m_DomainProcessData = ecrt_domain_data(m_Domain);
            
            if(!m_DomainProcessData)
            {
                std::cout << "Can't retrieve process data for the domain." << std::endl;
                exit(EXIT_FAILURE);
            }

            std::cout << "Successfully retrieved process data for the domain" << std::endl;
        }
        
    }
}
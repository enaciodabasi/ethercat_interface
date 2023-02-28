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
        Controller::Controller(bool enable_logging)
            : ENABLE_LOGGING(enable_logging)
        {
           
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
            
            m_CurrentMasterState = {};
            m_CurrentDomainState = {};

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
            
            //m_DomainProcessData = ecrt_domain_data(m_Domain);
            
            if(!(m_DomainProcessData = ecrt_domain_data(m_Domain)))
            {
                std::cout << "Can't retrieve process data for the domain." << std::endl;
                exit(EXIT_FAILURE);
            }

            

            std::cout << "Successfully retrieved process data for the domain" << std::endl;
        }

        void Controller::updateMasterState()
        {
            ec_master_state_t state;
            ecrt_master_state(m_EthercatMaster, &state);

            if(ENABLE_LOGGING)
            {
                if(state.slaves_responding != m_CurrentMasterState.slaves_responding)
                {
                    std::cout << state.slaves_responding << " slaves are responding." << std::endl;
                }

                if(state.link_up != m_CurrentMasterState.link_up)
                {
                    std::cout << "Link is " << (state.link_up ? "up" : "down") << std::endl;
                }

                if(state.al_states != m_CurrentMasterState.al_states)
                {
                    std::cout << "AL States: " << state.al_states << std::endl;
                } 
            }

            m_CurrentMasterState = state;
        }

        void Controller::updateDomainState()
        {
            ec_domain_state_t state;
            ecrt_domain_state(m_Domain, &state);

            if(ENABLE_LOGGING)
            {
                if(state.working_counter != m_CurrentDomainState.working_counter)
                {
                    std::cout << "Domain WC:" << state.working_counter << std::endl; 
                }

                if(state.wc_state != m_CurrentDomainState.wc_state)
                {
                    std::cout << "Domain State: " << state.wc_state << std::endl;
                }
            }

            m_CurrentDomainState = state;
        }
        
    }
}
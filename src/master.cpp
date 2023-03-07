/**
 * @file master.cpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-03-07
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "master.hpp"

namespace ethercat_interface
{
    namespace master
    {

        Master::Master(unsigned int master_index)
            : m_MasterIndex(master_index)
        {
            m_EthercatMaster = ecrt_request_master(master_index);
            if(!m_EthercatMaster)
            {
                // Log.
                exit(0);
            }
        }

        Master::~Master()
        {
            ecrt_release_master(m_EthercatMaster);
        }


        bool Master::activateMaster()
        {
            if(ecrt_master_activate(m_EthercatMaster))
            {
                // Log.
                return false;
            }

            return true;
        }

        void Master::updateMasterState()
        {
            ec_master_state_t state;
            ecrt_master_state(m_EthercatMaster, &state);

            if(ENABLE_LOGGING)
            {
                if(state.slaves_responding != m_EthercatMasterState.slaves_responding)
                {
                    std::cout << state.slaves_responding << " slaves are responding." << std::endl;
                }

                if(state.link_up != m_EthercatMasterState.link_up)
                {
                    std::cout << "Link is " << (state.link_up ? "up" : "down") << std::endl;
                }

                if(state.al_states != m_EthercatMasterState.al_states)
                {
                    std::cout << "AL States: " << state.al_states << std::endl;
                } 
            }

            m_EthercatMasterState = state;
        }
    }
}
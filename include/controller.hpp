/**
 * @file controller.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <iostream>
#include <vector>
#include <memory>

#include <sys/resource.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h> /* clock_gettime() */
#include <sys/mman.h> /* mlockall() */
#include <sched.h> /* sched_setscheduler() */


#include "ecrt.h"

#include "slave.hpp"
#include "utilities.hpp"
#include "interface_exceptions.hpp"

namespace ethercat_interface
{
    namespace controller
    {   
        /**
         * @brief 
         * 
         */
        class Controller
        {
            public:

            Controller(bool enable_logging = false);
            virtual ~Controller();
            
            void start();

            virtual void cyclic_task() = 0;

            virtual void loadSlaves() = 0;

            //virtual void initSlaves();

            inline void setConfigFileNames(const std::vector<std::pair<std::string, std::string>>& slaveConfigFileNames)
            {
                m_SlaveConfigFileNames = slaveConfigFileNames;
            }

            /**
             * @brief Updates the current master state. If ENABLE_LOGGING is set to true, logs the changes in the master.
             * 
             */
            void updateMasterState();

            /**
             * @brief Updates the current domain state. If ENABLE_LOGGING is set to true, logs the changes in the domain.
             * 
             */
            void updateDomainState();

            protected:

            /**
             * @brief Pointer to the EtherCAT Master.
             * 
             */
            ec_master_t* m_EthercatMaster;

            /**
             * @brief Current state of the EtherCAT Master, can be updated by the updateMasterState function. 
             * 
             */
            ec_master_state_t m_CurrentMasterState;
            
            /**
             * @brief Pointer to the domain.
             * 
             */
            ec_domain_t* m_Domain;

            /**
             * @brief Current state of the EtherCAT Domain.
             * 
             */
            ec_domain_state_t m_CurrentDomainState;

            /**
             * @brief Pointer for the domain PD, used for reading/writing values from/to the slave.
             * 
             */
            uint8_t* m_DomainProcessData = nullptr;

            std::vector<slave::Slave*> m_Slaves;

            void configureSlaves();

            std::vector<std::pair<std::string, std::string>> m_SlaveConfigFileNames;

            private:

            bool ENABLE_LOGGING = false;

        };
    }
}

#endif // CONTROLLER_HPP
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

            Controller();
            virtual ~Controller();
            
            void start();

            virtual void cyclic_task() = 0;

            virtual void loadSlaves() = 0;

            //virtual void initSlaves();

            inline void setConfigFileNames(const std::vector<std::pair<std::string, std::string>>& slaveConfigFileNames)
            {
                m_SlaveConfigFileNames = slaveConfigFileNames;
            }

            protected:

            /**
             * @brief Pointer to the EtherCAT Master.
             * 
             */
            ec_master_t* m_EthercatMaster;
            
            /**
             * @brief Pointer to the domain.
             * 
             */
            ec_domain_t* m_Domain;
            /**
             * @brief Pointer for the domain PD, used for reading/writing values from/to the slave.
             * 
             */
            uint8_t* m_DomainProcessData = nullptr;

            std::vector<slave::Slave*> m_Slaves;

            void configureSlaves();

            std::vector<std::pair<std::string, std::string>> m_SlaveConfigFileNames;

            

            private:

            

        };
    }
}

#endif // CONTROLLER_HPP
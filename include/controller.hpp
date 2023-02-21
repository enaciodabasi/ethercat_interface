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
            
            virtual void cyclic_task() = 0;

            virtual void loadSlaves(const std::vector<std::pair<std::string, std::string>>& slaveConfigFileNames) = 0;

            //virtual void initSlaves();

            inline void setConfigFileNames(const std::vector<std::pair<std::string, std::string>>& slaveConfigFileNames)
            {
                m_SlaveConfigFileNames = slaveConfigFileNames;
            }

            protected:

            std::unique_ptr<ec_master_t> m_EthercatMaster;

            std::unique_ptr<ec_domain_t> m_Domain;

            std::vector<std::unique_ptr<slave::Slave>> m_Slaves;

            void configureSlaves();

            std::vector<std::pair<std::string, std::string>> m_SlaveConfigFileNames;

            private:

            

        };
    }
}

#endif // CONTROLLER_HPP
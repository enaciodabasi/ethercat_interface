/**
 * @file controller.cpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "controller.hpp"

namespace ethercat_interface
{
    namespace controller
    {
        Controller::Controller()
        {

        }

        Controller::Controller(const std::string& config_file_path)
            : m_PathToConfigFile{config_file_path}
        {
            
        }

        bool Controller::setup(std::vector<Offset>& slave_offsets)
        {
            auto slaveConfigsOpt = loadSlaveConfig(m_PathToConfigFile);
            auto controllerConfigOpt = parser::parse_controller_config(m_PathToConfigFile);
            if(slaveConfigsOpt == std::nullopt || controllerConfigOpt == std::nullopt)  
            {
                return false;
            }
            const ControllerInfo controllerConfig = controllerConfigOpt.value();

            m_Master = std::make_unique<master::Master>(
                0,
                std::make_shared<logger::Logger>(controllerConfig.logDirPath, logger::FILE)
            );

            for(uint i = 0; i < controllerConfig.numOfDomains; i++)
            {
                m_Master->registerDomain(
                    new domain::Domain(
                        controllerConfig.domainNames[i]
                    )
                );
            }

            const std::vector<SlaveInfo> slaveConfigs = slaveConfigsOpt.value();
            if(!slaveConfigs.empty())
                for(auto slave_config : slaveConfigs)
                {
                    m_Master->addSlaveToDomain(slave_config);
                }

            m_Master->configureDomains();

            // Write to SDOs via COE

            

            // PREOP TO OP 

            m_Master->setupDomains();



            if(!on_startup())
            {
                return false;
            }
    

            // Call the start-up function for the slave config via SDO's in PRE-OP State.
            // Return the result. 
            return true;
        }

        bool Controller::on_startup()
        {

        }

        std::optional<std::vector<SlaveInfo>> Controller::loadSlaveConfig(
            std::string_view config_file_path
        )
        {
            return utilities::parse_config_file(config_file_path);
        }

        bool Controller::updateThreadInfo()
        {
            if(pthread_setschedparam(m_CyclicTaskThread.native_handle(), m_ThreadInfo.threadPolicy, &m_ThreadInfo.schedParam))
            {
                return false;
            }

            return true;
        }
    } // End of namespace controller

} // End of namespace ethercat_interface
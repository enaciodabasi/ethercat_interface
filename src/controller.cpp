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

#include "ethercat_interface/controller.hpp"
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

        Controller::~Controller()
        {
            if(m_CyclicTaskThread.joinable())
            {
                m_CyclicTaskThread.join();
            }
        }

        bool Controller::setup()
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
            auto startupConfig_o = utilities::parser::parse_startup_configs(m_PathToConfigFile);
            bool doStartupRoutine = true;

            if(startupConfig_o != std::nullopt){doStartupRoutine = false;}
            if(doStartupRoutine)
            {
                auto& startupConfigs = startupConfig_o.value();

                if(!on_startup(startupConfigs, slaveConfigs))
                {   
                    return false;
                }

                m_StartupInfos = startupConfigs;
            }

            // PREOP TO OP 

            m_Master->setupDomains();    

            // Activate ECRT Master.
            // Return the result. 
            return m_Master->activateMaster();
        }

        bool Controller::on_startup(std::vector<StartupInfo>& startup_configs, const std::vector<SlaveInfo>& slave_configs)
        {   
            if(startup_configs.empty()) // Check just to be sure.
            {
                return true;
            }

            for(auto& startup_config : startup_configs) // Iterate over all startup configurations
            {
                const std::string slaveNameTemp = startup_config.slaveName;

                std::optional<int> slavePosition_o = [&slave_configs, &slaveNameTemp]() -> std::optional<int>{

                    for(const auto sc : slave_configs)
                    {
                        if(sc.slaveName == slaveNameTemp)
                        {
                            return sc.position;
                        }
                        else
                            continue;
                    }

                    return std::nullopt;
                    
                }();  

                if(slavePosition_o == std::nullopt)
                {
                    return false;
                }

                int slavePosition = slavePosition_o.value();

                if(std::holds_alternative<uint8_t>(startup_config.data))
                {
                    
                    uint8_t valToWrite = std::get<uint8_t>(startup_config.data);

                    if(startup_config.configType == SdoConfigType::WRITE)
                    {
                        bool success =  m_Master->sdo_write<uint8_t>(
                            slavePosition,
                            startup_config.sdoInfo,
                            valToWrite
                        );

                        if(!success)
                            return false;

                    }
                    else if(startup_config.configType == SdoConfigType::READ)
                    {
                        auto result = m_Master->sdo_read<uint8_t>(
                            slavePosition,
                            startup_config.sdoInfo
                        );

                        if(result == std::nullopt)
                        {
                            return false;
                        }

                        startup_config.data = result.value();
                    }
                }
                else if(std::holds_alternative<uint16_t>(startup_config.data))
                {

                    uint16_t valToWrite = std::get<uint16_t>(startup_config.data);

                    if(startup_config.configType == SdoConfigType::WRITE)
                    {
                        bool success = m_Master->sdo_write<uint16_t>(
                            slavePosition,
                            startup_config.sdoInfo,
                            valToWrite
                        );

                        if(!success)
                            return false;

                    }
                    else if(startup_config.configType == SdoConfigType::READ)
                    {
                        auto result = m_Master->sdo_read<uint16_t>(
                            slavePosition,
                            startup_config.sdoInfo
                        );

                        if(result == std::nullopt)
                        {
                            return false;
                        }

                        startup_config.data = result.value();
                    }
                }
                else if(std::holds_alternative<uint32_t>(startup_config.data))
                {
                    uint32_t valToWrite = std::get<uint32_t>(startup_config.data);
                    
                    if(startup_config.configType == SdoConfigType::WRITE)
                    {
                        bool success = m_Master->sdo_write<uint32_t>(
                            slavePosition,
                            startup_config.sdoInfo,
                            valToWrite
                        );

                        if(!success)
                            return false;

                    }
                    else if(startup_config.configType == SdoConfigType::READ)
                    {
                        auto result = m_Master->sdo_read<uint32_t>(
                            slavePosition,
                            startup_config.sdoInfo
                        );

                        if(result == std::nullopt)
                        {
                            return false;
                        }

                        startup_config.data = result.value();
                    }
                }
                else if(std::holds_alternative<uint64_t>(startup_config.data))
                {
                    
                    uint64_t valToWrite = std::get<uint64_t>(startup_config.data);

                    if(startup_config.configType == SdoConfigType::WRITE)
                    {
                        bool success = m_Master->sdo_write<uint64_t>(
                            slavePosition,
                            startup_config.sdoInfo,
                            valToWrite
                        );

                        if(!success)
                            return false;
                    }
                    else if(startup_config.configType == SdoConfigType::READ)
                    {
                        auto result = m_Master->sdo_read<uint64_t>(
                            slavePosition,
                            startup_config.sdoInfo
                        );

                        if(result == std::nullopt)
                        {
                            return false;
                        }

                        startup_config.data = result.value();
                    }
                }
            }

            return true;
        }

        std::optional<std::vector<SlaveInfo>> Controller::loadSlaveConfig(
            std::string_view config_file_path
        )
        {
            return utilities::parse_config_file(config_file_path);
        }

        void Controller::startTask()
        {
            m_CyclicTaskThread = std::thread(
                &Controller::cyclicTask,
                this
            );
            updateThreadInfo();
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
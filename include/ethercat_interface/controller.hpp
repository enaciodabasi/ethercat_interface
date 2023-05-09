/**
 * @file controller.hpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief Contains abstract class definition for controlling an EtherCAT loop. 
 * @version 0.1
 * @date 2023-04-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <thread>
#include <mutex>
#include <shared_mutex>
#include <optional>
#include <any>

#include <pthread.h>

#include "master.hpp"
#include "domain.hpp"
#include "slave.hpp"
#include "utilities.hpp"
#include "offset.hpp"
#include "data.hpp"

#include <signal.h>

extern "C"
{
    #include "dc_sync.h"
}

namespace ethercat_interface
{
    namespace controller
    {
        /* namespace 
        {
            template<typename ... Ts>                                                 // (7) 
            struct Overload : Ts ... { 
                using Ts::operator() ...;
            };
            template<class... Ts> Overload(Ts...) -> Overload<Ts...>;
        }  */  

        /**
         * @brief Semi-abstract class to implement an EtherCAT control loop.
         * 
         */
        class Controller
        {
            public:

            /**
             * @brief Default constructor. If the default constructor is used
             * the loading of the slaves must be done by calling the loadSlaveConfig function
             * with the path to the config file.
             * 
             */
            Controller();

            /**
             * @brief Constructs a new Controller object and loads the slaves.
             * 
             * @param config_file_path Path to the YAML file containing information about the slaves.
             */
            Controller(const std::string& config_file_path);        

            virtual ~Controller();

            virtual void cyclicTask() = 0;
            
            /**
             * @brief Loads the slave config information.
             * 
             * @param config_file_path Path to the YAML file containing information about the slaves
             * @return true If there is error. 
             * @return false If there is no error.
             */
            std::optional<std::vector<SlaveInfo>> loadSlaveConfig(std::string_view config_file_path);

            /**
             * @brief Brings the whole system to operational mode.
             * 
             * @return true if no error is raised during setup. 
             * @return false if an error has been raised during setup.
             */
            virtual bool setup();

            template<typename T>
            std::optional<T> getData(
                const std::string& owner_name,
                const std::string& data_name
            );

            template<typename T>
            bool setData(
                const std::string& owner_name,
                const std::string& data_name,
                const T& new_val
            );

            protected:

            std::unique_ptr<master::Master> m_Master;

            std::thread m_CyclicTaskThread;

            std::unordered_map<std::string, std::unique_ptr<PDO::DataContainer>> m_SharedData;

            bool m_IsThreadRunning;

            DistributedClockHelper m_DcHelper;            

            /**
             * @brief Configures the slaves during their PRE-OP state.
             * 
             * @return true if no error is raised.
             * @return false if an error occurs.
             */
            virtual bool on_startup(std::vector<StartupInfo>& startup_configs, const std::vector<SlaveInfo>& slave_configs);

            /**
             * @brief Override this function to start your overriden cyclicTask method.
             * 
             */
            virtual void startTask();

            /**
             * @brief Updates the threads policy and priority using std::thread::native_handle in pthread_setschedparam.
             * 
             * @return true if pthread_setschedparam is successful.
             * @return false if pthread_setschedparam fails.
             */
            bool updateThreadInfo();

            /**
             * @brief Set the policy and priority of the task thread.
             * 
             * @param policy 
             * @param priority 
             */
            inline void setThreadParams(int policy, int priority)
            {
                m_ThreadInfo.threadPolicy = policy;
                m_ThreadInfo.threadPriority = priority;
                m_ThreadInfo.schedParam.sched_priority = m_ThreadInfo.threadPriority;
            }

            inline bool joinThread()
            {
                if(m_CyclicTaskThread.joinable())
                {
                    m_CyclicTaskThread.join();
                    return true;
                }
                return false;

            }
            
            void setTaskWakeUpTime();

            private:

            std::string m_PathToConfigFile;
        
            std::vector<StartupInfo> m_StartupInfos;

            struct
            {
                sched_param schedParam;
                int threadPolicy = SCHED_FIFO;
                int threadPriority = 19;
            } m_ThreadInfo;
        
        };

        template<typename T>
        std::optional<T> Controller::getData(
            const std::string& owner_name,
            const std::string& data_name
        )
        {
            auto owner = m_SharedData.find(owner_name);
            if(owner == m_SharedData.end())
            {
                return std::nullopt;
            }
            return owner->second->get<T>(data_name);
        }

        template<typename T>
        bool Controller::setData(
            const std::string& owner_name,
            const std::string& data_name,
            const T& new_val
        )
        {   
            auto owner = m_SharedData.find(owner_name);
            if(owner == m_SharedData.end())
            {
                return false;
            }

            return owner->second->set(data_name, new_val);
        }

    } // End of namespace controller

} // End of namespace ethercat_interface

#endif // CONTROLLER_HPP
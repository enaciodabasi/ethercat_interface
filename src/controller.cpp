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

        bool Controller::setup()
        {

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
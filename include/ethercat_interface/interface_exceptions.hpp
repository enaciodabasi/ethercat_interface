/**
 * @file interface_exceptions.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef INTERFACE_EXCEPTIONS_HPP
#define INTERFACE_EXCEPTIONS_HPP

#include <exception>
#include <string>

namespace ethercat_interface
{
    namespace exception
    {
        class EtherCAT_Interface_Exception : public std::exception
        {
            public:

            EtherCAT_Interface_Exception(const char* msg)
                : m_Message{msg}{}

            const char* what()
            {
                return m_Message.c_str();
            }

            private:

            std::string m_Message;

        };

        class MasterRequestException : public EtherCAT_Interface_Exception
        {
            public:

            MasterRequestException()
                : EtherCAT_Interface_Exception(m_Message){}

            private:

            char* m_Message = "Failed during EtherCAT Master request.\n";

        };

        class CreateDomainException : public EtherCAT_Interface_Exception
        {
            public:

            CreateDomainException()
                : EtherCAT_Interface_Exception(m_Message.c_str()){}

            private:

            std::string m_Message = "Failed creating domain.\n";
        };
    }
}

#endif
/**
 * @file slave.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-02-14
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef SLAVE_HPP
#define SLAVE_HPP

#include <iostream>

#include "ecrt.h"

namespace ethercat_interface
{
    class Slave
    {
        public:

        Slave(const std::string& slave_name);

        private:

        std::string m_SlaveName;

        int m_VendorID;

        int m_ProductCode;

        int m_Position;

        int m_Alias;



    };  
} // end of namespace ethercat_interface

#endif // SLAVE_HPP
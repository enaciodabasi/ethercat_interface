/**
 * @file data_defs.cpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief Implementation of a thread safe representation of PDO data.
 * @version 0.1
 * @date 2023-04-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "../include/data_defs.hpp"

namespace ethercat_interface
{
    namespace PDO
    {
        template<class Ps>
        Data<Ps>::Data()
        {

        }

        template<class Ps>
        Data<Ps>::~Data()
        {

        }

        template<class Ps>
        std::shared_ptr<Ps> Data<Ps>::getData()
        {
            std::lock_guard<std::mutex> lg(m_MutexRW);
            if(m_PdoDataQueue.empty()) // If there is no data in the queue, return an empty shared_ptr (== nullptr)
            {
                return std::shared_ptr<Ps>();
            }
            std::shared_ptr<Ps> latestPdoData = m_PdoDataQueue.front();

            m_PdoDataQueue.pop(); // Remove PDO data struct from the queue.

            return latestPdoData;
        }

        template<class Ps>
        bool Data<Ps>::writeData(Ps pdo_data_to_write)
        {
            std::shared_ptr<Ps> newData = std::make_shared<Ps>(std::move(pdo_data_to_write));

            std::lock_guard<std::mutex> lg(m_MutexRW);
            m_PdoDataQueue.push(newData);
        }
    }
}
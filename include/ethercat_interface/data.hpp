/**
 * @file pdo_data.hpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief Implementation of a thread safe representation of PDO data.
 * @version 0.1
 * @date 2023-04-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef PDO_DATA_HPP
#define PDO_DATA_HPP

#include <queue>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <unordered_map>
#include <variant>
#include <string>

namespace ethercat_interface
{

    namespace PDO
    {
        typedef std::variant<
            uint8_t,
            uint16_t,
            uint32_t,
            uint64_t,
            int8_t,
            int16_t,
            int32_t,
            int64_t,
            float,
            double,
            bool
        > PdoVar;

        template<class Ps>
        class Data
        {
            public:

            Data();
            ~Data();

            std::shared_ptr<Ps> getData();

            bool writeData(Ps pdo_data_to_write);

            private:

            std::queue<std::shared_ptr<Ps>> m_PdoDataQueue;

            /**
             * @brief Mutex used when pushing to and popping from the queue.
             * 
             */
            std::mutex m_MutexRW;

        };
    }

}

#endif // PDO_DATA_HPP
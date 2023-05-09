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
#include <algorithm>

#include "ethercat_interface/utilities.hpp"

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

        class DataContainer
        {
            public:

            DataContainer(const std::string& owner_name);

            DataContainer(
                const std::string& owner_name,
                const std::vector<std::pair<std::string, ethercat_interface::EC_Type>>& pdo_info
            );

            ~DataContainer();

            template<typename T>
            bool createData(const std::string& new_data_name);

            template<typename T> 
            std::optional<T> get(const std::string& data_name);

            template<typename T>
            bool set(const std::string& data_name, const T new_val);

            private:

            std::string m_OwnerName;

            std::unordered_map<std::string, PdoVar> m_DataMap;

            std::mutex m_MutexRW;
        };

        template<typename T>
        bool DataContainer::createData(const std::string& new_data_name)
        {
            auto found = m_DataMap.find(new_data_name);
            if(found != m_DataMap.end())
            {
                return false;
            }

            m_DataMap[new_data_name] = T();

            return true;
        }

        template<typename T>
        std::optional<T> DataContainer::get(const std::string& data_name)
        {
            std::unique_lock<std::mutex>(m_MutexRW);

            auto found = m_DataMap.find(data_name);
            if(found == m_DataMap.end())
            {
                return std::nullopt;
            }

            return std::get<T>(found->second);
        }

        template<typename T>
        bool DataContainer::set(const std::string& data_name, const T new_val)
        {
            std::unique_lock<std::mutex>(m_MutexRW);

            auto found = m_DataMap.find(data_name);
            if(found == m_DataMap.end())
            {
                return false;
            }
            
            // Check if template argument is same with the initial data type of the variant
            // If not return false.
            if(!std::holds_alternative<T>(m_DataMap.at(data_name)))
            {
                return false;
            }

            m_DataMap[data_name] = new_val;

            return true;
        }
    }

}

#endif // PDO_DATA_HPP
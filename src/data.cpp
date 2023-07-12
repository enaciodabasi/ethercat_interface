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

#include "ethercat_interface/data.hpp"

namespace ethercat_interface
{
    namespace PDO
    {
        
        DataContainer::DataContainer(const std::string& owner_name)
            : m_OwnerName(owner_name)
        {
            
        }

        DataContainer::DataContainer(
            const std::string& owner_name,
            const std::vector<std::pair<std::string, ethercat_interface::EC_Type>>& pdo_info
        )   : m_OwnerName(owner_name)
        {
            for(const auto& pdo : pdo_info)
            {

                switch (pdo.second)
                {
                case EC_Type::UINT8:
                    /* m_Data.push_back(new Data<uint8_t>(pdo.first)); */
                    m_DataMap[pdo.first] = uint8_t();
                break;
                case EC_Type::UINT16:
                    /* m_Data.push_back(new Data<uint16_t>(pdo.first)); */
                    m_DataMap[pdo.first] = uint16_t();
                break;
                case EC_Type::UINT32:
                    /* m_Data.push_back(new Data<uint32_t>(pdo.first)); */
                    m_DataMap[pdo.first] = uint32_t();
                break;
                case EC_Type::UINT64:
                    /* m_Data.push_back(new Data<uint64_t>(pdo.first)); */
                    m_DataMap[pdo.first] = uint64_t();
                break;
                case EC_Type::INT8:
                    /* m_Data.push_back(new Data<int8_t>(pdo.first)); */
                    m_DataMap[pdo.first] = int8_t();
                break;
                case EC_Type::INT16:
                    /* m_Data.push_back(new Data<int16_t>(pdo.first)); */
                    m_DataMap[pdo.first] = int16_t();
                break;
                case EC_Type::INT32:
                    /* m_Data.push_back(new Data<int32_t>(pdo.first)); */
                    m_DataMap[pdo.first] = int32_t();
                break;
                case EC_Type::INT64:
                    /* m_Data.push_back(new Data<int64_t>(pdo.first)); */
                    m_DataMap[pdo.first] = int64_t();
                break;
                case EC_Type::FLOAT:
                    /* m_Data.push_back(new Data<float>(pdo.first)); */
                    m_DataMap[pdo.first] = float();
                break;
                case EC_Type::DOUBLE:
                    /* m_Data.push_back(new Data<double>(pdo.first)); */
                    m_DataMap[pdo.first] = double();
                break;
                case EC_Type::BIT:
                    /* m_Data.push_back(new Data<bool>(pdo.first)); */
                    m_DataMap[pdo.first] = bool();
                break;
                default:
                    break;
                }
            }

        }

        const std::string DataContainer::gatherDataRxPDO()
        {
            std::string dataStr();

            for(const auto [key, value] : m_DataMap)
            {
                /* dataStr += key + ": " +  + " \n"; */
            }
        }
    }
}
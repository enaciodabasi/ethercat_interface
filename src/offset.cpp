/**
 * @file offset.cpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "../include/offset.hpp"

namespace ethercat_interface
{
    namespace offset
    {
        DataOffset::DataOffset(){}
    
        DataOffset::DataOffset(const std::vector<std::string>& pdo_names)
        {
            for(const std::string& pdo : pdo_names)
            {
                m_DataOffsets.emplace_back(std::make_pair(pdo, uint()));
            }
    
            for(std::size_t i = 0; i < m_DataOffsets.size(); i++)
            {
                m_DataOffsetAdressMap[m_DataOffsets[i].first] = &m_DataOffsets[i].second;
            }
        }
    
        DataOffset::~DataOffset()
        {
            
        }
    
        std::optional<unsigned int*> DataOffset::getDataOffset(const std::string&offset_name)
        {
            auto result = m_DataOffsetAdressMap.find(offset_name);
            if(result == m_DataOffsetAdressMap.end())
            {
                return std::nullopt;
            }    
    
            return result->second;
    
        }

    }
}
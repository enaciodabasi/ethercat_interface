/**
 * @file parser.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-05-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef PARSER_HPP_
#define PARSER_HPP_

#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <optional>
#include <iostream>
#include <variant>
#include <expected>

#include <yaml-cpp/yaml.h>

#include "ethercat_interface/common_defs.hpp"

namespace ethercat_interface
{
    namespace parser
    {

        using SlaveInformation = std::variant<SlaveInfo, std::vector<SlaveInfo>>;    
        
        // Contains constexpr definitions for making some parsing operations easier.
        namespace
        {
            const std::map<std::string, SlaveType> slaveTypes = {
                {"driver", SlaveType::Driver},
                {"plc", SlaveType::PLC},
                {"io", SlaveType::IO},
                {"coupler", SlaveType::Coupler}
            };

            const std::unordered_map<std::string, DataType> dataTypes = {
                {"uint8", DataType::UINT8},
                {"int8", DataType::INT8},
                {"uint16", DataType::UINT16},
                {"int16", DataType::INT16},
                {"uint32", DataType::UINT32},
                {"int32", DataType::INT32},
                {"uint64", DataType::UINT64},
                {"int64", DataType::INT64},
                {"double", DataType::DOUBLE},
                {"float", DataType::FLOAT}
            };
        }

        /**
         * @brief Parses the given node for information about the slave.
         * 
         * @param slave_node YAML node that is not a program or startup(?) node. 
         * @return std::optional<SlaveInfo> if any crucial configuration is not found returns a nullopt
         */
        std::expected<SlaveInformation, ReturnType> parseSlaveConfig(const YAML::Node& slave_node);

        /**
         * @brief Parses the .yaml configuration file specified in the path_to_config_file parameter
         * 
         * @param path_to_config_file 
         */
        std::expected<ProgramConfig, ReturnType> parseConfigFile(const std::string& path_to_config_file);
    

    } // End of namespace parser
} // End of namespace ethercat_interface

#endif // PARSER_HPP_
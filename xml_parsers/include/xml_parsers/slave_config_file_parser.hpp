/**
 * @file slave_config_file_parser.cpp
 * @author Eren Naci Odabasi (enaciodabasi@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-05
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef SLAVE_CONFIG_FILE_PARSER_HPP
#define SLAVE_CONFIG_FILE_PARSER_HPP

#include <pugixml.hpp>

#include <iostream>
#include <optional>
#include <vector>


namespace parser

{

    
    struct Entry;   
    struct Object;

    typedef std::vector<Entry> Entries;

    typedef std::vector<Object> Objects;

    enum PdoType
    {
        RxPdo,
        TxPdo
    };

    struct Entry
    {
            std::string index;
            std::string subindex;
            std::string bitLength;
            std::string name;
            std::string dataType;
    };

    struct PdoInfo
    {   
        PdoType pdoType;
        std::string index;
        std::string name;
        Entries entries;

        void toString()
        {
            std::cout
                << name << " " << index << " " << ((pdoType == PdoType::RxPdo) ? "RxPDO" : "TxPDO")
                << " " << "PDO Entries: " 
                << std::endl;

            for(auto entry : entries)
            {
                std::cout << entry.name << " " << entry.index << " " << entry.subindex 
                << " " << entry.bitLength << " " << entry.dataType << std::endl;
            }
        }
    };  

    struct OpModeInfo 
    {
        std::string name;
        std::string description;
        std::string assignActivate;
        
        std::string cycleTimeSync0_Factor;
        std::string cycleTimeSync0_Value;
        std::string shiftTimeSync0_Input;
        std::string shiftTimeSync0_Value;

        std::string cycleTimeSync1_Factor;
        std::string cycleTimeSync1_Value;
        std::string shiftTimeSync1_Input;
        std::string shiftTimeSync1_Value;

        void toString()
        {
            std::cout
                << "Name: " << name << '\n'
                << "Description: " << description << '\n'
                << "Cycle Time Sync 0: Factor: " << cycleTimeSync0_Factor << " " << "Value: " << cycleTimeSync0_Value 
                << "\nShift Time Sync 0: Input: " << shiftTimeSync0_Input << " " << "Value: " << shiftTimeSync0_Value
                << "Cycle Time Sync 1: Factor: " << cycleTimeSync1_Factor << " " << "Value: " << cycleTimeSync1_Value 
                << "\nShift Time Sync 1: Input: " << shiftTimeSync1_Input << " " << "Value: " << shiftTimeSync1_Value
                << std::endl;
        }
    };

    struct Object
    {
    
        std::string index;
        std::string name;
        std::string type;
        std::string bitsize;
        std::string accessFlag;

        void toString()
        {
            std::cout 
                << "Index: " << index
                << "\nName: " << name
                << "\nType: " << type
                << "\nBit Size: " << bitsize
                << "\nAccess: " << accessFlag
                << std::endl;
        }
    };
    

    struct SlaveConfig
    {
        std::string vendorIdStr;
        std::string productCodeStr;
        std::string revisionNoStr;
        std::string fullDeviceName;
        
        std::vector<PdoInfo> rxPDOs;

        std::vector<PdoInfo> txPDOs;

        std::vector<OpModeInfo> opModes;

        Objects objects;

        void toString()
        {
            std::cout 
                << "\nVendor ID: " << vendorIdStr
                << "\nProduct Code: " << productCodeStr
                << "\nRevision No: " << revisionNoStr
                << "\nDevice Name: " << fullDeviceName

                << std::endl;

            for(auto pdo : rxPDOs)
                pdo.toString();

            for(auto pdo : txPDOs)
                pdo.toString();
            
            for(auto opmode : opModes)
                opmode.toString();

            for(auto object : objects)
                object.toString();
        }
    };

    /**
     * @brief Finds device spesific information from the given XML node.
     * 
     * @param device_node Reference to the Device node in the XML file.
     * @param conf Pointer to the SlaveConfig struct to edit.
     */
    void getPdoInfo(
        pugi::xml_node& device_node,
        SlaveConfig* conf
    );

    /**
     * @brief Finds the operation mode configuration (Distributed Clock and Synchron Manager)
     * 
     * @param device_node Reference to the Device node in the XML file. 
     * @param conf Pointer to the SlaveConfig struct to edit.
     */
    void getOpModeInfo(
        pugi::xml_node& device_node,
        SlaveConfig* conf
    );

    void getObjects(
        pugi::xml_node& device_node,
        SlaveConfig* conf
    );

    /**
     * @brief 
     * 
     * @param path_to_slave_config_file 
     * @param slave_name 
     * @param vendor_id 
     * @param product_code 
     * @param revision_num 
     * @return std::optional<SlaveConfig> 
     */
    std::optional<SlaveConfig> parseSlaveConfigFile(
        const std::string& path_to_slave_config_file,
        const std::string& slave_name,
        const std::string& vendor_id,
        const std::string& product_code,
        const std::string&  revision_num 
    );

}

#endif // SLAVE_CONFIG_FILE_PARSER_HPP
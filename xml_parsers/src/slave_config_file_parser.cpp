#include "xml_parsers/slave_config_file_parser.hpp"


namespace parser
{

    std::optional<SlaveConfig> parseSlaveConfigFile(
        const std::string& path_to_slave_config_file,
        const std::string& slave_name,
        const std::string& vendor_id,
        const std::string& product_code,
        const std::string& revision_num 
    )
    {   
        pugi::xml_document xmlDoc;
        auto parseResult = xmlDoc.load_file(
            path_to_slave_config_file.c_str(), 
            116U, 
            pugi::xml_encoding::encoding_latin1
        );
        if(!parseResult)
        {
            return std::nullopt;
        }

        SlaveConfig* conf = nullptr;
        
        pugi::xml_node ethercatInfoNode = xmlDoc.child("EtherCATInfo");

        if(vendor_id != ethercatInfoNode.child("Vendor").child("Id").child_value())
        {
            return std::nullopt;
        }

        pugi::xml_node descriptionsNode = ethercatInfoNode.child("Descriptions");
        pugi::xml_node devicesNode = descriptionsNode.child("Devices");

        pugi::xml_node foundDeviceNode;

        for(pugi::xml_node device = devicesNode.first_child(); device; device = device.next_sibling())
        {
            pugi::xml_node typeNode = device.child("Type");

            pugi::xml_attribute productCodeAtr = typeNode.attribute("ProductCode");
            pugi::xml_attribute revisionNoAtr = typeNode.attribute("RevisionNo");
            
            if(productCodeAtr.empty() || revisionNoAtr.empty())
            {
                break;
            }
            const std::string productCodeStr = productCodeAtr.as_string();
            const std::string revisionNoStr = revisionNoAtr.as_string();
            /* std::cout << "PC: " << productCodeStr << " " << "RN: " << revisionNoStr << std::endl; */

            // Compare the given parameters with
            // the current attributes found in the XML Device node.
            // If the product code and the revision number do not match, continue on to the next Device Node.
            if(productCodeStr != product_code || revisionNoStr != revision_num)
            {   
                std::cout << "No match" << std::endl;
                continue;
            }
            else
            {   
                conf = new SlaveConfig();
                conf->vendorIdStr = vendor_id;
                conf->productCodeStr = productCodeStr;
                conf->revisionNoStr = revisionNoStr; 
                foundDeviceNode = device;
                break;
            }

        }

        getPdoInfo(foundDeviceNode, conf);

        getOpModeInfo(foundDeviceNode, conf);
        
        if(conf)
        {
            auto slaveConfig = *conf;
            delete conf;
            return slaveConfig;
        }
                
        return std::nullopt;
    }


    void getPdoInfo(
        pugi::xml_node& device_node,
        SlaveConfig* conf
    )
    {
        conf->fullDeviceName = device_node.child("Type").next_sibling().child_value();

        // Find RxPDO information
        std::vector<PdoInfo> rxPdos;

        for(pugi::xml_node device_child = device_node.first_child(); device_child; device_child = device_child.next_sibling())
        {   
            std::string childName = device_child.name();

            if(childName != "RxPdo")
                continue;
            
            PdoInfo p;
            p.pdoType = PdoType::RxPdo;

            //pugi::xml_node currPdo = device_child;

            // Get PDO index
            auto pdoIndex = device_child.child("Index");
            std::string pdoIndexVal = pdoIndex.child_value();
            if(!pdoIndexVal.empty())
                p.index = pdoIndexVal;

            // Get PDO Name
            auto pdoName = device_child.child("Name");
            std::string pdoNameVal = pdoName.child_value();
            if(!pdoNameVal.empty())
                p.name = pdoNameVal;

            // Get PDO entry node
            pugi::xml_node pdoEntry = device_child.child("Entry");
            std::string tempChildName = pdoEntry.name();
            for(pdoEntry; tempChildName == "Entry"; pdoEntry = pdoEntry.next_sibling())
            {
                tempChildName = pdoEntry.name();
                Entry entry;
                

                // Get PDO Entry Index
                auto pdoEntryIndex = pdoEntry.child("Index");
                std::string pdoEntryIndexVal = pdoEntryIndex.child_value();
                if(!pdoEntryIndexVal.empty())
                    entry.index = pdoEntryIndexVal;

                // Get PDO Entry Subindex
                auto pdoEntrySubindex = pdoEntry.child("Subindex");
                std::string pdoEntrySubindexVal = pdoEntrySubindex.child_value();
                if(!pdoEntrySubindexVal.empty())
                    entry.subindex = pdoEntrySubindexVal;

                // Get PDO entry Bit Length
                auto pdoEntryBitLen = pdoEntry.child("BitLen");
                std::string pdoEntryBitLenVal = pdoEntryBitLen.child_value();
                if(!pdoEntryBitLenVal.empty())
                    entry.bitLength = pdoEntryBitLenVal;

                // Get PDO Entry Name
                auto pdoEntryName = pdoEntry.child("Name");
                std::string pdoEntryNameVal = pdoEntryName.child_value();
                if(!pdoEntryNameVal.empty())
                    entry.name = pdoEntryNameVal;

                // Get PDO Entry Data Type
                auto pdoEntryDataType = pdoEntry.child("DataType");
                std::string pdoEntryDataTypeVal = pdoEntryDataType.child_value();
                if(!pdoEntryDataTypeVal.empty())
                    entry.dataType = pdoEntryDataTypeVal;

                p.entries.emplace_back(std::move(entry));

            }
            
            rxPdos.emplace_back(std::move(p));

        }

        if(!rxPdos.empty())
        {
            conf->rxPDOs = rxPdos;
        }

        std::vector<PdoInfo> txPDOs;

        for(pugi::xml_node device_child = device_node.first_child(); device_child; device_child = device_child.next_sibling())
        {   
            std::string childName = device_child.name();

            if(childName != "TxPdo")
                continue;
            
            PdoInfo p;
            p.pdoType = PdoType::TxPdo;

            //pugi::xml_node currPdo = device_child;

            // Get PDO index
            auto pdoIndex = device_child.child("Index");
            std::string pdoIndexVal = pdoIndex.child_value();
            if(!pdoIndexVal.empty())
                p.index = pdoIndexVal;

            // Get PDO Name
            auto pdoName = device_child.child("Name");
            std::string pdoNameVal = pdoName.child_value();
            if(!pdoNameVal.empty())
                p.name = pdoNameVal;

            // Get PDO entry node
            pugi::xml_node pdoEntry = device_child.child("Entry");
            std::string tempChildName = pdoEntry.name();
            for(pdoEntry; tempChildName == "Entry"; pdoEntry = pdoEntry.next_sibling())
            {
                tempChildName = pdoEntry.name();
                Entry entry;
                // Get PDO Entry Index
                auto pdoEntryIndex = pdoEntry.child("Index");
                std::string pdoEntryIndexVal = pdoEntryIndex.child_value();
                if(!pdoEntryIndexVal.empty())
                    entry.index = pdoEntryIndexVal;

                // Get PDO Entry Subindex
                auto pdoEntrySubindex = pdoEntry.child("Subindex");
                std::string pdoEntrySubindexVal = pdoEntrySubindex.child_value();
                if(!pdoEntrySubindexVal.empty())
                    entry.subindex = pdoEntrySubindexVal;

                // Get PDO entry Bit Length
                auto pdoEntryBitLen = pdoEntry.child("BitLen");
                std::string pdoEntryBitLenVal = pdoEntryBitLen.child_value();
                if(!pdoEntryBitLenVal.empty())
                    entry.bitLength = pdoEntryBitLenVal;

                // Get PDO Entry Name
                auto pdoEntryName = pdoEntry.child("Name");
                std::string pdoEntryNameVal = pdoEntryName.child_value();
                if(!pdoEntryNameVal.empty())
                    entry.name = pdoEntryNameVal;

                // Get PDO Entry Data Type
                auto pdoEntryDataType = pdoEntry.child("DataType");
                std::string pdoEntryDataTypeVal = pdoEntryDataType.child_value();
                if(!pdoEntryDataTypeVal.empty())
                    entry.dataType = pdoEntryDataTypeVal;

                p.entries.emplace_back(std::move(entry));

            }

            txPDOs.emplace_back(std::move(p));

        }

        if(!txPDOs.empty())
        {
            conf->txPDOs = txPDOs;
        }
        
    }

    void getOpModeInfo(
        pugi::xml_node& device_node,
        SlaveConfig* conf
    )
    {
        pugi::xml_node dcNode = device_node.child("Dc");

        if(dcNode.empty())
            return;

        for(pugi::xml_node opModeNode = dcNode.first_child(); opModeNode; opModeNode = opModeNode.next_sibling())
        {
            OpModeInfo opModeInfo;

            opModeInfo.name = opModeNode.child("Name").child_value();
            opModeInfo.description = opModeNode.child("Desc").child_value();
            opModeInfo.assignActivate = opModeNode.child("AssignActivate").child_value();

            if(auto cycleTimeSync0FactorAttr = opModeNode.child("CycleTimeSync0").attribute("Factor"))
            {
                if(!cycleTimeSync0FactorAttr.empty())
                {
                    opModeInfo.cycleTimeSync0_Factor = cycleTimeSync0FactorAttr.as_string();
                }
            }
            opModeInfo.cycleTimeSync0_Value = opModeNode.child("CycleTimeSync1").child_value();
            
            if(auto shiftTimeSync0FactorAttr = opModeNode.child("ShiftTimeCycle0").attribute("Input"))
            {
                if(!shiftTimeSync0FactorAttr.empty())
                {
                    opModeInfo.shiftTimeSync0_Input = shiftTimeSync0FactorAttr.as_string();
                }
            }
            opModeInfo.shiftTimeSync0_Value = opModeNode.child("ShiftTimeCycle0").child_value();
            

            if(auto cycleTimeSync1FactorAttr = opModeNode.child("CycleTimeSync1").attribute("Factor"))
            {
                if(!cycleTimeSync1FactorAttr.empty())
                {
                    opModeInfo.cycleTimeSync1_Factor = cycleTimeSync1FactorAttr.as_string();
                }
            }
            opModeInfo.cycleTimeSync1_Value = opModeNode.child("CycleTimeSync1").child_value();

            if(auto shiftTimeSync1FactorAttr = opModeNode.child("ShiftTimeCycle1").attribute("Input"))
            {
                if(!shiftTimeSync1FactorAttr.empty())
                {
                    opModeInfo.shiftTimeSync1_Input = shiftTimeSync1FactorAttr.as_string();
                }
            }
            opModeInfo.shiftTimeSync1_Value = opModeNode.child("ShiftTimeCycle1").child_value();
        
            conf->opModes.push_back(opModeInfo);
        }
    }

}

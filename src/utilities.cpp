#include "ethercat_interface/utilities.hpp"

#include <algorithm>

namespace ethercat_interface
{
    namespace utilities
    {   

        namespace parser
        {
            
            std::optional<ControllerInfo> parse_controller_config(std::string_view config_file_path)
            {
                ControllerInfo controllerInfo;

                if(auto controller_config_doc = [&config_file_path](){
                    const auto allDocs = YAML::LoadAllFromFile(static_cast<std::string>(config_file_path));
                    YAML::Node configNode;
                    for(const auto& doc: allDocs)
                    {
                        if(const auto tempNode = doc["controller_config"])
                        {
                            configNode = tempNode;
                        }
                        else
                            continue;
                    }

                    return configNode;
                }())
                {
                     std::vector<std::string> domain_names = controller_config_doc["domain_names"].as<std::vector<std::string>>();

                    if(domain_names.empty())
                    {
                        return std::nullopt;
                    }
                    const std::string logDirPath = controller_config_doc["log_directory_path"].as<std::string>();

                    controllerInfo.domainNames = domain_names;
                    controllerInfo.numOfDomains = (uint)domain_names.size();
                    controllerInfo.logDirPath = logDirPath;
                }
                std::cout << "Passed controller config\n";
                return controllerInfo;
                
            }

            std::optional<std::vector<StartupInfo>> parse_startup_configs(std::string_view config_file_path)
            {   
                std::vector<StartupInfo> startupInfo;

                if(auto startup_config_doc = [&config_file_path](){
                    const auto allDocs = YAML::LoadAllFromFile(static_cast<std::string>(config_file_path));
                    YAML::Node startupNode;
                    for(const auto& doc : allDocs)
                    {
                        if(const auto tempNode = doc["startup_config"])
                        {   
                            startupNode = tempNode;
                        }
                        else
                            continue;
                    }

                    return startupNode;
                }())
                {
                    
                    for(const auto& startup_config : startup_config_doc)
                    {
                        StartupInfo tempSI;
                        
                        tempSI.slaveName = startup_config["slave_name"].as<std::string>();
                        tempSI.sdoInfo.first = (uint16_t)startup_config["sdo_index"].as<uint16_t>();
                        tempSI.sdoInfo.second = (uint8_t)startup_config["sdo_subindex"].as<uint16_t>();

                        const std::string dataTypeStr = startup_config["data_type"].as<std::string>();
                        
                        if(const auto data_val = startup_config["data_value"])
                        {
                            tempSI.deduceDataType(dataTypeStr, data_val);
                            tempSI.configType = SdoConfigType::WRITE;
                        }
                        else
                        {
                            tempSI.deduceDataType(dataTypeStr);
                            tempSI.configType = SdoConfigType::READ;
                        }

                        startupInfo.emplace_back(std::move(tempSI));
                    }
                }
                else
                {
                    return std::nullopt;
                }
                
                return startupInfo;
            }

        }

        std::vector<std::optional<int>> detect_null_diffs(const std::vector<std::string>& diffs_with_nulls)
        {
            std::vector<std::optional<int>> newDiffs;
            newDiffs.resize(diffs_with_nulls.size());

            for(std::size_t i = 0; i < diffs_with_nulls.size(); i++)
            {
                if(diffs_with_nulls[i] == "NULL" || diffs_with_nulls[i] == "null")
                {
                    newDiffs[i] = std::nullopt;
                }
                else
                {
                    newDiffs[i] = std::stoi(diffs_with_nulls[i]);
                }
            }

            return newDiffs;
        }

        std::vector<ec_direction_t> intToEcDirectionEnum(const std::vector<int> &directions)
        {
            std::vector<ec_direction_t> dirs;
            
            for(std::size_t i = 0; i < directions.size(); i++)
            {
                switch (directions[i])
                {
                case 0:
                    dirs.emplace_back(EC_DIR_INVALID);
                    break;
                case 1:
                    dirs.emplace_back(EC_DIR_OUTPUT);
                    break;
                case 2:
                    dirs.emplace_back(EC_DIR_INPUT);
                    break;
                case 3:
                    dirs.emplace_back(EC_DIR_COUNT);
                    break;
                default:
                    dirs.emplace_back(EC_DIR_INVALID);
                    break;
                }
            }

            return dirs;
        }

        std::vector<ec_watchdog_mode_t> intToEcWatchdogEnum(const std::vector<int> &watchdog_modes)
        {
            std::vector<ec_watchdog_mode_t> watchdogModes;
            
            for(std::size_t i = 0; i < watchdog_modes.size(); i++)
            {
                switch (watchdog_modes[i])
                {
                case 0:
                    watchdogModes.emplace_back(EC_WD_DEFAULT);
                    break;
                case 1:
                    watchdogModes.emplace_back(EC_WD_ENABLE);
                    break;
                case 2:
                    watchdogModes.emplace_back(EC_WD_DISABLE);
                    break;
                default:
                    watchdogModes.emplace_back(EC_WD_DEFAULT);
                    break;
                }
            }

            return watchdogModes;
        }

        toHexaHelper::toHexaHelper()
        {   
            uint8_t count = 0x00;
            for(uint8_t i = 0; i <= 255; i++)
            {
                charHexMap[static_cast<char>(i)] = count;

                count += 0x01;
                
            }
        }

        std::vector<uint8_t> toHexadecimal(const std::vector<uint>& to_fix)
        {
            std::vector<uint8_t> hexas;

            
        
            // This is bad practice.
            for(std::size_t i = 0; i < to_fix.size(); i++)
            {                
                switch (to_fix[i])
                {
                case 0:
                    hexas.emplace_back(0x00);
                    break;
                case 1:
                    hexas.emplace_back(0x01);
                    break;
                case 2:
                    hexas.emplace_back(0x02);
                    break;
                case 3:
                    hexas.emplace_back(0x03);
                    break;
                case 4:
                    hexas.emplace_back(0x04);
                    break;
                case 5:
                    hexas.emplace_back(0x05);
                    break;
                case 6:
                    hexas.emplace_back(0x06);
                    break;
                case 7:
                    hexas.emplace_back(0x07);
                    break;
                case 8:
                    hexas.emplace_back(0x08);
                    break;
                case 9:
                    hexas.emplace_back(0x09);
                    break;
                case 10:
                    hexas.emplace_back(0x0A);
                    break;
                case 11:
                    hexas.emplace_back(0x0B);
                    break;
                case 12:
                    hexas.emplace_back(0x0C);
                    break;
                case 13:
                    hexas.emplace_back(0x0D);
                    break;
                case 14:
                    hexas.emplace_back(0x0E);
                    break;
                case 15:
                    hexas.emplace_back(0x0F);
                    break;
                case 16:
                    hexas.emplace_back(0x10);
                    break;
                case 17:
                    hexas.emplace_back(0x11);
                    break;
                case 18:
                    hexas.emplace_back(0x12);
                    break;
                case 19:
                    hexas.emplace_back(0x13);
                    break;
                case 20:
                    hexas.emplace_back(0x14);
                    break;
                case 21:
                    hexas.emplace_back(0x15);
                    break;
                case 22:
                    hexas.emplace_back(0x16);
                    break;
                case 23:
                    hexas.emplace_back(0x17);
                    break;
                case 24:
                    hexas.emplace_back(0x18);
                    break;
                case 25:
                    hexas.emplace_back(0x19);
                    break;
                case 26:
                    hexas.emplace_back(0x1A);
                    break;
                case 27:
                    hexas.emplace_back(0x1B);
                    break;
                case 28:
                    hexas.emplace_back(0x1C);
                    break;
                case 29:
                    hexas.emplace_back(0x1D);
                    break;
                case 30:
                    hexas.emplace_back(0x1E);
                    break;
                case 31:
                    hexas.emplace_back(0x1F);
                    break;
                default:
                    break;
                }

            }
            
            
            return hexas;
        }

        std::vector<uint8_t> fitSubindexes(const std::vector<uint>& to_fix)
        {

            std::vector<uint8_t> output;
            for (auto i : to_fix) {
                uint8_t hex = 0x00;
                if (i <= 0xFF) {
                    hex = static_cast<uint8_t>(i);
                }
                output.push_back(hex);
            }
            return output;
        }

        SlaveInfo parse_config_file(
            const std::string& file_name,
            const std::string& slave_name
        )
        {

            SlaveInfo info;

            try{

                YAML::Node config_file = YAML::LoadFile(file_name);

                YAML::Node slave_config  = config_file[slave_name];

                info.slaveName = slave_name;
                const std::string typeStr = slave_config["type"].as<std::string>();
                if(typeStr == "coupler" || typeStr == "Coupler")
                {
                    info.slaveType = SlaveType::Coupler;
                    return info;
                }
                else if(typeStr == "driver" || typeStr == "Driver")
                {
                    info.slaveType = SlaveType::Driver;
                }

                info.vendorID = slave_config["vendor_id"].as<int>();
                info.productCode = slave_config["product_id"].as<int>();
                info.position = slave_config["slave_position"].as<uint>();
                info.alias = slave_config["slave_alias"].as<uint>();
                
                info.pdoEntryInfo.indexes = slave_config["pdo_entry_info"]["indexes"].as<std::vector<uint16_t>>();
                info.pdoEntryInfo.subindexes = toHexadecimal(slave_config["pdo_entry_info"]["subindexes"].as<std::vector<uint>>());
                
                info.pdoEntryInfo.bitLengths = slave_config["pdo_entry_info"]["bit_lengths"].as<std::vector<uint16_t>>();
                
                info.ioMappingInfo.RxPDO_Address = slave_config["pdo_entry_info"]["rxpdo_address"].as<uint16_t>();
                info.ioMappingInfo.TxPDO_Address = slave_config["pdo_entry_info"]["txpdo_address"].as<uint16_t>();
                info.ioMappingInfo.RxPDO_Size = slave_config["pdo_entry_info"]["rxpdo_size"].as<unsigned int>();
                info.ioMappingInfo.TxPDO_Size = slave_config["pdo_entry_info"]["txpdo_size"].as<unsigned int>();
                info.ioMappingInfo.RxPDO_Indexes = slave_config["pdo_entry_info"]["rxpdo_indexes"].as<std::vector<uint16_t>>();
                info.ioMappingInfo.TxPDO_Indexes = slave_config["pdo_entry_info"]["txpdo_indexes"].as<std::vector<uint16_t>>();

                info.slaveSyncInfo.numSyncManagers = static_cast<std::size_t>(slave_config["slave_sync_info"]["num_sync_managers"].as<int>());
                info.slaveSyncInfo.syncManagerDirections = slave_config["slave_sync_info"]["sync_manager_directions"].as<std::vector<int>>();
                info.slaveSyncInfo.numPDOs = slave_config["slave_sync_info"]["number_of_pdos"].as<std::vector<uint>>();
                const std::vector<std::string> tempPdoDiffs = slave_config["slave_sync_info"]["pdo_index_diff"].as<std::vector<std::string>>();
                info.slaveSyncInfo.pdoIndexDiff = detect_null_diffs(tempPdoDiffs);
                info.slaveSyncInfo.watchdogModes = slave_config["slave_sync_info"]["watchdog_mode"].as<std::vector<int>>();

            }catch(const YAML::BadFile& ex)
            {
                
                std::cout << ex.what();
            }
            catch(const YAML::TypedBadConversion<unsigned short>& ex)
            {
                info.toString();
            }

            return info;
        }

        std::optional<std::vector<SlaveInfo>> parse_config_file(std::string_view file_name)
        {   
            std::vector<SlaveInfo> slaveConfigs;
            try{

                auto config_docs = YAML::LoadAllFromFile(static_cast<std::string>(file_name));
                //std::cout << config_docs[1]["slave_name"].as<std::string>() << std::endl;
                
                if(config_docs.empty())
                {
                    return std::nullopt;
                }

                for(const auto& doc : config_docs)
                {
                    if(doc["controller_config"] || doc["startup_config"])
                    {
                        std::cout << "controller or startup config found skipping" << std::endl;
                        continue;
                    }

                    SlaveInfo conf;
                    // Get slave configuration information from the YAML document
                    conf.slaveName = doc["slave_name"].as<std::string>();
                    const std::string typeStr = doc["slave_type"].as<std::string>();
                    conf.vendorID = doc["vendor_id"].as<int>();
                    conf.productCode = doc["product_id"].as<int>();
                    conf.position = doc["slave_position"].as<uint>();
                    conf.alias = doc["slave_alias"].as<uint>();

                    if(typeStr == "coupler" || typeStr == "Coupler")
                    {
                        conf.slaveType = SlaveType::Coupler;
                        slaveConfigs.push_back(conf);
                        continue;
                    }
                    else if(typeStr == "driver" || typeStr == "Driver")
                    {
                        conf.slaveType = SlaveType::Driver;
                    }
                    else if(typeStr == "io" || typeStr == "IO" || typeStr == "Io")
                    {
                        conf.slaveType = SlaveType::IO;
                    }

                    conf.pdoNames = doc["pdo_names"].as<std::vector<std::string>>();

                    conf.domainName = doc["domain_name"].as<std::string>();

                    conf.pdoEntryInfo.indexes = doc["pdo_entry_info"]["indexes"].as<std::vector<uint16_t>>();
                    conf.pdoEntryInfo.subindexes = toHexadecimal(doc["pdo_entry_info"]["subindexes"].as<std::vector<uint>>());
                    conf.pdoEntryInfo.bitLengths = doc["pdo_entry_info"]["bit_lengths"].as<std::vector<uint16_t>>();
                
                    conf.ioMappingInfo.RxPDO_Address = doc["pdo_entry_info"]["rxpdo_address"].as<uint16_t>();
                    conf.ioMappingInfo.TxPDO_Address = doc["pdo_entry_info"]["txpdo_address"].as<uint16_t>();
                    conf.ioMappingInfo.RxPDO_Size = doc["pdo_entry_info"]["rxpdo_size"].as<unsigned int>();
                    conf.ioMappingInfo.TxPDO_Size = doc["pdo_entry_info"]["txpdo_size"].as<unsigned int>();
                    conf.ioMappingInfo.RxPDO_Indexes = doc["pdo_entry_info"]["rxpdo_indexes"].as<std::vector<uint16_t>>();
                    conf.ioMappingInfo.TxPDO_Indexes = doc["pdo_entry_info"]["txpdo_indexes"].as<std::vector<uint16_t>>();

                    conf.slaveSyncInfo.numSyncManagers = static_cast<std::size_t>(doc["slave_sync_info"]["num_sync_managers"].as<int>());
                    conf.slaveSyncInfo.syncManagerDirections = doc["slave_sync_info"]["sync_manager_directions"].as<std::vector<int>>();
                    conf.slaveSyncInfo.numPDOs = doc["slave_sync_info"]["number_of_pdos"].as<std::vector<uint>>();
                    const std::vector<std::string> tempPdoDiffs = doc["slave_sync_info"]["pdo_index_diff"].as<std::vector<std::string>>();
                    conf.slaveSyncInfo.pdoIndexDiff = detect_null_diffs(tempPdoDiffs);
                    conf.slaveSyncInfo.watchdogModes = doc["slave_sync_info"]["watchdog_mode"].as<std::vector<int>>();
                    
                    if(const auto dcConfig = doc["dc_info"])
                    {
                        DC_Info tempDcInfo;
                        tempDcInfo.assign_activate = dcConfig["assign_activate"].as<uint16_t>();
                        tempDcInfo.sync0_cycle = dcConfig["sync0_cycle"].as<uint32_t>();
                        tempDcInfo.sync0_shift = dcConfig["sync0_shift"].as<int32_t>();
                        tempDcInfo.sync1_cycle = dcConfig["sync1_cycle"].as<uint32_t>();
                        tempDcInfo.sync1_shift = dcConfig["sync1_shift"].as<int32_t>();

                        conf.dcInfo.emplace<DC_Info>(tempDcInfo);
                    }
                    else
                    {
                        conf.dcInfo = false;
                    }

                    // If only one slave should be created from the current slave information
                    // Add conf to the config vector and continue to loop over the remaining documents.
                    if(doc["num_of_slaves"].as<uint>() == 1)
                    {
                        slaveConfigs.push_back(conf);
                        continue;
                    }

                    uint numOfSlaves = doc["num_of_slaves"].as<uint>();
                    
                    for(uint i = 0; i < numOfSlaves; i++)
                    {
                        SlaveInfo tempConf = conf;
                        tempConf.slaveName = conf.slaveName + "_" + std::to_string(i);
                        tempConf.position += i;

                        slaveConfigs.push_back(tempConf); 
                    }    
                
                }
            }
            catch(const YAML::BadFile& ex)
            {
                std::cout << ex.what();
            }

            return slaveConfigs;
        }

        DC_Info getDcInfo(
            const std::string& file_name,
            const std::string& slave_name
        )
        {

            DC_Info dcInfo;

            try
            {
                
                YAML::Node configFile = YAML::LoadFile(file_name);
                YAML::Node slaveConfig = configFile[slave_name];
                YAML::Node dcConfig = slaveConfig["dc_info"];

                dcInfo.assign_activate = dcConfig["assign_activate"].as<uint16_t>();
                dcInfo.sync0_cycle = dcConfig["sync0_cycle"].as<uint32_t>();
                dcInfo.sync0_shift = dcConfig["sync0_shift"].as<int32_t>();
                dcInfo.sync1_cycle = dcConfig["sync1_cycle"].as<uint32_t>();
                dcInfo.sync1_shift = dcConfig["sync1_shift"].as<int32_t>();

            }
            catch(const YAML::BadFile& ex)
            {
                std::cout << ex.what();
            }

            return dcInfo;
        }

        namespace debug
        {
            
            void TimeMeasureInfo_s::updateMinMax()
            {
                if (latency_ns > latency_max_ns) {
                    latency_max_ns = latency_ns;
                }
                if (latency_ns < latency_min_ns) {
                    latency_min_ns = latency_ns;
                }
                if (period_ns > period_max_ns) {
                    period_max_ns = period_ns;
                }
                if (period_ns < period_min_ns) {
                    period_min_ns = period_ns;
                }
                if (exec_ns > exec_max_ns) {
                    exec_max_ns = exec_ns;
                }
                if (exec_ns < exec_min_ns) {
                    exec_min_ns = exec_ns;
                }
            }

            void TimeMeasureInfo_s::resetMinMax()
            {
                period_max_ns = 0;
                period_min_ns = 0xffffffff;
                exec_max_ns = 0;
                exec_min_ns = 0xffffffff;
                latency_max_ns = 0;
                latency_min_ns = 0xffffffff;
            }

            std::string TimeMeasureInfo_s::getTimingStats()
            {
                //std::cout.precision(10);
                std::string str;
                str += "Period: " + std::to_string(period_min_ns) + " ... " + std::to_string(period_max_ns) + "\n";
                str += "Exec: " + std::to_string(exec_min_ns) + " ... " + std::to_string(exec_max_ns) + "\n";
                str += "Latency: " + std::to_string(latency_min_ns) + " ... " + std::to_string(latency_max_ns) + "\n";

                return str;
            }

            void TimeMeasureInfo_s::updateEndTime()
            {
                clock_gettime(usedClock, &endTime);   
            }

            void measureTime(TimeMeasureInfo_s& tmInfo_s, timespec wakeupTime)
            {
                clock_gettime(tmInfo_s.usedClock, &tmInfo_s.startTime);
                tmInfo_s.latency_ns = timeDiff(wakeupTime, tmInfo_s.startTime);
                tmInfo_s.period_ns = timeDiff(tmInfo_s.lastStartTime, tmInfo_s.startTime);
                tmInfo_s.exec_ns = timeDiff(tmInfo_s.lastStartTime, tmInfo_s.endTime);
                tmInfo_s.lastStartTime = tmInfo_s.startTime;
                
                tmInfo_s.updateMinMax();
            }

            long timeDiff(timespec t1, timespec t2)
            {
                auto diff = ((t2.tv_sec - t1.tv_sec) * NANOSEC_PER_SEC + (t2.tv_nsec - t1.tv_nsec));

                return diff;
            }
        }
    }

    PdoEntryInfo::PdoEntryInfo()
    {
        indexes = std::vector<uint16_t>();
        subindexes = std::vector<uint8_t>();
        bitLengths = std::vector<uint16_t>();
    }
    IoMappingInfo::IoMappingInfo()
    {
        RxPDO_Address = 0;
        TxPDO_Address = 0;
        RxPDO_Size = 0;
        TxPDO_Size = 0;
        RxPDO_Indexes = std::vector<uint16_t>();
        TxPDO_Indexes = std::vector<uint16_t>();
    }
    SlaveSyncInfo::SlaveSyncInfo()
    {
        numSyncManagers = 0;
        syncManagerDirections = std::vector<int>();
        numPDOs = std::vector<uint>();
        pdoIndexDiff = std::vector<std::optional<int>>();
        watchdogModes = std::vector<int>();
    }
    
    SlaveInfo::SlaveInfo()
    {
        slaveName = std::string();
        vendorID = 0;
        productCode = 0;
        position = 0;
        alias = 0;
        pdoEntryInfo = PdoEntryInfo();
        ioMappingInfo = IoMappingInfo();
        slaveSyncInfo = SlaveSyncInfo();
    }
    DC_Info::DC_Info()
    {
        assign_activate = 0;
        sync0_cycle = 0;
        sync0_shift = 0;
        sync1_cycle = 0;
        sync1_shift = 0;
    }

    void StartupInfo::deduceDataType(const std::string& data_type_str)
        {
            auto res = std::find_if(
                 EC_TYPE_STRING_PAIRS.begin(),
                 EC_TYPE_STRING_PAIRS.end(),
                 [&data_type_str](const std::pair<std::string, EC_Type>& p)
                 {
                     return (p.first == data_type_str);
                 }
            );

            if(res == EC_TYPE_STRING_PAIRS.end())
            {
                 return;
            }

            const EC_Type currType = res->second;

            // If the SDO is to be read
            // Change the variant type to the data_type in the configuration file.
    
            switch (currType)
            {
            case EC_Type::UINT8:
                data = uint8_t();
                break;
            case EC_Type::UINT16:
                data = uint16_t();
                break;
            case EC_Type::UINT32:
                data = uint32_t();
                break;
            case EC_Type::UINT64:
                data = uint64_t();
                break;
            default:
                return;
            }
        }

        void StartupInfo::deduceDataType(
            const std::string& data_type_str,
            const YAML::Node& data_node
        )
        {
            auto res = std::find_if(
                 EC_TYPE_STRING_PAIRS.begin(),
                 EC_TYPE_STRING_PAIRS.end(),
                 [&data_type_str](const std::pair<std::string, EC_Type>& p)
                 {
                     return (p.first == data_type_str);
                 }
            );

            if(res == EC_TYPE_STRING_PAIRS.end())
            {
                 return;
            }
            
            const EC_Type currType = res->second;

            switch (currType)
            {
            case EC_Type::UINT8:
                this->data = (uint8_t)data_node.as<uint16_t>();
                break;
            case EC_Type::UINT16:
                this->data = data_node.as<uint16_t>();
                break;
            case EC_Type::UINT32:
                this->data = data_node.as<uint32_t>();
                break;
            case EC_Type::UINT64:
                this->data = data_node.as<uint64_t>();
                break;
            default:
                return;
            }

        }
}
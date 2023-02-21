#include "../include/utilities.hpp"


namespace ethercat_interface
{
    namespace utilities
    {
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

                info.vendorID = slave_config["vendor_id"].as<int>();

                info.productCode = slave_config["product_id"].as<int>();

                info.position = slave_config["slave_position"].as<uint>();

                info.alias = slave_config["slave_alias"].as<uint>();

                info.pdoEntryInfo.indexes = slave_config["pdo_entry_info"]["indexes"].as<std::vector<uint32_t>>();

                info.pdoEntryInfo.subindexes = slave_config["pdo_entry_info"]["subindexes"].as<std::vector<uint8_t>>();

                info.pdoEntryInfo.bitLengths = slave_config["pdo_entry_info"]["bit_lengths"].as<std::vector<uint16_t>>();
                
                info.slaveSyncInfo.numSyncManagers = static_cast<std::size_t>(slave_config["slave_sync_info"]["num_sync_managers"].as<int>());

                info.slaveSyncInfo.syncManagerDirections = slave_config["slave_sync_info"]["sync_manager_directions"].as<std::vector<int>>();

                info.slaveSyncInfo.numPDOs = slave_config["slave_sync_info"]["number_of_pdos"].as<std::vector<int>>();
                
                const std::vector<std::string> tempPdoDiffs = slave_config["slave_sync_info"]["pdo_index_diff"].as<std::vector<std::string>>();
                info.slaveSyncInfo.pdoIndexDiff = detect_null_diffs(tempPdoDiffs);

                info.slaveSyncInfo.watchdogModes = slave_config["slave_sync_info"]["watchdog_mode"].as<std::vector<int>>();

            }catch(const YAML::BadFile& ex)
            {
                std::cout << ex.what();
            }

            return info;
        }
    }
}
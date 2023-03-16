/**
 * @file generator.cpp
 * @author Eren Naci Odabaşı
 * @brief Creates a custom Offset class to use with an EtherCAT slave.
 * @version 0.1
 * @date 2023-03-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <yaml-cpp/yaml.h>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

/**
 * @brief 
 * 
 * @param argc: Argument count should be 2. 
 * @param argv: Argument 1: Name of the slave.
 *              Argument 2: Path of the config file to read the PDO names from.
 *              Arguemnt 3: Path where the created custom offset file (type .hpp) should be saved.
 * @return int 
 */

int main(int argc, char** argv)
{
    if(argc != 4)
    {   
        std::cout << "Not enough arguments. Provided args: " << argc - 1 << ", expected 3" << std::endl;
        return 0;
    }

    const std::string slaveName = argv[1];
    const std::string configFileName = argv[2]; 
    const std::string pathToSave = argv[3];

    std::cout << "Slave " << slaveName << " in config file " << configFileName << " offset will be created in " << pathToSave << std::endl;

    std::vector<std::string>* pdoNames = nullptr;

    try
    {
        YAML::Node config_file = YAML::LoadFile(configFileName);
        YAML::Node slave_config = config_file[slaveName];
        //auto tempVec = slave_config["generator_info"].as<std::vector<std::string>>();
        pdoNames = new std::vector<std::string>(slave_config["generator_info"].as<std::vector<std::string>>());
    }
    catch(const YAML::BadFile& ex)
    {
        std::cout << ex.what();
        return 0;
    }

    if(pdoNames == nullptr)
    {
        return 0;
    }    
    
    std::ofstream newFile;
    newFile.open(pathToSave);
    const std::string className = slaveName + "_Offset";

    newFile << "#include \"offset.hpp\" \n";
    newFile << "class " << slaveName << "_Offset" << " : public ethercat_interface::Offset\n" << "{\n";
    newFile << "\t public:\n";
    newFile << "\t" << className << "();\n";
    newFile << '\t' << "struct offset\n" << "\t{\n";
    for(auto name : *pdoNames)
    {
        newFile << "\t\t" << "uint " << name << ";\n";
    }
    newFile << "\t};\n";
    newFile << "\toffset m_Data;\n";
    newFile << "\tvoid setData();\n";
    newFile << "\tvoid setData(void* data)override;\n";
    newFile << "\tvoid init_OffsetMap() override;\n";
    newFile << "};\n";

    newFile << className << "::" << className << "()\n{\n";
    newFile << "\tinit_OffsetMap();\n";
    newFile << "\tm_NumOffsets = m_OffsetNameIndexes.size();\n}\n";

    newFile << "void " << className << "::setData(void* data)\n{\n";
    newFile << "\tm_Data = *(offset*)data;\n}\n";

    newFile << "void " << className << "::init_OffsetMap()\n{\n";

    for(auto name : *pdoNames)
    {
        newFile << "\tm_OffsetNameIndexMap[\"" << name << "\"] = &m_Data." << name << ";\n";
    }

    newFile << "}\n";

    newFile.close();

    if(pdoNames)
        delete pdoNames;

    return 0;
}
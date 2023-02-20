
#include "../include/offset.hpp"

#include <iostream>
#include <filesystem>
class ServoOffset : public ethercat_interface::Offset
{
    public:

	ServoOffset();

    struct offset
    {
        uint ctrl_word;
	    uint target_velocity;
	    uint velocity_offset;
	    uint operation_mode;
	    uint digital_output;
	    uint status_word;
	    uint current_position;
	    uint current_velocity;
	    uint current_torque;
	    uint mode_display;
        uint digital_input; 
    };

    offset m_Data;

	void setData();

    void setData(void* data) override;

	//unsigned int* getData(const std::string& offset_name) override;

	void init_OffsetMap();


};

ServoOffset::ServoOffset()
{
	init_OffsetMap();

	m_NumOffsets = m_OffsetNameIndexes.size();
}

void ServoOffset::setData(void* data)
{
	m_Data = *(offset*)data;

}

/* unsigned int* ServoOffset::getData(const std::string& offset_name)
{
	auto found = m_OffsetNameIndexMap.find(offset_name);
	if(found == m_OffsetNameIndexMap.end())
	{
		return nullptr;
	}

	return found->second;
} */

void ServoOffset::init_OffsetMap()
{
	m_OffsetNameIndexMap["ctrl_word"] = &m_Data.ctrl_word;
	m_OffsetNameIndexes.emplace_back("ctrl_word");

	m_OffsetNameIndexMap["target_velocity"] = &m_Data.target_velocity;
	m_OffsetNameIndexes.emplace_back("target_velocity");
	
	m_OffsetNameIndexMap["velocity_offset"] = &m_Data.velocity_offset;
	m_OffsetNameIndexes.emplace_back("velocity_offset");

	m_OffsetNameIndexMap["operation_mode"] = &m_Data.operation_mode;
	m_OffsetNameIndexes.emplace_back("operation_mode");

	m_OffsetNameIndexMap["digital_output"] = &m_Data.digital_output;
	m_OffsetNameIndexes.emplace_back("digital_output");
	
	m_OffsetNameIndexMap["status_word"] = &m_Data.status_word;
	m_OffsetNameIndexes.emplace_back("status_word");
	
	m_OffsetNameIndexMap["current_position"] = &m_Data.current_position;
	m_OffsetNameIndexes.emplace_back("current_position");
	
	m_OffsetNameIndexMap["current_velocity"] = &m_Data.current_velocity;
	m_OffsetNameIndexes.emplace_back("current_velocity");
	
	m_OffsetNameIndexMap["current_torque"] = &m_Data.current_torque;
	m_OffsetNameIndexes.emplace_back("current_torque");
	
	m_OffsetNameIndexMap["mode_display"] = &m_Data.mode_display;
	m_OffsetNameIndexes.emplace_back("mode_display");
	
	m_OffsetNameIndexMap["digital_input"] = &m_Data.digital_input;
	m_OffsetNameIndexes.emplace_back("digital_input");
}


void register_domains(ethercat_interface::Offset* offsetservo)
{
	for(std::size_t i = 0; i < offsetservo->getNumberOfOffsets(); i++)
	{
		std::cout << offsetservo->getData(offsetservo->m_OffsetNameIndexes[i]) << std::endl;
	}
}

class Slave
{
	public:
	Slave(ethercat_interface::Offset* ofs)
	{
		offset = ofs;
	}
	ethercat_interface::Offset* offset;
};

int main()
{
	/* ethercat_interface::Offset* s;
	s = new ServoOffset();

	register_domains(s);

	Slave slave(s);

	register_domains(slave.offset); */

	std::cout << std::filesystem::current_path() << std::endl; 

	return 0;
}
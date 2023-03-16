#include "../../include/offset.hpp"

class EL7221_9014_Offset : public ethercat_interface::Offset
{
    public:

    EL7221_9014_Offset();

    struct offset
    {
        uint ctrl_word;
        uint target_velocity;
        uint op_mode;
        uint current_position;
        uint status_word;
        uint current_velocity;
        uint current_torque;
        uint op_mode_display;
    };

    offset m_Data;

    void setData();
    void setData(void* data) override;

    void init_OffsetMap() override;
};

EL7221_9014_Offset::EL7221_9014_Offset()
{
    init_OffsetMap();

    m_NumOffsets = m_OffsetNameIndexes.size();
}

void EL7221_9014_Offset::setData(void* data)
{
    m_Data = *(offset*)data;
}

void EL7221_9014_Offset::init_OffsetMap()
{
    m_OffsetNameIndexMap["ctrl_word"] = &m_Data.ctrl_word;
	m_OffsetNameIndexes.emplace_back("ctrl_word");

	m_OffsetNameIndexMap["target_velocity"] = &m_Data.target_velocity;
	m_OffsetNameIndexes.emplace_back("target_velocity");
	
    m_OffsetNameIndexMap["op_mode"] = &m_Data.op_mode;
    m_OffsetNameIndexes.emplace_back("op_mode");

    m_OffsetNameIndexMap["status_word"] = &m_Data.status_word;
	m_OffsetNameIndexes.emplace_back("status_word");
	
    m_OffsetNameIndexMap["current_position"] = &m_Data.current_position;
	m_OffsetNameIndexes.emplace_back("current_position");
	
	m_OffsetNameIndexMap["current_velocity"] = &m_Data.current_velocity;
	m_OffsetNameIndexes.emplace_back("current_velocity");

    m_OffsetNameIndexMap["current_torque"] = &m_Data.current_torque;
	m_OffsetNameIndexes.emplace_back("current_torque");

    m_OffsetNameIndexMap["op_mode_display"] = &m_Data.op_mode_display;
    m_OffsetNameIndexes.emplace_back("op_mode_display");

}
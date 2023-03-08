#ifndef OFFSET_EXAMPLE_HPP
#define OFFSET_EXAMPLE_HPP

#include "../../include/offset.hpp"

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

#endif //OFFSET_EXAMPLE_HPP
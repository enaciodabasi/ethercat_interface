#include "ecrt.h"
#include <iostream>
#include <vector>
#include <thread>
static ec_master_t* master;
static ec_domain_t* domain;
static ec_slave_config_t *sc;
static uint8_t* domain_process_data;








/* static ec_pdo_info_t pdo_out[] ={
	{0x1600}
};
static ec_pdo_info_t pdo_in[] = {
	{0x1A03}
}; */


//#define PLC_POS 0, 0
//#define PLC 0x01000056, 0x00002301
//static ec_pdo_entry_info_t input_channel[] = {
//	{0x2001, 0x01, 80},
//	{0x2001, 0x02, 80},
//	{0x2001, 0x03, 80},
//	{0x2001, 0x04, 80},
//	{0x2001, 0x05, 80}
//};
//
//static ec_pdo_entry_info_t output_channel[] = {
//	{0x10f3, 0x04, 8},
//	{0x2000, 0x01, 80},
//	{0x2000, 0x02, 80},
//	{0x2000, 0x03, 80},
//	{0x2000, 0x04, 80},
//	{0x2000, 0x05, 80}
//	
//};
//
//static ec_pdo_info_t pdo_out[] ={
//	{0x1600, 5, input_channel}
//}; 
//
//static ec_pdo_info_t pdo_in[] = {
//	{0x1A04, 6, output_channel}
//};
//
//
//static ec_sync_info_t syncs[] = {
//	{0, EC_DIR_OUTPUT, 0, NULL},
//	{1, EC_DIR_INPUT, 0, NULL},
//	{2, EC_DIR_OUTPUT, 1, pdo_out},
//    {3, EC_DIR_INPUT, 1, pdo_in},
//	{0xff}
//};
//
//static uint out_arr_offset = 0;
//static uint diag_offset = 0;
//static uint in_arr_offset1 = 0;
//static uint in_arr_offset2 = 0;
//static uint in_arr_offset3 = 0;
//static uint in_arr_offset4 = 0;
//static uint in_arr_offset5 = 0;
//
//static ec_pdo_entry_reg_t regs[] = {
//	{PLC_POS, PLC, 0x2001, 0x01, &out_arr_offset},
//	{PLC_POS, PLC, 0X10f3, 0x04, &diag_offset},
//	{PLC_POS, PLC, 0x2000, 0x01, &in_arr_offset1},
///* 	{PLC_POS, PLC, 0x2000, 0x02, &in_arr_offset2},
//	{PLC_POS, PLC, 0x2000, 0x03, &in_arr_offset3},
//	{PLC_POS, PLC, 0x2000, 0x04, &in_arr_offset4}, */
//	{}
//};

static struct
{
	uint ctrl_word;
	uint op_mode;
	uint target_torque;
	uint target_pos;
	uint target_vel;
	uint torque_offset;
	uint tuning_cmd;
	uint physical_outputs;
	uint bitmask;
	uint mosi;
	uint velocity_offset;
	uint statusword;
	uint disp;
	uint position_val;
	uint velocity_val;
	uint torque_val;
	uint a1;
	uint a2;
	uint a3;
	uint a4;
	uint ts;
	uint di;
	uint miso2;
	uint time;
	uint position_demand;
	uint vel_demand;
	uint torque_demand;
}offset;
const static ec_pdo_entry_reg_t domain_regs[] = {
	{0, 0, 0x000022d2 , 0x00000201 ,0x6040, 0x00, &offset.ctrl_word}, /* Controlword */
    {0, 0, 0x000022d2 , 0x00000201 ,0x6060, 0x00, &offset.op_mode}, /* Modes of operation */
    {0, 0, 0x000022d2 , 0x00000201 ,0x6071, 0x00, &offset.target_torque}, /* Target Torque */
    {0, 0, 0x000022d2 , 0x00000201 ,0x607a, 0x00, &offset.target_pos}, /* Target position */
    {0, 0, 0x000022d2 , 0x00000201 ,0x60ff, 0x00, &offset.target_vel}, /* Target velocity */
    {0, 0, 0x000022d2 , 0x00000201 ,0x60b2, 0x00, &offset.target_torque}, /* Torque offset */
    {0, 0, 0x000022d2 , 0x00000201 ,0x2701, 0x00, &offset.tuning_cmd}, /* Tuning command */
    {0, 0, 0x000022d2 , 0x00000201 ,0x60fe, 0x01, &offset.physical_outputs}, /* Physical outputs */
    {0, 0, 0x000022d2 , 0x00000201 ,0x60fe, 0x02, &offset.bitmask}, /* Bit mask */
    {0, 0, 0x000022d2 , 0x00000201 ,0x2703, 0x00, &offset.mosi}, /* User MOSI */
    {0, 0, 0x000022d2 , 0x00000201 ,0x60b1, 0x00, &offset.velocity_offset}, /* Velocity offset */
    {0, 0, 0x000022d2 , 0x00000201 ,0x6041, 0x00, &offset.statusword}, /* Statusword */
    {0, 0, 0x000022d2 , 0x00000201 ,0x6061, 0x00, &offset.disp}, /* Modes of operation display */
    {0, 0, 0x000022d2 , 0x00000201 ,0x6064, 0x00, &offset.position_val}, /* Position actual value */
    {0, 0, 0x000022d2 , 0x00000201 ,0x606c, 0x00, &offset.velocity_val}, /* Velocity actual value */
    {0, 0, 0x000022d2 , 0x00000201 ,0x6077, 0x00, &offset.torque_val}, /* Torque actual value */
    {0, 0, 0x000022d2 , 0x00000201 ,0x2401, 0x00, &offset.a1}, /* Analog input 1 */
    {0, 0, 0x000022d2 , 0x00000201 ,0x2402, 0x00, &offset.a2}, /* Analog input 2 */
    {0, 0, 0x000022d2 , 0x00000201 ,0x2403, 0x00, &offset.a3}, /* Analog input 3 */
    {0, 0, 0x000022d2 , 0x00000201 ,0x2404, 0x00, &offset.a4}, /* Analog input 4 */
    {0, 0, 0x000022d2 , 0x00000201 ,0x2702, 0x00, &offset.ts}, /* Tuning status */
    {0, 0, 0x000022d2 , 0x00000201 ,0x60fd, 0x00, &offset.di}, /* Digital inputs */
    {0, 0, 0x000022d2 , 0x00000201 ,0x2704, 0x00, &offset.miso2}, /* User MISO */
    {0, 0, 0x000022d2 , 0x00000201 ,0x20f0, 0x00, &offset.time}, /* Timestamp */
    {0, 0, 0x000022d2 , 0x00000201 ,0x60fc, 0x00, &offset.position_demand}, /* Position demand internal value */
    {0, 0, 0x000022d2 , 0x00000201 ,0x606b, 0x00, &offset.vel_demand}, /* Velocity demand value */
    {0, 0, 0x000022d2 , 0x00000201 ,0x6074, 0x00, &offset.torque_demand}, /* Torque demand */
	{}
};

int main(int argc, char**argv)
{
	
	master = ecrt_request_master(0);
	if(!master)
		return 1;
	
	if(!(domain = ecrt_master_create_domain(master)))
	{
		return 1;
	}

	if(!(sc = ecrt_master_slave_config(master, 0, 0, 0x000022d2, 0x00000201)))
	{
		return 1;
	}


	if(ecrt_domain_reg_pdo_entry_list(domain, domain_regs))
	{
		std::cout << "Failed to register pdo list\n";
		return 1;
	}
	
	if(ecrt_master_activate(master))
	{
		return 1;
	}

	if(!(domain_process_data = ecrt_domain_data(domain)))
	{
		std::cout << "Error during ecrt_domain_data\n";
		return 1;
	}
	

	while(true)
	{
		ecrt_master_receive(master);
		ecrt_domain_process(domain);

		std::cout << EC_READ_S32(domain_process_data + offset.position_val) << std::endl;

		/* uint8_t* data = domain_process_data + in_arr_offset1;
		
		std::vector<int8_t> vals = [&data]() -> std::vector<int8_t>{
			std::vector<int8_t> res;
			for(int i = 0; i < 10; i++)
			{
				auto val = ((int8_t) *((uint8_t*)(data + i)));
				res.push_back(val);
			}
			
			return res;
		}();

		for(const auto in : vals)
		{
			std::cout << "from plc: " << (uint16_t)in << std::endl;
		} */

			/* std::cout << "Diag: " << (uint16_t)EC_READ_S8(domain_process_data + diag_offset) << std::endl;
		ecrt_domain_queue(domain);
		ecrt_master_send(master);
		using namespace std::chrono_literals;
		const std::chrono::duration<double, std::milli> sl(4ms);
		std::this_thread::sleep_for(sl); */

	}

	ecrt_release_master(master);

	return 0;
	
}
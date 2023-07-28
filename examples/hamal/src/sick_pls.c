

#include "ecrt.h"

#define VendorID 0x01000056
#define ProductCode 0x00002301
#define SlavePos 0, 0

bool run = true;

static struct
{
    unsigned int input_msg;
    unsigned int diag;
    unsigned int output_msg;
}plc_offset;

const static ec_pdo_entry_reg_t domain_regs[] = {
    {SlavePos, VendorID, ProductCode, 0x2001, 0x01, &plc_offset.input_msg},
    {SlavePos, VendorID, ProductCode, 0x10f3, 0x04, &plc_offset.diag},
    {SlavePos, VendorID, ProductCode, 0x2000, 0x01, &plc_offset.output_msg}
};

ec_pdo_entry_info_t slave_0_pdo_entries[] = {
    {0x2001, 0x01, 80}, /* Dataset1 */
    {0x10f3, 0x04, 8}, /* New Messages Available */
    {0x2000, 0x01, 80}, /* Dataset1 */
};

ec_pdo_info_t slave_0_pdos[] = {
    {0x1600, 1, slave_0_pdo_entries + 0}, /* Out */
    {0x1a00, 2, slave_0_pdo_entries + 1}, /* In */
};

ec_sync_info_t slave_0_syncs[] = {
    {0, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE},
    {1, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},
    {2, EC_DIR_OUTPUT, 1, slave_0_pdos + 0, EC_WD_ENABLE},
    {3, EC_DIR_INPUT, 1, slave_0_pdos + 1, EC_WD_DISABLE},
    {0xff}
};

static ec_master_t *master = NULL;
static ec_master_state_t master_state = {};

static ec_domain_t *domain1 = NULL;
static ec_domain_state_t domain1_state = {};

static ec_slave_config_t *sc;
static ec_slave_config_state_t sc_state = {};


int main(int argc, char** argv)
{
    while(run)
    {
        
    }
}

#ifndef SLAVE_EXAMPLE_HPP
#define SLAVE_EXAMPLE_HPP

#include "../include/slave.hpp"


class EPOS4 : public ethercat_interface::slave::Slave
{
    public:

    EPOS4(const std::string& slave_name, ethercat_interface::utilities::SlaveInfo slave_info, ethercat_interface::Offset* offset = nullptr);
    ~EPOS4(){};

    private:

    ec_sync_info_t* configure_syncs() override;
    
};

#endif
# EtherCAT Interface

This repository aims to create a simple framework for using the EtherCAT Master library IgH.
The current state supports motor driver slaves that follow the CIA402 standard.

# Build and Installation

This library requires the EtherCAT Master library from IgH as a dependency.

```bash
cd path_to/ethercat_interface
mkdir build && cd build
cmake ..
sudo cmake --build . --target install
```

If the libethercat.so file is not found during the build process, change the file path for it in the CMakeLists.txt .

# Usage

Before running the example or your code make sure the EtherCAT system is up and running in your system.
An example of this process can be found in the setup_ethercat.sh file. In a terminal as root user run:
```bash
./setup_ethercat.sh
```

The Master and Domain base classes provides the EtherCAT Master and the domains for your slaves.

```cpp
#include "ethercat_interface/master.hpp"
#include "ethercat_interface/domain.hpp"
#include "ethercat_interface/slave.hpp"

class MyController
{
    public:

    MyController()
    {
        m_Master0 = new ethercat_interface::master::Master(0);

        m_Domain0 = new ethercat_interface::domain::Domain("domain_0");
        master->registerDomain(m_Domain0);
        epos4 = new ethercat_interface::slave::Slave(
            "epos4_0",
            "/path/to/config/file/EPOS4/epos4_config.yaml",
            new ServoOffset(),
            true
        );

        m_Domain0->registerSlave(epos4);
        m_Master0->configureDomains();
        m_Master0->setupDomains();
        if(!m_Master0->activateMaster())
            std::cout << "cant activate master\n";
    }

    ethercat_interface::master::Master* m_Master0;

    ethercat_interface::domain::Domain* m_Domain0;

    private:

    std::unique_ptr<MyCustomSlave> m_EPOS4_0;
}

```

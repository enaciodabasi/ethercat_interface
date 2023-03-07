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

In order to use the interface with your own slave, inherit from the Slave base class and override the methods that you wish to change. The "slave_example.hpp/.cpp" in the examples directory can provide a starting point.
```cpp
#include "ethercat_interface/slave.hpp"

class EPOS4 : public ethercat_interface::slave::Slave
{   
}

```

The Controller base class provides the EtherCAT Master and the domains for your slaves. It also has the pure cyclic_task function which should be overriden by your custom Controller-derived class.

```cpp
#include "ethercat_interface/controller.hpp"

class MyController: public ethercat_interface::controller::Controller
{
    public:

    private:

    std::unique_ptr<MyCustomSlave> m_MyCustomSlave0;
}

```

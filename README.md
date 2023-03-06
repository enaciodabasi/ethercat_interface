# EtherCAT Interface

This repository aims to create a simple framework for using the EtherCAT Master library IgH.

# Build and Installation

This library requires the EtherCAT Master library from IgH as a dependency.

'''bash
cd path_to/ethercat_interface
mkdir build && cd build
cmake ..
sudo cmake --build . --target install
'''

If the libethercat.so file is not found during the build process, change the file path for it in the CMakeLists.txt .
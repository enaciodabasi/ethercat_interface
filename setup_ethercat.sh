#! /usr/bin/bash

# start the EtherCAT system.
ethercatctl start
# Set the link to the ethernet up.
ip link set enp3s0 up

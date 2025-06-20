#!/bin/bash
sudo apt -qqy update && sudo apt -qqy upgrade
sudo apt install -y autogen autoconf autoconf-archive libtool autotools-dev 
sudo apt install -y gpiod libgpiod-dev
sudo apt -qqy upgrade
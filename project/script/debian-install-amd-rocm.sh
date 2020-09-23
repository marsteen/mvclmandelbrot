#!/bin/sh
wget -q -O - http://repo.radeon.com/rocm/rocm.gpg.key | apt-key add -
echo 'deb [arch=amd64] http://repo.radeon.com/rocm/apt/debian/ xenial main' | tee /etc/apt/sources.list.d/rocm.list
apt install rocm-dkms

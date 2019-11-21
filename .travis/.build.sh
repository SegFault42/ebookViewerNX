#!/bin/bash -ex

sudo apt-get update
sudo apt-get -y install build-essential
source /etc/profile.d/devkit-env.sh
export DEVKITA64=/opt/devkitpro/devkitA64

cd libs
make -f Makefile.mupdf
cd ..
make

#!/bin/sh

### This script will set up an environment to run OpenAI gym on a new VM running
### Ubuntu 18.04. It will install all components necessary to replicate an
### environment to run the gym correctly on a VM. Also, it will will install the
### gym-uds dependencies needed to communicate with other executions (C++, Haskell)
### Consider that Python3 is installed along with pip3

### Install OpenAI Gym
sudo apt update -y && sudo apt upgrade -y
sudo apt install -y python3-dev python3-pip zlib1g-dev libjpeg-dev cmake swig python-pyglet python3-opengl libboost-all-dev libsdl2-dev libosmesa6-dev patchelf ffmpeg xvfb autoconf automake libtool curl make g++ unzip
sudo python3 -m pip install --upgrade pip
sudo python3 -m pip install gym
# if results are going to be copied to S3:
# sudo apt-get -y install s3cmd
# s3cmd --configure
# s3cmd put LOGFILE.txt s3://S3BUCKET/
### Install grpcio
pip3 install grpcio --user
pip3 install grpcio-tools --user
git clone -b $(curl -L https://grpc.io/release) https://github.com/grpc/grpc
cd grpc
git submodule update --init --recursive
make
sudo make install
sudo ldconfig
### Install Protobuf
cd third_party/protobuf/
# ./autogen.sh
# ./configure
# make
sudo make install
sudo ldconfig
cd ~

### Install gym-uds-api
git clone https://github.com/rval735/gym-uds-api/
cd gym-uds-api
git checkout MultipleActions
./build.sh
cd binding-cpp/
make
cd ~

### NOTE!! If the compiler complains when trying to copile the
### c++ example (gym-uds-client), use the MakeFile that is located
### on the "src" folder to create all files: headers, pn and binary

### install Zweifel library
git clone https://github.com/zweifel/zweifel.git
cd zweifel/src
./create_all_libs.sh
cd ..
PATH_TO_ZWEIFEL_LIBRARY=$(pwd)/lib
cd ~

### install BiSUNA
git clone git@bitbucket.org:rval735/suna.git
cd suna
make
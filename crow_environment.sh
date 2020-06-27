#!/bin/bash

export NLOHMANN_CROW_UNAME=`uname -a`
export NLOHMANN_CROW_CMAKE_SYSTEM_NAME=`uname -s`
export NLOHMANN_CROW_HOSTNAME=`uname -n`
export NLOHMANN_CROW_IP_ADDRESS=`dig @resolver1.opendns.com ANY myip.opendns.com +short`

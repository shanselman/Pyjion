#!/bin/bash
set -e
DOTNET_VERSION=6.0.101
export TZ=Europe/London
export DEBIAN_FRONTEND=noninteractive
echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections
ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

apt-get -y update && apt-get install -y software-properties-common && apt-get -y update \
    && add-apt-repository ppa:deadsnakes/ppa && apt-get -y update \
    && apt-get install -y wget bison unzip bzip2 python3.10 python3.10-dev python3.10-distutils python3.10-venv \
    && apt-get clean -y && rm -rf /var/lib/apt/lists/*
wget https://dotnetcli.azureedge.net/dotnet/Sdk/${DOTNET_VERSION}/dotnet-sdk-${DOTNET_VERSION}-linux-x64.tar.gz -q -O dotnet.tar.gz
tar zxf dotnet.tar.gz
cp shared/Microsoft.NETCore.App/6.0.1/libclrjit.so .
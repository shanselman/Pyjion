#!/bin/bash
DOTNET_VERSION=6.0.101
export TZ=Europe/London
ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

apt-get -y update && apt-get install -y software-properties-common && apt-get -y update \
    && add-apt-repository ppa:deadsnakes/ppa && apt-get -y update \
    && apt-get install -y wget bison unzip bzip2 python3.10 python3.10-dev python3.10-distutils python3.10-venv \
    && apt-get clean -y && rm -rf /var/lib/apt/lists/*
wget https://dotnetcli.azureedge.net/dotnet/Sdk/${DOTNET_VERSION}/dotnet-sdk-${DOTNET_VERSION}-linux-x64.tar.gz
mkdir -p dotnet && tar zxf dotnet-sdk-${DOTNET_VERSION}-linux-x64.tar.gz -C dotnet
cp dotnet/shared/Microsoft.NETCore.App.app/${DOTNET_VERSION}/libclrjit.so /usr/local/lib
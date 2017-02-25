FROM ubuntu:16.04

RUN apt-get update && apt-get install -y build-essential libboost-program-options-dev libcppdb-dev libboost-dev libgcrypt11-dev

RUN mkdir -p /opt/data

COPY . /opt/data/

FROM ubuntu:20.04

RUN apt update
ENV TZ=US
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN apt install build-essential nodejs python-dev autotools-dev libicu-dev libbz2-dev wget -y
WORKDIR /opt
RUN wget -O boost_1_75_0.tar.gz https://sourceforge.net/projects/boost/files/boost/1.75.0/boost_1_75_0.tar.gz/download && tar xzvf boost_1_75_0.tar.gz && cd boost_1_75_0/ && ./bootstrap.sh --prefix=/usr/ && ./b2 install --with-timer --with-json --with-log --with-program_options --with-math --with-random

RUN mkdir -p /opt/coding-dead/runner
WORKDIR /opt/coding-dead

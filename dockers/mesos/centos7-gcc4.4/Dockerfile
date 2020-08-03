FROM centos:7
MAINTAINER Alexander Rukletsov <rukletsov@gmail.com>

RUN yum update -qy

# Install Dependencies
RUN yum groupinstall -qy "Development Tools"

RUN yum -qy install                        \
  python-devel                             \
  java-1.7.0-openjdk-devel                 \
  maven                                    \
  zlib-devel                               \
  libcurl-devel                            \
  openssl-devel                            \
  cyrus-sasl-devel                         \
  cyrus-sasl-md5                           \
  apr-devel                                \
  subversion-devel                         \
  apr-util-devel

# Install GCC 4.4
RUN yum -qy install                        \
  compat-gcc-44.x86_64                     \
  compat-gcc-44-c++.x86_64

# Creating a Mesos repository
RUN mkdir -p /mesos
RUN mkdir -p /tmp

WORKDIR /mesos

# Clone mesos repository
ADD mesos /mesos

# Build Mesos
RUN export CC=gcc44
RUN export CXX=g++44
RUN ./bootstrap
RUN mkdir build
RUN cd build && ../configure --disable-python
RUN cd build && make -j 8 install



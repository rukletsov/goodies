FROM ubuntu:14.04
MAINTAINER Alexander Rukletsov <rukletsov@gmail.com>

RUN apt-get update -q

#Install Dependencies
RUN apt-get -qy install             \
    build-essential                 \
    autoconf                        \
    automake                        \
    ca-certificates                 \
    gdb                             \
    wget                \
    git-core                        \
    libcurl4-nss-dev                \
    libsasl2-dev                    \
    libtool                         \
    libsvn-dev                \
    libapr1-dev                \
    libgoogle-glog-dev            \
    libboost-dev            \
    protobuf-compiler            \
    libprotobuf-dev            \
    make                            \
    python                \
    python2.7                \
    libpython-dev            \
    python-dev                \
    python-protobuf            \
    python-setuptools            \
    heimdal-clients            \
    libsasl2-modules-gssapi-heimdal    \
    --no-install-recommends

#Creating a Mesos repository
RUN mkdir -p /mesos
RUN mkdir -p /tmp
RUN mkdir -p /usr/share/java/
RUN wget http://search.maven.org/remotecontent?filepath=com/google/protobuf/protobuf-java/2.5.0/protobuf-java-2.5.0.jar -O protobuf.jar
RUN mv protobuf.jar /usr/share/java/

WORKDIR /mesos

#Clone mesos repository
ADD mesos /mesos

#Bootstrap
RUN ./bootstrap

#Configure
RUN mkdir build && cd build && ../configure --disable-java --disable-optimize

#Make
RUN cd build && make -j 8 install

#RUN easy_install /mesos/build/src/python/dist/mesos.interface-0.22.0-py2.7.egg
#RUN easy_install /mesos/build/src/python/dist/mesos.native-0.22.0-py2.7-linux-x86_64.egg

ADD mesos.conf /etc/ld.so.conf.d/mesos.conf
RUN ldconfig


FROM fedora:23
MAINTAINER Alexander Rukletsov <rukletsov@gmail.com>

RUN dnf update -qy

RUN dnf groupinstall -qy "Development Tools"

RUN dnf -qy install                        \
  git                                      \
  gcc-c++

# Fetch and build the application
RUN git clone https://github.com/rukletsov/unresponsive-process.git /unresponsive-process
WORKDIR /unresponsive-process
RUN make

ENTRYPOINT ["/unresponsive-process/bin/unresponsive_process"]


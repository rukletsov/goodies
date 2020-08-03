#!/usr/bin/env bash
#
# Usage: ./grab-perf-trace.sh [node_hostname]
#
# The node hostname must contain either the string 'master' or 'agent' so that
# we can successfully identify the node type. These hostnames can be found in a
# link contained in the README.md created by the SRE team for each mixed
# workload test.

HOSTNAME=$1

echo $HOSTNAME | grep master
RETURN_CODE=$?

if [ "$RETURN_CODE" = "0" ]; then
  NODE_TYPE=master
else
  NODE_TYPE=agent
fi

FILENAME="mesos-$HOSTNAME-`date -u +%R | sed 's/:/-/'`.stacks"

TARGET_PID="`ssh centos@$HOSTNAME ps ax | grep mesos-$NODE_TYPE | grep -v grep | awk '{print $1}'`"
echo $TARGET_PID

ssh "centos@$HOSTNAME" "sudo perf record --freq=100 --all-cpus --no-inherit --output perf.data --call-graph dwarf -p $TARGET_PID -- sleep 60
  sudo perf script --header --input perf.data | c++filt > $FILENAME
  gzip $FILENAME"

scp "centos@$HOSTNAME:~/$FILENAME.gz" ./

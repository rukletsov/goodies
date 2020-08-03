#!/usr/bin/env python
# -*- coding: utf-8 -*-

#!/usr/bin/env python

# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import sys
import time
import threading
import signal
import datetime

try:
    from mesos.native import MesosExecutorDriver, MesosSchedulerDriver
    from mesos.interface import Executor, Scheduler
    from mesos.interface import mesos_pb2
except ImportError:
    from mesos import Executor, MesosExecutorDriver, MesosSchedulerDriver, Scheduler
    import mesos_pb2

import task_state


TASK_CPUS = 1
TASK_MEM = 256

class LaunchScheduler(Scheduler):
    def __init__(self, command):
        print "Initializing scheduler instance"
        self.tasksLaunched = 0
        self.tasksFinished = 0
        self.command = command

    def registered(self, driver, frameworkId, masterInfo):
        print "Registered with {} as {}".format(
            frameworkId.value, masterInfo.id)

    def resourceOffers(self, driver, offers):
        for offer in offers:
            tasks = []

            tid = self.tasksLaunched
            self.tasksLaunched += 1

            task = mesos_pb2.TaskInfo()
            task.task_id.value = str(tid)
            task.slave_id.value = offer.slave_id.value
            task.name = "task %d" % tid

            task.command.value = self.command

            cpus = task.resources.add()
            cpus.name = "cpus"
            cpus.type = mesos_pb2.Value.SCALAR
            cpus.scalar.value = TASK_CPUS

            mem = task.resources.add()
            mem.name = "mem"
            mem.type = mesos_pb2.Value.SCALAR
            mem.scalar.value = TASK_MEM

            tasks.append(task)

            driver.launchTasks(offer.id, tasks)


    def statusUpdate(self, driver, update):
        taskID = update.task_id.value
        stateName = task_state.decode[update.state]
        print "{}: Task [{}] is in state [{}]".format(
            datetime.datetime.now(), taskID, stateName)

        if update.state > 1: # Terminal state
            self.tasksFinished += 1


def hard_shutdown(signal, frame):
    print "Shutting down..."
    driver.stop()


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print "Usage: %s master command" % sys.argv[0]
        sys.exit(1)

    framework = mesos_pb2.FrameworkInfo()
    framework.user = ""
    framework.name = "Lanceur (Simple Launcher Framework in python)"

    driver = MesosSchedulerDriver(
        LaunchScheduler(sys.argv[2]),
        framework,
        sys.argv[1])

    print "Starting Mesos driver"

    # driver.run() blocks; we run it in a separate thread
    def run_driver_async():
        status = 0 if driver.run() == mesos_pb2.DRIVER_STOPPED else 1
        driver.stop()
        sys.exit(status)
    framework_thread = threading.Thread(target = run_driver_async)
    framework_thread.start()

    print "(Listening for Ctrl-C)"
    signal.signal(signal.SIGINT, hard_shutdown)
    while framework_thread.is_alive():
        time.sleep(1)

    sys.exit(0)

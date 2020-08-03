lanceur
=======

A (very) simple mesos framework for launching shell tasks.

How to use
----------

The scheduler requires Mesos python eggs. They can either be installed (e.g., via `easy_install`) or provided via `PYTHONPATH`. The latter is preferable because the system environment is not modified in this case. If you build Mesos from sources, python eggs are put into "/src/python/dist" folder; e.g. if Mesos build folder is "/Users/user/mesos/build/", the eggs are in "/Users/user/mesos/build/src/python/dist". In this case, the framework scheduler can be launched as:
```
PYTHONPATH=/Users/user/mesos/build/src/python/dist/mesos-0.29.0-py2.7.egg:/Users/user/mesos/build/src/python/dist/mesos.cli-0.29.0-py2.7.egg:/Users/user/mesos/build/src/python/dist/mesos.executor-0.29.0-py2.7-macosx-10.10-intel.egg:/Users/user/mesos/build/src/python/dist/mesos.interface-0.29.0-py2.7.egg:/Users/user/mesos/build/src/python/dist/mesos.native-0.29.0-py2.7.egg:/Users/user/mesos/build/src/python/dist/mesos.scheduler-0.29.0-py2.7-macosx-10.10-intel.egg python lanceur.py <master-ip:port> <command>
```

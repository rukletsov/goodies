#!/bin/bash

( function sighandler { echo "called $1"; }; trap 'sighandler TERM' SIGTERM; trap 'sighandler INT' SIGINT; echo $$; echo $(which sleep); while true; do date; sleep 1; done; exit 0 )

#!/bin/bash

[ -e client_fifo ] || mkfifo client_fifo
nc 52.8.225.234 17004 < client_fifo | bash ./play_seaquest.sh > client_fifo

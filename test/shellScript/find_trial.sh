#!/bin/bash

sdb root on & sdb shell cat /proc/sched_debug | grep "trial"

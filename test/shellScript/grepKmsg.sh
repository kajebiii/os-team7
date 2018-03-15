#!/bin/bash

echo "$1"
cat /proc/kmsg | grep "$1"

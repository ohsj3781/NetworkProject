#!/bin/bash

# Define the log file variable
LOGFILE="logq1.dat"

# Process the log file and extract flows
awk '$1=="1" {print $2 "\t" $3}' "$LOGFILE" > flow1.dat && \
awk '$1=="2" {print $2 "\t" $3}' "$LOGFILE" > flow2.dat && \
awk '$1=="Back" {print $2 "\t" $3}' "$LOGFILE" > flowBack.dat

python3 assignment01.py

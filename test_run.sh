#!/bin/bash
xvfb-run ./neverball > out.txt 2>&1 &
sleep 2
pkill neverball
cat out.txt

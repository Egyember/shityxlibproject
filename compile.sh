#!/bin/bash
gcc ./test.c -lX11 -g -fstack-check -fbounds-check -o ./test
chmod +x ./test

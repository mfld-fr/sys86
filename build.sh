#!/bin/bash

# Build binaries for target

as86 -0 -o test.o test.s

ld86 -0 -d -M -o test.bin test.o > test.map


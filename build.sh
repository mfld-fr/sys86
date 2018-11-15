#!/bin/bash

# Build binaries for target

as86 -0 -o flash.o flash.s

ld86 -0 -d -M -o flash.bin flash.o > flash.map


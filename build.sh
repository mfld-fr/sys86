#!/bin/bash

# Build all the MON86 project

SCRIPTDIR="$(dirname "$0")"
. "$SCRIPTDIR/env.sh"
make all

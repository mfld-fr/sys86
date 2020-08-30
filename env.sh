#!/bin/bash

# Set up the project environment

# Must be executed in project top directory

if [[ ! -f "env.sh" ]]; then
	echo "error: source this script from the project top directory";
	return 1;
fi

export TOPDIR="$(pwd)"

echo TOPDIR set to $TOPDIR

# Require the cross build tools

if [[ ! -d "$TOPDIR/cross" ]]; then
	echo "error: missing folder for the cross build tools";
	return 1;
fi

export CROSSDIR="$TOPDIR/cross"	

echo CROSSDIR set to $CROSSDIR

# Set path to binaries

add_path () {
	if [[ ":$PATH:" != *":$1:"* ]]; then
		export PATH="$1:$PATH"
	fi
}

add_path "$CROSSDIR/bin"

echo PATH set to $PATH


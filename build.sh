#!/bin/bash

export PREFIX="$HOME/Dev/System-Dev/SharkyOS/cross-compiler/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
make all

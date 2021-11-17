#!/bin/sh -xe
git clean -xfd
git submodule foreach --recursive git clean -xfd

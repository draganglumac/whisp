#!/usr/bin/env bash

function setup_for() {
	if [ -d build ]; then
		rm -rf build
	fi	
	mkdir build
	cp $1 build/
}

function make_and_run() {
	make -s test
	./runtests
}

function teardown() {
	rm -rf build
#	rm runtests
}

for f in tests/*/*.c
do
	setup_for $f
	make_and_run
	teardown	
done

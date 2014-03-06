all: setup artifact
setup:
	mkdir -p build
	cp src/**/*.c build
	cp src/**/*.h build
artifact:
	gcc build/*.c -o chat -ljnxc -luuid -DJNX_MEMORY_MANAGEMENT -g
teardown:
	rm -rf build

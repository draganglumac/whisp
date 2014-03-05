all: setup artifact teardown
setup:
	mkdir -p build
	cp src/**/*.c build
	cp src/**/*.h build
artifact:
	gcc build/*.c -o chat -ljnxc -luuid -DJNX_MEMORY_MANAGEMENT
teardown:
	rm -rf build

all: teardown setup artifact
setup:
	mkdir -p build
	cp src/*.c build
	cp src/**/*.c build
	cp src/**/*.h build
artifact:
	gcc build/*.c -o chat -ljnxc -luuid -DJNX_MEMORY_MANAGEMENT -g -lcrypto -lssl
teardown:
	rm -rf build
tests:
	cp src/**/*.c build
	cp src/**/*.h build
	gcc build/*.c -o runtests -ljnxc -luuid -DJNX_MEMORY_MANAGEMENT -g -lcrypto -lssl

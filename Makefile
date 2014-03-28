all: teardown setup artifact teardown
setup:
	mkdir -p build
	cp src/**/*.c build
	cp src/**/*.h build
artifact:
	gcc build/*.c -o whisp -ljnxc -luuid -DJNX_MEMORY_MANAGEMENT -g -lcrypto -lssl
teardown:
	rm -rf build
test:
	cp src/**/*.c build
	cp src/**/*.h build
	rm -rf build/main.c
	gcc build/*.c -o runtests -ljnxc -luuid -DJNX_MEMORY_MANAGEMENT -g -lcrypto -lssl

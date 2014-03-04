all:
	gcc src/*.c -o chat -ljnxc -luuid -DJNX_MEMORY_MANAGEMENT

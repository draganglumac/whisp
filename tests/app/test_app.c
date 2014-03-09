#include <assert.h>

#include <jnxc_headers/jnxterm.h>
#include "app.h"

void test_random_port_in_range() {
	jnx_term_printf_in_color(JNX_COL_WHITE, "\ttest_random_port_in_range(): ");

	int i, count = 10, min = 10000, max = 20000, current;
	for (i = 0; i < count; i++) {
		current = random_in_range(min, max);
		assert(min <= current && current <= max);
	}

	jnx_term_printf_in_color(JNX_COL_GREEN, "Pass\n");
}

int main() {
	jnx_term_printf_in_color(JNX_COL_WHITE, "Testing app.c functions:\n");
	
	test_random_port_in_range();
	
	jnx_term_printf_in_color(JNX_COL_WHITE, "OK.\n");
	return 0;
}

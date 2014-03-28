#include <assert.h>
#include <time.h>
#include <string.h>
#include <jnxc_headers/jnxterm.h>
#include <jnxc_headers/jnxmem.h>
#include "peerstore.h"


char *make_guid() {
	
	char *ar[7] = {
		"A","B","C","D","E","F","G"
	};
	char buffer[256];
	char *a = ar[rand() % 7];
	char *b = ar[rand() % 7];
	char *c = ar[rand() % 7];
	char *d = ar[rand() % 7];
	char *e = ar[rand() % 7];
	sprintf(buffer,"%s%s%s%s%s",a,b,c,d,e);
	return strdup(buffer);
}
raw_peer *make_fake_peer() {
	raw_peer *r = JNX_MEM_MALLOC(sizeof(raw_peer));
	r->guid = make_guid();
	r->command = strdup("");
	r->ip = strdup("");
	r->port = strdup("");
	r->secure_port = strdup("");
	r->peerstring = strdup("");
	return r;
}
void add_remove_peers() {
	jnx_term_printf_in_color(JNX_COL_WHITE, "\tadd_remove_peers(): ");
	int c = 10;
	assert(peerstore_get_count() == 0);
	int x;
	
	for(x = 0; x < c; ++x) {
		peerstore_add_peer(make_fake_peer());
	}
	assert(peerstore_get_count() == c);

	peerstore_destroy();

	assert(peerstore_get_count() == 0);

	jnx_term_printf_in_color(JNX_COL_GREEN, "Pass\n");
}

int main() {
	srand(0);
	jnx_term_printf_in_color(JNX_COL_WHITE, "Testing peerstore.c functions:\n");
	
	add_remove_peers();

	jnx_term_printf_in_color(JNX_COL_WHITE, "OK.\n");
	return 0;
}

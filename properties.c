#include "properties.h"

#include <assert.h>
#include <ctype.h>

void
properties_parse_line( char *line, struct evkeyvalq *output ) {
	char *key, *value, *end;
	key = line;
	while( isspace(*key) ) key++;
	if( *key == ';' ) return;
	value = key;
	while( isalnum(*value) || *value == '_' ) value++;
	while( isspace(*value) ) { *value++ = 0; };
	if( *value != '=' ) return;
	*value++ = 0;
	while( isspace(*value) ) value++;
	end = value + strlen(value) - 1;
	while( end > value && isspace(*end) ) end--;
	*(end+1) = 0;
	
	if( strlen(key) && strlen(value) ) {
		evhttp_add_header(output, key, value);
	}
}

void
properties_parse_file( FILE *fh, struct evkeyvalq *output ) {
	char line[1024];
	assert( output != NULL );
	assert( fh != NULL );
	
	while( fgets(line, sizeof(line) / sizeof(char), fh) != NULL ) {
		properties_parse_line(line, output);
	}		
}
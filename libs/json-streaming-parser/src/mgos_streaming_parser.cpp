#include "mgos_json_streaming_parser.h"
#include "mgos_streaming_parser.h"

JsonStreamingParser parser;
SampleJsonListener sample_listener;

//
//
void SampleJsonListener::whitespace(char c) {
	printf(" ");
}

void SampleJsonListener::startDocument() {
	printf("start document.\n");
}

void SampleJsonListener::key(String keystring) {
	const char *c_str = keystring.c_str();
	printf("'%s' : ", c_str);
}

void SampleJsonListener::value(String value) {
	const char *c_str = value.c_str();
	printf("'%s',\n", c_str);
}

void SampleJsonListener::endArray() {
	printf("] end array. \n");
}

void SampleJsonListener::endObject() {
	printf("} end object.\n");
}

void SampleJsonListener::endDocument() {
	printf(" end document.\n");
}

void SampleJsonListener::startArray() {
	 printf(" [ ");
}

void SampleJsonListener::startObject() {
	 printf(" { ");
}


void streaming_parser_set_listener(void * json_listener)
{
	parser.setListener((JsonListener *)json_listener);
}


void sample_streaming_parser_init()
{
	streaming_parser_set_listener(&sample_listener);
}

//
//
int streaming_parser_push_data(const char * const json, const int len)
{
	int i;

	for (i = 0; i < len; i++) {
		parser.parse(json[i]);
	}

	return len;
}

//
//

#ifndef MOS_LIBS_MGOS_JSON_STREAMING_PARSER_H_
#define MOS_LIBS_MGOS_JSON_STREAMING_PARSER_H_

#ifdef __cplusplus

#include "JsonStreamingParser.h"
#include "JsonListener.h"
#include <WString.h>

extern "C" {
#endif

int streaming_parser_push_data(const char * const data, const int len);
void streaming_parser_set_listener(void * json_listener);

#ifdef __cplusplus
}
#endif

#endif // MOS_LIBS_MGOS_JSON_STREAMING_PARSER_H_

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void sf_init();
void sf_command(const char* command);
void sf_set_output_callback(void (*callback)(const char*));

#ifdef __cplusplus
}
#endif

#ifndef TT_BASIC_H
#define TT_BASIC_H
#include "GUI.H"
#define MAX_LINES 256
#define LINE_LEN 160
#define OUT_LINES 10
#define OUT_LEN 80
#define BASIC_FILES 16
#define BASIC_PATH 160
typedef char (*BasicSource)[LINE_LEN];
typedef BasicSource (*BasicReader)(char *path, int *count);
typedef int (*BasicInput)(char *prompt, char *buffer, int capacity);
typedef int (*BasicPoll)(void);
void basic_set_input(BasicInput reader);
void basic_set_poll(BasicPoll reader);
int basic_is_reserved(char *name);
void basic_set_gui(BasicGui callback);
#define BASIC_DEBUG_LINE 0
#define BASIC_DEBUG_STOP 1
#define BASIC_DEBUG_ERROR 2
typedef int (*BasicDebug)(char *path, int row, int depth, int reason);
typedef struct { char name[48], type[32], value[128]; } BasicDebugValue;
void basic_set_debug(BasicDebug callback);
int basic_debug_frames(void);
int basic_debug_frame(int frame, char *name, char *path, int *row);
int basic_debug_count(int frame);
int basic_debug_variable(int frame, int index, BasicDebugValue *value);
char *basic_debug_error(void);
void basic_set_reader(BasicReader reader);
int basic_run(char source[MAX_LINES][LINE_LEN], int count,
              char output[OUT_LINES][OUT_LEN], int *used);
int basic_run_project(char source[MAX_LINES][LINE_LEN], int count,
                      char *mainpath, char output[OUT_LINES][OUT_LEN], int *used);
int basic_scan_project(char source[MAX_LINES][LINE_LEN], int count, char *mainpath);
int basic_file_count(void);
char *basic_file_path(int file);
char *basic_file_name(int file);
int basic_error_main_line(void);
char *basic_error_path(void);
#endif

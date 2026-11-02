#ifndef TT_BASIC_H
#define TT_BASIC_H
#define MAX_LINES 64
#define LINE_LEN 96
#define OUT_LINES 10
#define OUT_LEN 80
#define BASIC_FILES 16
#define BASIC_PATH 160
typedef char (*BasicSource)[LINE_LEN];
typedef BasicSource (*BasicReader)(char *path, int *count);
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

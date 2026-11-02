#ifndef TT_PROJECT_H
#define TT_PROJECT_H
#include "BASIC.H"
typedef struct {
    char file[BASIC_PATH], directory[BASIC_PATH], name[9];
    char members[BASIC_FILES][BASIC_PATH];
    int count;
} Project;
typedef int (*ProjectMkdir)(char *path);
char *project_error(void);
int project_equal(char *a,char *b);
int project_join(char *out,char *directory,char *name);
char *project_basename(char *path);
void project_default(Project *project,char *entry);
int project_read_source(char *path,char text[MAX_LINES][LINE_LEN],int *count);
int project_save(Project *project);
int project_load(Project *project,char *file);
int project_create(Project *project,char *parent,char *name,ProjectMkdir mkdir_fn);
int project_new_file(Project *project,char *name);
int project_add_file(Project *project,char *path);
#endif

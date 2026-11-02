#ifndef TT_COMPLETE_H
#define TT_COMPLETE_H
#include "BASIC.H"
#define COMPLETE_MAX 64
typedef struct { char name[24], detail[80]; } CompletionItem;
typedef struct {
    CompletionItem item[COMPLETE_MAX];
    int count, start, end;
} Completion;
void complete_build(BasicSource source, int count, int row, int col,
                    char *path, BasicReader reader, Completion *result);
#endif

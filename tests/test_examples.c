#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "BASIC.H"
static char source[MAX_LINES][LINE_LEN],output[OUT_LINES][OUT_LEN];
static int input(char *prompt,char *buffer,int capacity)
{
    assert(capacity>6);strcpy(buffer,strstr(prompt,"age") ? "21" : "Marco");return 1;
}
int main(int argc,char **argv)
{
    char *files[]={"MAIN.BAS","OODEMO.BAS","FLOW.BAS","POLY.BAS","INPUT.BAS","DEBUG.BAS","NUMBERS.BAS"};
    char *expected[]={"42","First: 42","Collection sum: 42","Rex says woof","Hello Marco, adult.","Result: 12","Array field: 0.25"};
    char path[BASIC_PATH];FILE *file;int i,n,len,used,result,j,found;
    assert(argc==2);basic_set_input(input);
    for(i=0;i<7;++i){
        assert(strlen(argv[1])+strlen(files[i])+2<BASIC_PATH);
        sprintf(path,strchr(argv[1],'\\') ? "%s\\%s" : "%s/%s",argv[1],files[i]);
        file=fopen(path,"r");assert(file);memset(source,0,sizeof(source));n=0;
        while(n<MAX_LINES && fgets(source[n],LINE_LEN,file)){
            len=strlen(source[n]);while(len && (source[n][len-1]=='\n'||source[n][len-1]=='\r'))source[n][--len]=0;++n;
        }
        assert(!ferror(file));fclose(file);result=basic_run_project(source,n,path,output,&used);
        if(result)fprintf(stderr,"%s: %s\n",files[i],output[used-1]);assert(!result);
        found=0;for(j=0;j<used;++j)if(!strcmp(output[j],expected[i]))found=1;
        if(!found){for(j=0;j<used;++j)fprintf(stderr,"%s\n",output[j]);}assert(found);
    }
    basic_set_input(0);puts("7 shipped examples passed.");return 0;
}

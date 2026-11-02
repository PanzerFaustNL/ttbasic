#include <stdio.h>
#include <string.h>
#include <assert.h>
#ifdef ATARI
#include <tos.h>
#else
#include <sys/stat.h>
#endif
#include "PROJECT.H"
static char text[MAX_LINES][LINE_LEN];
static int mkdir_project(char *path)
{
#ifdef ATARI
    return Dcreate(path)>=0;
#else
    return mkdir(path,0700)==0;
#endif
}
static void write_file(char *path,char *value)
{
    FILE *f;f=fopen(path,"wb");assert(f);fputs(value,f);assert(!fclose(f));
}
int main(int argc,char **argv)
{
    Project p,loaded,before;char path[BASIC_PATH],external[BASIC_PATH],target[BASIC_PATH],name[16];
    int n,i;FILE *f;
    assert(argc==2);
    assert(project_create(&p,argv[1],"ALPHA",mkdir_project));assert(p.count==1);
    assert(!strcmp(p.name,"ALPHA"));assert(project_load(&loaded,p.file));assert(project_equal(p.file,loaded.file));
    assert(project_read_source(p.members[0],text,&n));assert(strstr(text[0],"Hello from ALPHA"));
    before=p;assert(!project_create(&p,argv[1],"ALPHA",mkdir_project));assert(!memcmp(&p,&before,sizeof(p)));
    assert(!project_create(&p,argv[1],"../BAD",mkdir_project));assert(!memcmp(&p,&before,sizeof(p)));
    assert(project_new_file(&p,"MODEL"));assert(p.count==2);assert(strstr(p.members[1],"MODEL.BAS"));
    assert(project_read_source(p.members[1],text,&n));assert(!strcmp(text[0],"Rem New source file"));
    before=p;assert(!project_new_file(&p,"MODEL.BAS"));assert(!memcmp(&p,&before,sizeof(p)));
    assert(project_join(external,argv[1],"LIB.BAS"));write_file(external,"Class Library\r\nEnd Class\r\n");
    assert(project_add_file(&p,external));assert(p.count==3);assert(project_equal(p.members[2],external)==0);
    assert(project_read_source(p.members[2],text,&n));assert(!strcmp(text[0],"Class Library"));
    assert(project_add_file(&p,p.members[2]));assert(p.count==3);
    write_file(external,"REM Changed outside\r\n");before=p;
    assert(!project_add_file(&p,external));assert(!memcmp(&p,&before,sizeof(p)));
    assert(project_read_source(p.members[2],text,&n));assert(!strcmp(text[0],"Class Library"));
    assert(project_load(&loaded,p.file));assert(loaded.count==3);assert(project_equal(loaded.members[0],p.members[0]));
    assert(project_join(path,p.directory,"ALPHA.PBK"));f=fopen(path,"r");assert(f);fclose(f);
    assert(project_join(path,p.directory,"BAD.TBP"));write_file(path,"WRONG\n");before=loaded;
    assert(!project_load(&loaded,path));assert(!memcmp(&loaded,&before,sizeof(loaded)));
    write_file(path,"TTBASIC_PROJECT 1\nNAME=BAD\nENTRY=MISSING.BAS\nFILE=MISSING.BAS\n");
    assert(!project_load(&loaded,path));assert(!memcmp(&loaded,&before,sizeof(loaded)));
    write_file(path,"TTBASIC_PROJECT 1\nNAME=BAD\nENTRY=MAIN.BAS\nFILE=MODEL.BAS\n");assert(!project_load(&loaded,path));
    assert(project_join(path,p.directory,"../LIB.BAS"));assert(project_equal(path,external));
    assert(project_join(target,"C:\\WORK\\DEMO",".\\MAIN.BAS"));assert(project_equal(target,"C:\\WORK\\DEMO\\MAIN.BAS"));
    assert(!project_new_file(&p,"VERYLONGNAME.BAS"));assert(!project_new_file(&p,"X.TXT"));
    for(i=3;i<BASIC_FILES;++i){sprintf(name,"FILE%d",i);assert(project_new_file(&p,name));}
    assert(p.count==BASIC_FILES);before=p;assert(!project_new_file(&p,"EXTRA"));assert(!memcmp(&p,&before,sizeof(p)));
    assert(project_join(path,p.directory,"LONG.BAS"));f=fopen(path,"wb");assert(f);
    for(i=0;i<LINE_LEN;++i)fputc('x',f);fclose(f);assert(!project_read_source(path,text,&n));
    f=fopen(path,"wb");assert(f);for(i=0;i<MAX_LINES+1;++i)fputs("\r\n",f);fclose(f);assert(!project_read_source(path,text,&n));
    write_file(path,"Print 1\r\nPrint 2\nPrint 3\rEnd");assert(project_read_source(path,text,&n));assert(n==4&&!strcmp(text[3],"End"));
    f=fopen(path,"wb");assert(f);fputc(0,f);fclose(f);assert(!project_read_source(path,text,&n));
    puts("Project persistence, membership, collisions and source validation checks passed.");return 0;
}

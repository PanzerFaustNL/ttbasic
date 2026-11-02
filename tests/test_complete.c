#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "COMPLETE.H"
static char source[MAX_LINES][LINE_LEN];
static Completion result;
static char path[BASIC_PATH];
static char memory_source[MAX_LINES][LINE_LEN];
static BasicSource memory_reader(char *requested,int *count)
{
    if(strstr(requested,"REKEN.BAS")){*count=3;return memory_source;}return 0;
}
static int has(char *name)
{
    int i;for(i=0;i<result.count;++i)if(!strcmp(result.item[i].name,name))return 1;return 0;
}
static void build(char *text)
{
    int n,k,col;char *p;
    memset(source,0,sizeof(source));n=k=col=0;
    for(p=text;*p;++p){if(*p=='\n'){++n;k=0;}else if(*p=='|')col=k;else source[n][k++]=*p;}
    complete_build(source,n+1,n,col,path,0,&result);
}
int main(int argc,char **argv)
{
    FILE *f;char imported[BASIC_PATH];
    assert(argc==2);sprintf(path,"%s/MAIN.BAS",argv[1]);sprintf(imported,"%s/REKEN.BAS",argv[1]);
    f=fopen(imported,"w");assert(f);
    fputs("Class Base\nPublic Sub Inherited()\nEnd Sub\nPrivate hidden As Integer\nEnd Class\nClass Reken\nInherits Base\nPrivate value As Integer\nPublic Sub New(start As Integer)\nEnd Sub\nPublic Function GetValue() As Integer\nReturn 10\nEnd Function\nEnd Class\n",f);fclose(f);
    build("pri|");assert(has("Print") && has("Private") && result.start==0 && result.end==3);
    build("Prin|t 42");assert(has("Print") && result.end==5);
    build("Imports \"REKEN.BAS\"\nDim third As New Reken(10)\nthird.|");
    assert(has("GetValue") && has("Inherited") && !has("value") && !has("hidden") && !has("New") && !has("Print"));
    build("Imports \"REKEN.BAS\"\nDim third As New Reken(10)\nthird.g|");assert(result.count==1 && has("GetValue"));
    assert(strstr(result.item[0].detail,"GetValue()") && strstr(result.item[0].detail,"Integer"));
    build("Class Local\nPrivate value As Integer\nPublic Sub Test(input As String)\nMe.|");assert(has("value") && has("Test") && !has("input"));
    build("Class Local\nPublic Sub Test(input As String)\ninp|");assert(has("input"));
    build("Class Local\nPublic Sub Test(secret As String)\nEnd Sub\nEnd Class\nsec|");assert(!has("secret"));
    build("Dim things As New Collection\nthings.|");assert(has("Add") && has("Remove") && has("Clear") && has("Count"));
    build("Print \"pri|");assert(!result.count);
    build("Rem pri|");assert(!result.count);
    build("' pri|");assert(!result.count);
    build("Dim x As Missing\nx.|");assert(!result.count);
    build("Imports \"MISSING.BAS\"\nPr|");assert(has("Print"));
    build("Dim local As Integer\nloc|");assert(has("local"));
    build("Imports \"REKEN.BAS\"\nRek|");assert(result.count==1 && has("Reken"));
    assert(strstr(result.item[0].detail,"Reken(start As Integer)"));
    strcpy(memory_source[0],"Class Reken");strcpy(memory_source[1],"Public unsaved As String");strcpy(memory_source[2],"End Class");
    build("Imports \"REKEN.BAS\"\nDim x As Reken\nx.|");
    complete_build(source,3,2,2,path,memory_reader,&result);
    assert(has("unsaved") && !has("GetValue"));
    remove(imported);puts("Completion checks passed: imports, members, inheritance, visibility, scope, signatures and incomplete code.");return 0;
}

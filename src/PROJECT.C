/* Portable project manifests and non-destructive file operations, ANSI C. */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "PROJECT.H"
static char *last_error="";
char *project_error(void) { return last_error; }
static int error(char *message) { last_error=message;return 0; }
int project_equal(char *a,char *b)
{
    while(*a && *b){
        if((*a=='/'||*a=='\\')&&(*b=='/'||*b=='\\')){++a;++b;}
        else if(toupper((unsigned char)*a++)!=toupper((unsigned char)*b++))return 0;
    }
    return !*a && !*b;
}
char *project_basename(char *path)
{
    char *name,*p;name=path;for(p=path;*p;++p)if(*p=='/'||*p=='\\')name=p+1;return name;
}
static int clean(char *text)
{
    for(;*text;++text)if((unsigned char)*text<32 || *text=='"')return 0;return 1;
}
int project_join(char *out,char *directory,char *name)
{
    char raw[BASIC_PATH*2],part[BASIC_PATH],sep;
    int offsets[64],parts,pos,prefix,i,start,len,absolute;
    if(strlen(directory)>=BASIC_PATH||strlen(name)>=BASIC_PATH||!clean(directory)||!clean(name))return error("Invalid or long path");
    sep=strchr(directory,'\\')||strchr(name,'\\')||(directory[0]&&directory[1]==':')||(name[0]&&name[1]==':')?'\\':'/';
    absolute=*name=='/'||*name=='\\'||(name[0]&&name[1]==':');raw[0]=0;
    if(!absolute && directory[0]){strcpy(raw,directory);len=strlen(raw);if(raw[len-1]!='/'&&raw[len-1]!='\\') {raw[len++]=sep;raw[len]=0;}}
    strcat(raw,name);for(i=0;raw[i];++i)if(raw[i]=='/'||raw[i]=='\\')raw[i]=sep;
    pos=i=parts=0;
    if(raw[0]&&raw[1]==':'){out[pos++]=raw[0];out[pos++]=':';i=2;}
    if(raw[i]==sep)out[pos++]=raw[i++];prefix=pos;out[pos]=0;
    while(raw[i]){
        while(raw[i]==sep)++i;start=i;while(raw[i]&&raw[i]!=sep)++i;len=i-start;if(!len)break;
        memcpy(part,raw+start,len);part[len]=0;if(!strcmp(part,"."))continue;
        if(!strcmp(part,"..")&&parts&&strcmp(out+offsets[parts-1],"..")){pos=offsets[--parts];if(pos>prefix)--pos;out[pos]=0;continue;}
        if(!strcmp(part,"..")&&!parts&&prefix)return error("Path goes above root");
        if(pos&&out[pos-1]!=sep&&out[pos-1]!=':')out[pos++]=sep;
        if(pos+len>=BASIC_PATH||parts==64)return error("Path too long");
        offsets[parts++]=pos;memcpy(out+pos,part,len);pos+=len;out[pos]=0;
    }
    out[pos]=0;return pos?1:error("Empty path");
}
static void directory_of(char *out,char *file)
{
    char *name;int n;name=project_basename(file);n=(int)(name-file);
    if(!n){strcpy(out,".");return;}memcpy(out,file,n);out[n]=0;
}
static int filename(char *out,char *name,char *extension)
{
    char *dot;int i,n;dot=strchr(name,'.');n=dot?(int)(dot-name):(int)strlen(name);
    if(n<1||n>8||!clean(name))return error("Use a name of 1-8 letters, digits or underscores");
    if(dot&&!project_equal(dot,extension))return error("Wrong filename extension");
    for(i=0;i<n;++i){if(!isalnum((unsigned char)name[i])&&name[i]!='_'&&name[i]!='-')return error("Invalid filename character");out[i]=(char)toupper((unsigned char)name[i]);}
    strcpy(out+n,extension);return 1;
}
static int exists(char *path)
{
    FILE *f;f=fopen(path,"rb");if(!f)return 0;fclose(f);return 1;
}
static int source_name(char *path)
{
    char name[16];return filename(name,project_basename(path),".BAS");
}
void project_default(Project *p,char *entry)
{
    char normalized[BASIC_PATH];memset(p,0,sizeof(Project));strcpy(p->name,"DEFAULT");
    if(!project_join(normalized,"",entry))return;
    strcpy(p->members[0],normalized);p->count=1;directory_of(p->directory,normalized);
    project_join(p->file,p->directory,"DEFAULT.TBP");
}
int project_read_source(char *path,char text[MAX_LINES][LINE_LEN],int *count)
{
    FILE *f;int c,row,col,skip_lf;f=fopen(path,"rb");if(!f)return error("Cannot open source file");
    memset(text,0,(unsigned long)MAX_LINES*LINE_LEN);row=col=skip_lf=0;
    while((c=fgetc(f))!=EOF){
        if(skip_lf&&c=='\n'){skip_lf=0;continue;}skip_lf=0;
        if(row==MAX_LINES){fclose(f);return error("Too many source lines");}
        if(c=='\r'||c=='\n'){text[row++][col]=0;col=0;skip_lf=c=='\r';continue;}
        if(!c||(c<32&&c!='\t')){fclose(f);return error("Source is not a text file");}
        if(col==LINE_LEN-1){fclose(f);return error("Source line too long");}text[row][col++]=(char)c;
    }
    if(ferror(f)){fclose(f);return error("Error reading source file");}fclose(f);
    *count=row+(col?1:0);if(!*count)*count=1;return 1;
}
static char *relative(Project *p,char *path)
{
    int n;char prefix[BASIC_PATH];strcpy(prefix,p->directory);n=strlen(prefix);
    if(n&&prefix[n-1]!='/'&&prefix[n-1]!='\\')prefix[n++]=strchr(prefix,'\\')?'\\':'/';prefix[n]=0;
    if(strlen(path)>=(unsigned)n){char saved;int same;saved=path[n];path[n]=0;same=project_equal(path,prefix);path[n]=saved;if(same)return path+n;}
    return path;
}
int project_save(Project *p)
{
    FILE *f;char tmp[BASIC_PATH],backup[BASIC_PATH],*dot;int i,failed;
    if(!p->count||p->count>BASIC_FILES||!p->file[0])return error("Invalid project");
    strcpy(tmp,p->file);dot=strrchr(tmp,'.');if(!dot||!project_equal(dot,".TBP"))return error("Project needs .TBP extension");
    strcpy(dot,".PTM");strcpy(backup,tmp);strcpy(strrchr(backup,'.'),".PBK");
    f=fopen(tmp,"w");if(!f)return error("Cannot create project temporary file");
    failed=fprintf(f,"TTBASIC_PROJECT 1\nNAME=%s\nENTRY=%s\n",p->name,relative(p,p->members[0]))<0;
    for(i=0;i<p->count;++i)if(fprintf(f,"FILE=%s\n",relative(p,p->members[i]))<0)failed=1;
    if(fclose(f))failed=1;if(failed)return error("Cannot write project file");
    if(exists(p->file)){remove(backup);if(rename(p->file,backup))return error("Cannot back up project file");}
    if(rename(tmp,p->file)){rename(backup,p->file);return error("Cannot replace project file; check .PBK");}
    return 1;
}
static int add_member(Project *p,char *path)
{
    int i;for(i=0;i<p->count;++i)if(project_equal(p->members[i],path))return i+1;
    if(p->count==BASIC_FILES)return error("Project exceeds 16 files");
    strcpy(p->members[p->count++],path);return p->count;
}
int project_load(Project *out,char *path)
{
    Project p;FILE *f;char line[BASIC_PATH+32],entry[BASIC_PATH],full[BASIC_PATH],*value;int i,len,header;
    memset(&p,0,sizeof(p));entry[0]=0;if(!project_join(p.file,"",path))return 0;
    directory_of(p.directory,p.file);f=fopen(p.file,"r");if(!f)return error("Cannot open project file");header=0;
    while(fgets(line,sizeof(line),f)){
        len=strlen(line);while(len&&(line[len-1]=='\r'||line[len-1]=='\n'))line[--len]=0;
        if(!header){if(strcmp(line,"TTBASIC_PROJECT 1")){fclose(f);return error("Unsupported project format");}header=1;continue;}
        if(!line[0]||line[0]=='#')continue;
        value=strchr(line,'=');if(!value){fclose(f);return error("Invalid project record");}*value++=0;
        if(!strcmp(line,"NAME")){
            char name[16];if(p.name[0]||!filename(name,value,".TBP")){fclose(f);return error("Invalid project name");}
            *strchr(name,'.')=0;strcpy(p.name,name);
        }else if(!strcmp(line,"ENTRY")){
            if(entry[0]||!project_join(entry,p.directory,value)||!source_name(entry)){fclose(f);return error("Invalid project entry");}
        }else if(!strcmp(line,"FILE")){
            if(!project_join(full,p.directory,value)||!source_name(full)||!add_member(&p,full)){fclose(f);return 0;}
        }else{fclose(f);return error("Unknown project record");}
    }
    if(ferror(f)){fclose(f);return error("Error reading project file");}fclose(f);
    if(!header||!p.name[0]||!entry[0]||!p.count)return error("Incomplete project file");
    for(i=0;i<p.count;++i)if(project_equal(entry,p.members[i]))break;
    if(i==p.count)return error("Entry file is not a project member");
    if(i){strcpy(full,p.members[0]);strcpy(p.members[0],p.members[i]);strcpy(p.members[i],full);}
    for(i=0;i<p.count;++i)if(!exists(p.members[i]))return error("Project source file is missing");
    *out=p;return 1;
}
int project_create(Project *out,char *parent,char *name,ProjectMkdir mkdir_fn)
{
    Project p;char stem[16],file[16];FILE *f;
    if(!filename(file,name,".TBP"))return 0;strcpy(stem,file);*strchr(stem,'.')=0;
    memset(&p,0,sizeof(p));strcpy(p.name,stem);
    if(!project_join(p.directory,parent,stem)||!project_join(p.file,p.directory,file)||!project_join(p.members[0],p.directory,"MAIN.BAS"))return 0;
    if(!mkdir_fn||!mkdir_fn(p.directory))return error("Project folder exists or cannot be created");
    if(exists(p.members[0]))return error("Entry file already exists");
    f=fopen(p.members[0],"w");if(!f)return error("Cannot create entry file");
    if(fprintf(f,"Print \"Hello from %s!\"\nEnd\n",p.name)<0){fclose(f);return error("Cannot write entry file");}
    if(fclose(f))return error("Cannot close entry file");p.count=1;
    if(!project_save(&p))return 0;*out=p;return 1;
}
int project_new_file(Project *out,char *name)
{
    Project p;char file[16],path[BASIC_PATH];FILE *f;
    if(!filename(file,name,".BAS")||!project_join(path,out->directory,file))return 0;
    if(exists(path))return error("Source file already exists");
    p=*out;if(!add_member(&p,path))return 0;
    f=fopen(path,"w");if(!f)return error("Cannot create source file");
    if(fputs("Rem New source file\n",f)==EOF){fclose(f);remove(path);return error("Cannot write source file");}
    if(fclose(f)){remove(path);return error("Cannot close source file");}
    if(!project_save(&p)){remove(path);return 0;}*out=p;return p.count;
}
int project_add_file(Project *out,char *source)
{
    Project p;char absolute[BASIC_PATH],path[BASIC_PATH],file[16];FILE *in,*to;int c,failed,copied,index;
    if(!project_join(absolute,out->directory,source)||!filename(file,project_basename(absolute),".BAS")||!project_join(path,out->directory,file))return 0;
    if(!exists(absolute))return error("Selected source file is missing");
    p=*out;index=add_member(&p,path);if(!index)return 0;copied=!project_equal(absolute,path);
    if(copied){
        if(exists(path))return error("A different file with that name already exists in project");
        in=fopen(absolute,"rb");if(!in)return error("Cannot read selected source file");
        to=fopen(path,"wb");if(!to){fclose(in);return error("Cannot copy file into project");}failed=0;
        while((c=fgetc(in))!=EOF)if(fputc(c,to)==EOF){failed=1;break;}
        if(ferror(in))failed=1;fclose(in);if(fclose(to))failed=1;
        if(failed){remove(path);return error("Error copying source file");}
    }
    if(!project_save(&p)){if(copied)remove(path);return 0;}
    *out=p;return index;
}

/* Tolerant declaration index for editing incomplete BASIC, without executing it. */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "COMPLETE.H"
#include "PROJECT.H"
#define SYMBOLS 512
#define TOKENS 64
typedef struct {
    char name[24], owner[24], type[24], detail[80];
    int scope, visible, kind;
} Symbol;
static Symbol symbols[SYMBOLS];
static int symbol_count, scope_id, focus_scope;
static char focus_class[24], paths[BASIC_FILES][BASIC_PATH];
static int file_count;
static char tokens[TOKENS][LINE_LEN];
static char disk_source[MAX_LINES][LINE_LEN];
static int equal(char *a, char *b)
{
    while (*a && *b) if (toupper((unsigned char)*a++) != toupper((unsigned char)*b++)) return 0;
    return !*a && !*b;
}
static int starts(char *a, char *b)
{
    while (*b) if (!*a || toupper((unsigned char)*a++) != toupper((unsigned char)*b++)) return 0;
    return 1;
}
static int letter(int c) { return isalnum((unsigned char)c) || c == '_'; }
static void copy_name(char *dest, char *src) { strncpy(dest,src,23);dest[23]=0; }
static int tokenize(char *s)
{
    int n,k;
    n=0;
    while (*s && n<TOKENS) {
        if (isspace((unsigned char)*s)) { ++s;continue; }
        if (*s=='\'') break;
        k=0;
        if (*s=='"') {
            ++s;
            while (*s && *s!='"') { if(k<LINE_LEN-1)tokens[n][k++]=*s; ++s; }
            if (*s) ++s;
        } else if (letter(*s)) {
            while (letter(*s)) { if(k<LINE_LEN-1)tokens[n][k++]=*s;++s; }
        } else tokens[n][k++]=*s++;
        tokens[n][k]=0;
        if (!n && equal(tokens[n],"Rem")) return 0;
        ++n;
    }
    return n;
}
static Symbol *add(char *name,char *owner,char *type,int scope,int visible,int kind)
{
    Symbol *s;
    if (symbol_count==SYMBOLS || strlen(name)>23) return 0;
    s=&symbols[symbol_count++];memset(s,0,sizeof(*s));
    copy_name(s->name,name);copy_name(s->owner,owner);copy_name(s->type,type);
    s->scope=scope;s->visible=visible;s->kind=kind;
    strncpy(s->detail,type,79);return s;
}
static void queue(char *base,char *name)
{
    char path[BASIC_PATH],directory[BASIC_PATH];int i,n;
    n=(int)(project_basename(base)-base);
    if(n){memcpy(directory,base,n);directory[n]=0;}else strcpy(directory,".");
    if (!project_join(path,directory,name)) return;
    for(i=0;i<file_count;++i)if(project_equal(paths[i],path))return;
    if(file_count<BASIC_FILES)strcpy(paths[file_count++],path);
}
static void index_source(BasicSource src,int count,char *path,int focus,int cursor)
{
    char owner[24],type[24];int r,n,i,j,k,scope,visible,method;Symbol *s;
    owner[0]=0;scope=0;method=0;
    for(r=0;r<count;++r) {
        if(focus && r==cursor){strcpy(focus_class,owner);focus_scope=scope;}
        n=tokenize(src[r]);if(!n)continue;
        if(equal(tokens[0],"Imports") && n>1){queue(path,tokens[1]);continue;}
        if(equal(tokens[0],"End") && n>1) {
            if(equal(tokens[1],"Class") || equal(tokens[1],"Interface"))owner[0]=0;
            if(equal(tokens[1],"Sub") || equal(tokens[1],"Function")){scope=0;method=0;}
            continue;
        }
        i=0;visible=1;
        while(i<n && (equal(tokens[i],"Public") || equal(tokens[i],"Private") || equal(tokens[i],"Protected") || equal(tokens[i],"Overrides") || equal(tokens[i],"Overridable"))) {
            if(equal(tokens[i],"Private"))visible=0;
            if(equal(tokens[i],"Protected"))visible=2;
            ++i;
        }
        if(i+1>=n)continue;
        if(equal(tokens[i],"Class") || equal(tokens[i],"Interface")) {
            copy_name(owner,tokens[i+1]);add(owner,"","",0,1,1);continue;
        }
        if(equal(tokens[i],"Inherits") && owner[0]) {
            for(j=symbol_count-1;j>=0;--j)if(symbols[j].kind==1 && equal(symbols[j].name,owner)){copy_name(symbols[j].type,tokens[i+1]);break;}
            continue;
        }
        if(equal(tokens[i],"Sub") || equal(tokens[i],"Function")) {
            s=add(tokens[i+1],owner,"",0,visible,2);
            if(s){
                k=0;
                for(j=i+1;j<n && !equal(tokens[j],"Implements");++j){
                    int len;len=strlen(tokens[j]);
                    if(k+len+2>=80)break;
                    if(k && letter(tokens[j][0]) && (letter(s->detail[k-1]) || s->detail[k-1]==')'))s->detail[k++]=' ';
                    memcpy(s->detail+k,tokens[j],len);k+=len;s->detail[k]=0;
                }
            }
            if(s && equal(s->name,"New")) {
                char *args;args=strchr(s->detail,'(');
                if(args)for(j=0;j<symbol_count;++j)if(symbols[j].kind==1 && equal(symbols[j].name,owner)){
                    strcpy(symbols[j].detail,owner);
                    strncat(symbols[j].detail,args,79-strlen(owner));break;
                }
            }
            scope=++scope_id;method=1;
            for(j=i+2;j+2<n;++j)if((isalpha((unsigned char)tokens[j][0]) || tokens[j][0]=='_') && equal(tokens[j+1],"As"))add(tokens[j],owner,tokens[j+2],scope,1,0);
            continue;
        }
        if(equal(tokens[i],"Dim"))++i;
        else if(method || !owner[0])continue;
        if(i+1>=n)continue;
        for(j=i+1;j+1<n;++j)if(equal(tokens[j],"As")){
            k=j+1;if(equal(tokens[k],"New") && k+1<n)++k;
            copy_name(type,tokens[k]);add(tokens[i],owner,type,scope,visible,0);break;
        }
    }
}
static void offer(Completion *out,char *name,char *detail,char *prefix)
{
    int i;CompletionItem *item;
    if(!starts(name,prefix) || strlen(name)>23)return;
    for(i=0;i<out->count;++i)if(equal(out->item[i].name,name))return;
    if(out->count==COMPLETE_MAX)return;
    item=&out->item[out->count++];strcpy(item->name,name);strncpy(item->detail,detail,79);item->detail[79]=0;
}
static int in_context(Symbol *s)
{
    if(s->scope)return s->scope==focus_scope;
    return !s->owner[0] || equal(s->owner,focus_class);
}
static int code_position(char *line,int col)
{
    int i,quote;quote=0;
    for(i=0;i<col;++i){
        if(line[i]=='"') {
            if(quote && line[i+1]=='"'){++i;continue;}
            quote=!quote;
        } else if(!quote && line[i]=='\'')return 0;
    }
    if(quote)return 0;
    while(*line==' ' || *line=='\t')++line;
    return !(toupper((unsigned char)line[0])=='R' && toupper((unsigned char)line[1])=='E' && toupper((unsigned char)line[2])=='M' && (!line[3] || isspace((unsigned char)line[3])));
}
void complete_build(BasicSource source,int count,int row,int col,char *path,BasicReader reader,Completion *out)
{
    static char words[]="If Then Else ElseIf End While Wend For Each In To Step Next Select Case Is IsNot Do Loop Until Exit Continue Dim As New Class Interface Inherits Implements Public Private Protected Overridable Overrides Sub Function Return Call Let Print Input Rem Integer Long Boolean String Variant Object Collection True False Nothing Me MyBase And Or Not Xor Mod ReDim Preserve TypeOf Array Len Count Str CStr Val CInt Abs UBound LBound Left Right Mid UCase LCase";
    int f,n,i,j,start,end,member,depth;BasicSource src;FILE *file;
    char prefix[24],receiver[24],type[24],base[24],word[24],*p;CompletionItem temp;
    memset(out,0,sizeof(*out));
    if(row<0 || row>=count || col<0 || col>(int)strlen(source[row]))return;
    out->start=out->end=col;
    if(!code_position(source[row],col))return;
    start=col;while(start && letter(source[row][start-1]))--start;
    end=col;while(letter(source[row][end]))++end;
    if(col-start>23)return;
    memcpy(prefix,source[row]+start,col-start);prefix[col-start]=0;out->start=start;out->end=end;
    symbol_count=scope_id=focus_scope=0;focus_class[0]=0;file_count=1;strcpy(paths[0],path);
    for(f=0;f<file_count;++f){
        if(!f){src=source;n=count;}
        else {
            src=reader ? reader(paths[f],&n) : 0;
            if(!src){
                file=fopen(paths[f],"r");if(!file)continue;n=0;
                while(n<MAX_LINES && fgets(disk_source[n],LINE_LEN,file)){
                    i=strlen(disk_source[n]);while(i && (disk_source[n][i-1]=='\r' || disk_source[n][i-1]=='\n'))disk_source[n][--i]=0;++n;
                }
                fclose(file);src=disk_source;
            }
        }
        index_source(src,n,paths[f],!f,row);
    }
    member=start>0 && source[row][start-1]=='.';
    if(member){
        j=start-1;i=j;while(i && letter(source[row][i-1]))--i;
        if(i==j || j-i>23)return;
        memcpy(receiver,source[row]+i,j-i);receiver[j-i]=0;type[0]=0;
        if(equal(receiver,"Me") || equal(receiver,"MyBase"))strcpy(type,focus_class);
        else {
            for(i=symbol_count-1;i>=0;--i)if(equal(symbols[i].name,receiver) && symbols[i].kind==0 && in_context(&symbols[i])){
                strcpy(type,symbols[i].type);if(symbols[i].scope)break;
            }
        }
        if(equal(receiver,"MyBase"))for(i=0;i<symbol_count;++i)if(symbols[i].kind==1 && equal(symbols[i].name,type)){strcpy(type,symbols[i].type);break;}
        if(equal(type,"Collection")){
            offer(out,"Add","Add(value)",prefix);offer(out,"Remove","Remove(index)",prefix);offer(out,"Clear","Clear()",prefix);offer(out,"Count","Integer",prefix);offer(out,"Item","Item(index)",prefix);
        }
        for(depth=0;type[0] && depth<16;++depth){
            base[0]=0;
            for(i=0;i<symbol_count;++i){
                Symbol *s;s=&symbols[i];
                if(s->kind==1 && equal(s->name,type))strcpy(base,s->type);
                if(s->scope || !s->owner[0] || !equal(s->owner,type) || equal(s->name,"New"))continue;
                if(s->visible!=1 && !equal(focus_class,type) && !(s->visible==2 && (equal(receiver,"Me") || equal(receiver,"MyBase"))))continue;
                offer(out,s->name,s->detail,prefix);
            }
            strcpy(type,base);
        }
    }else{
        for(i=0;i<symbol_count;++i)if(in_context(&symbols[i]))offer(out,symbols[i].name,symbols[i].detail,prefix);
        p=words;
        while(*p){i=0;while(*p && *p!=' ')word[i++]=*p++;word[i]=0;if(*p)++p;offer(out,word,"BASIC",prefix);}
    }
    for(i=1;i<out->count;++i){temp=out->item[i];j=i;while(j>0 && strcmp(out->item[j-1].name,temp.name)>0){out->item[j]=out->item[j-1];--j;}out->item[j]=temp;}
}

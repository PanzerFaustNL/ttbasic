/* Persistent form model and bounded source generation; independent of GEM. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "FORM.H"
static char problem[96];
static char tokens[16][LINE_LEN];
static int ntokens,generated;
static BasicSource destination;
static FormModel candidate;
static char buffer[LINE_LEN],quoted[FORM_TEXT*2+1];
static char handlers[FORM_CONTROLS+2][FORM_NAME];
static int handler_kinds[FORM_CONTROLS+2],handler_count;
static int equal(char *a,char *b)
{
    while(*a && *b)if(toupper((unsigned char)*a++)!=toupper((unsigned char)*b++))return 0;return !*a && !*b;
}
static char *space(char *s){while(*s==' ' || *s=='\t')++s;return s;}
static int error(char *message){strncpy(problem,message,95);problem[95]=0;return 0;}
char *form_last_error(void){return problem;}
char *form_kind(int kind)
{
    static char *names[]={"Window","Button","Label","TextBox","CheckBox"};
    return kind>=GUI_WINDOW && kind<=GUI_CHECKBOX?names[kind-1]:"?";
}
char *form_event(int kind){return kind==GUI_WINDOW?"Load":kind==GUI_TEXTBOX?"Change":kind==GUI_LABEL?"":"Click";}
static int identifier(char *name)
{
    int i,n;n=strlen(name);if(n<1 || n>=FORM_NAME || (!isalpha((unsigned char)name[0]) && name[0]!='_'))return 0;
    for(i=1;i<n;++i)if(!isalnum((unsigned char)name[i]) && name[i]!='_')return 0;
    return !basic_is_reserved(name) && !equal(name,"Application");
}
static int text_valid(char *text)
{
    int i;if(strlen(text)>=FORM_TEXT)return 0;for(i=0;text[i];++i)if((unsigned char)text[i]<32 || (unsigned char)text[i]>126)return 0;return 1;
}
static int number(char *text,int *out,int minimum,int maximum)
{
    char *end;long value;if(!*text)return error("Enter a whole number");
    value=strtol(text,&end,10);if(*end || value<minimum || value>maximum)return error("Value outside allowed range");*out=(int)value;return 1;
}
static int name_used(FormModel *m,char *name,int except)
{
    int i;if(except!=-1 && equal(m->name,name))return 1;
    for(i=0;i<m->count;++i)if(i!=except && equal(m->control[i].name,name))return 1;return 0;
}
void form_default(FormModel *m)
{
    memset(m,0,sizeof(*m));strcpy(m->name,"Form1");strcpy(m->text,"TT BASIC form");
    strcpy(m->load,"Form1_Load");strcpy(m->closed,"Form1_Closed");m->width=320;m->height=200;m->next_id=1;
}
int form_validate(FormModel *m)
{
    int i,j;FormControl *c;char *handler;
    if(!identifier(m->name) || !text_valid(m->text))return error("Invalid form name or title (ASCII, max 63 chars)");
    if(m->width<120 || m->width>600 || m->height<64 || m->height>400)return error("Form size: width 120..600, height 64..400");
    if(m->count<0 || m->count>FORM_CONTROLS || m->next_id<1 || m->next_id>9999)return error("Invalid form control count or sequence");
    for(i=-2;i<m->count;++i){
        handler=i==-2?m->load:i==-1?m->closed:m->control[i].handler;
        if(*handler && (!identifier(handler) || name_used(m,handler,-2)))return error("Handler must be an identifier distinct from object names");
        if(i<0)continue;c=&m->control[i];
        if(c->kind<GUI_BUTTON || c->kind>GUI_CHECKBOX || !identifier(c->name) || !text_valid(c->text))return error("Invalid control type, name or text");
        if(name_used(m,c->name,i))return error("Form and control names must be unique");
        if(c->x<0 || c->y<0 || c->width<16 || c->height<16 || c->x>m->width-c->width || c->y>m->height-c->height)return error("Control must fit inside the form");
        if(c->kind==GUI_TEXTBOX && (int)strlen(c->text)>(c->width-8)/8)return error("Text does not fit in TextBox width");
        if(c->kind==GUI_LABEL && *c->handler)return error("Label has no event handler");
        j=c->enabled;if(j<0 || j>1 || c->visible<0 || c->visible>1 || c->checked<0 || c->checked>1)return error("Boolean properties must be 0 or 1");
    }return 1;
}
int form_add(FormModel *m,int kind,int x,int y)
{
    FormControl *c;
    if(m->count==FORM_CONTROLS)return error("Maximum 16 controls in the designer");
    if(kind<GUI_BUTTON || kind>GUI_CHECKBOX)return error("Choose a control tool");
    candidate=*m;c=&candidate.control[candidate.count];memset(c,0,sizeof(*c));c->kind=kind;
    do{if(candidate.next_id>=9999)return error("Control name sequence exhausted");sprintf(c->name,"%s%d",form_kind(kind),candidate.next_id++);}while(name_used(&candidate,c->name,candidate.count));
    strcpy(c->text,c->name);c->width=kind==GUI_BUTTON?96:kind==GUI_CHECKBOX?160:144;c->height=24;
    if(c->width>m->width)c->width=m->width;
    c->x=x;c->y=y;if(c->x>m->width-c->width)c->x=m->width-c->width;if(c->y>m->height-c->height)c->y=m->height-c->height;
    c->enabled=c->visible=1;
    if(kind!=GUI_LABEL)sprintf(c->handler,"%s_%s",c->name,form_event(kind));
    ++candidate.count;if(!form_validate(&candidate))return 0;*m=candidate;return 1;
}
int form_delete(FormModel *m,int index)
{
    if(index<0 || index>=m->count)return error("Select a control first");
    --m->count;memmove(m->control+index,m->control+index+1,(m->count-index)*sizeof(FormControl));return 1;
}
int form_bounds(FormModel *m,int index,int x,int y,int width,int height)
{
    if(index<0 || index>=m->count)return error("Select a control first");candidate=*m;
    candidate.control[index].x=x;candidate.control[index].y=y;candidate.control[index].width=width;candidate.control[index].height=height;
    if(!form_validate(&candidate))return 0;*m=candidate;return 1;
}
int form_set(FormModel *m,int index,int property,char *value)
{
    FormControl *c;char *target;int *integer,maximum;
    if(index< -1 || index>=m->count)return error("No selection");candidate=*m;c=index<0?0:&candidate.control[index];target=0;integer=0;maximum=600;
    if(property==FORM_NAME_PROP)target=c?c->name:candidate.name;
    else if(property==FORM_TEXT_PROP)target=c?c->text:candidate.text;
    else if(property==FORM_HANDLER)target=c?c->handler:candidate.load;
    else if(property==FORM_CLOSED && !c)target=candidate.closed;
    else if(property==FORM_WIDTH)integer=c?&c->width:&candidate.width;
    else if(property==FORM_HEIGHT){integer=c?&c->height:&candidate.height;maximum=400;}
    else if(property==FORM_X && c)integer=&c->x;
    else if(property==FORM_Y && c)integer=&c->y;
    else if(property==FORM_ENABLED && c){integer=&c->enabled;maximum=1;}
    else if(property==FORM_VISIBLE && c){integer=&c->visible;maximum=1;}
    else if(property==FORM_CHECKED && c && c->kind==GUI_CHECKBOX){integer=&c->checked;maximum=1;}
    else return error("Property not available for this selection");
    if(target){if(strlen(value)>=(unsigned)(property==FORM_TEXT_PROP?FORM_TEXT:FORM_NAME))return error("Property text is too long");strcpy(target,value);}
    if(integer && !number(value,integer,0,maximum))return 0;
    if(!form_validate(&candidate))return 0;*m=candidate;return 1;
}
static int tokenize(char *line)
{
    int n,k,closed;char *p;p=line;n=0;
    while(*(p=space(p))){
        if(n==16)return error("Too many form metadata fields");k=0;
        if(*p=='"'){
            closed=0;++p;while(*p){if(*p=='"'){++p;if(*p!='"'){closed=1;break;}}if(k==LINE_LEN-1)return error("Form metadata text is too long");tokens[n][k++]=*p++;}
            if(!closed)return error("Unclosed form metadata string");
            if(*p && *p!=' ' && *p!='\t')return error("Invalid quoted metadata");
        }else while(*p && *p!=' ' && *p!='\t'){if(k==LINE_LEN-1)return error("Form metadata token is too long");tokens[n][k++]=*p++;}
        tokens[n++][k]=0;
    }ntokens=n;return 1;
}
static int markers(BasicSource source,int count,int *begin,int *end)
{
    int i;char line[LINE_LEN],*p;*begin=*end=-1;
    if(count<0 || count>MAX_LINES)return error("Invalid source line count");
    for(i=0;i<count;++i){
        if(!memchr(source[i],0,LINE_LEN))return error("Source line exceeds 159 characters");
        strcpy(line,space(source[i]));p=line+strlen(line);while(p>line && (p[-1]==' ' || p[-1]=='\t'))*--p=0;
        if(equal(line,"Rem @TTFORM BEGIN 1")){if(*begin>=0 || *end>=0)return error("Duplicate form marker");*begin=i;}
        if(equal(line,"Rem @TTFORM END")){if(*begin<0 || *end>=0)return error("Unmatched form marker");*end=i;}
    }
    if((*begin<0)!=(*end<0))return error("Missing form marker");return 1;
}
int form_read(BasicSource source,int count,FormModel *out)
{
    int begin,end,i,j,id,seen[FORM_CONTROLS+1],window;FormControl *c;char prefix[12],*p;
    if(!markers(source,count,&begin,&end))return -1;if(begin<0)return 0;
    memset(&candidate,0,sizeof(candidate));memset(seen,0,sizeof(seen));window=0;
    for(i=begin+1;i<end;++i){
        p=space(source[i]);if(strlen(p)<11)continue;memcpy(prefix,p,11);prefix[11]=0;
        if(!equal(prefix,"Rem @TTFORM") || (p[11] && p[11]!=' ' && p[11]!='\t'))continue;
        if(!tokenize(source[i]))return -1;
        if(ntokens<2 || !equal(tokens[0],"Rem") || !equal(tokens[1],"@TTFORM"))continue;
        if(ntokens<3)return error("Invalid form metadata"),-1;
        if(equal(tokens[2],"W")){
            if(window || ntokens!=9 || strlen(tokens[3])>=FORM_NAME)return error("Invalid form header"),-1;
            strcpy(candidate.name,tokens[3]);
            if(!number(tokens[4],&candidate.width,120,600) || !number(tokens[5],&candidate.height,64,400) || !number(tokens[6],&candidate.next_id,1,9999))return -1;
            if(strlen(tokens[7])>=FORM_NAME || strlen(tokens[8])>=FORM_NAME)return error("Handler name too long"),-1;
            if(strcmp(tokens[7],"-"))strcpy(candidate.load,tokens[7]);if(strcmp(tokens[8],"-"))strcpy(candidate.closed,tokens[8]);window=1;
        }else if(equal(tokens[2],"C")){
            if(!window || ntokens!=13 || candidate.count==FORM_CONTROLS)return error("Invalid form control metadata"),-1;
            c=&candidate.control[candidate.count++];
            if(!number(tokens[3],&c->kind,GUI_BUTTON,GUI_CHECKBOX) || strlen(tokens[4])>=FORM_NAME)return error("Invalid control type/name"),-1;
            strcpy(c->name,tokens[4]);
            if(!number(tokens[5],&c->x,0,600) || !number(tokens[6],&c->y,0,400) || !number(tokens[7],&c->width,16,600) || !number(tokens[8],&c->height,16,400))return -1;
            if(!number(tokens[9],&c->enabled,0,1) || !number(tokens[10],&c->visible,0,1) || !number(tokens[11],&c->checked,0,1))return -1;
            if(strlen(tokens[12])>=FORM_NAME)return error("Handler name too long"),-1;if(strcmp(tokens[12],"-"))strcpy(c->handler,tokens[12]);
        }else if(equal(tokens[2],"T")){
            if(ntokens!=5 || !number(tokens[3],&id,0,candidate.count) || seen[id] || !text_valid(tokens[4]))return error("Invalid or duplicate form text metadata"),-1;
            strcpy(id?candidate.control[id-1].text:candidate.text,tokens[4]);seen[id]=1;
        }else return error("Unknown form metadata version or record"),-1;
    }
    if(!window)return error("Form header is missing"),-1;
    for(j=0;j<=candidate.count;++j)if(!seen[j])return error("Form text metadata is missing"),-1;
    if(!form_validate(&candidate))return -1;*out=candidate;return 1;
}
static int emit(char *line)
{
    if(strlen(line)>=LINE_LEN)return error("Generated line exceeds 159 characters");
    if(generated==MAX_LINES)return error("Form plus event code exceeds 256 source lines");
    strcpy(destination[generated++],line);return 1;
}
static void quote(char *text)
{
    int i,k;k=0;quoted[k++]='"';for(i=0;text[i];++i){if(text[i]=='"')quoted[k++]='"';quoted[k++]=text[i];}quoted[k++]='"';quoted[k]=0;
}
static int read_word(char **cursor,char *word)
{
    int n;char *p;p=space(*cursor);n=0;
    while(isalnum((unsigned char)*p) || *p=='_'){if(n<FORM_NAME-1)word[n]=*p;++n;++p;}
    *cursor=p;word[n<FORM_NAME?n:FORM_NAME-1]=0;return n>0 && n<FORM_NAME;
}
int form_handler_line(BasicSource source,int count,char *name)
{
    int i,nested,sub;char first[FORM_NAME],second[FORM_NAME],*p;nested=0;
    for(i=0;i<count;++i){
        p=source[i];if(!read_word(&p,first))continue;
        while(equal(first,"Public") || equal(first,"Private") || equal(first,"Protected"))if(!read_word(&p,first))break;
        if(!read_word(&p,second))continue;
        if(equal(first,"Class") || equal(first,"Interface")){nested=1;continue;}
        if(equal(first,"End") && (equal(second,"Class") || equal(second,"Interface"))){nested=0;continue;}
        sub=equal(first,"Sub");if(!nested && equal(second,name) && (sub || equal(first,"Function")))return sub?i:-2;
    }return -1;
}
static void add_handler(char *name,int kind)
{
    int i;if(!*name)return;
    for(i=0;i<handler_count;++i)if(equal(handlers[i],name)){if(handler_kinds[i]!=kind)handler_kinds[i]=0;return;}
    strcpy(handlers[handler_count],name);handler_kinds[handler_count++]=kind;
}
int form_generate(FormModel *m,BasicSource source,int count,BasicSource output,int *used)
{
    int begin,end,i,line;FormControl *c;*used=0;
    if(source==output)return error("Generate into a separate source buffer");
    if(!form_validate(m) || !markers(source,count,&begin,&end))return 0;
    if(begin<0 && count)return error("Source is not a designer form; create a new form project");
    destination=output;generated=0;handler_count=0;
#define LINE(s) if(!emit(s))return 0
    for(i=0;i<begin;++i){LINE(source[i]);}
    LINE("Rem @TTFORM BEGIN 1");LINE("Rem Layout generated by the form designer (Ctrl+D).");
    sprintf(buffer,"Rem @TTFORM W %s %d %d %d %s %s",m->name,m->width,m->height,m->next_id,*m->load?m->load:"-",*m->closed?m->closed:"-");LINE(buffer);
    quote(m->text);sprintf(buffer,"Rem @TTFORM T 0 %s",quoted);LINE(buffer);
    for(i=0;i<m->count;++i){c=&m->control[i];
        sprintf(buffer,"Rem @TTFORM C %d %s %d %d %d %d %d %d %d %s",c->kind,c->name,c->x,c->y,c->width,c->height,c->enabled,c->visible,c->checked,*c->handler?c->handler:"-");LINE(buffer);
        quote(c->text);sprintf(buffer,"Rem @TTFORM T %d %s",i+1,quoted);LINE(buffer);
    }
    sprintf(buffer,"Dim %s As New Window(\"\", %d, %d)",m->name,m->width,m->height);LINE(buffer);
    quote(m->text);sprintf(buffer,"%s.Text = %s",m->name,quoted);LINE(buffer);
    for(i=0;i<m->count;++i){c=&m->control[i];
        sprintf(buffer,"Dim %s As New %s(%s, \"\", %d, %d, %d, %d)",c->name,form_kind(c->kind),m->name,c->x,c->y,c->width,c->height);LINE(buffer);
        quote(c->text);sprintf(buffer,"%s.Text = %s",c->name,quoted);LINE(buffer);
        if(!c->enabled){sprintf(buffer,"%s.Enabled = False",c->name);LINE(buffer);}
        if(!c->visible){sprintf(buffer,"%s.Visible = False",c->name);LINE(buffer);}
        if(c->kind==GUI_CHECKBOX && c->checked){sprintf(buffer,"%s.Checked = True",c->name);LINE(buffer);}
        if(*c->handler){sprintf(buffer,"AddHandler %s.%s, AddressOf %s",c->name,form_event(c->kind),c->handler);LINE(buffer);add_handler(c->handler,c->kind);}
    }
    if(*m->load){sprintf(buffer,"AddHandler %s.Load, AddressOf %s",m->name,m->load);LINE(buffer);add_handler(m->load,GUI_WINDOW);}
    if(*m->closed){sprintf(buffer,"AddHandler %s.Closed, AddressOf %s",m->name,m->closed);LINE(buffer);add_handler(m->closed,GUI_WINDOW);}
    sprintf(buffer,"%s.Show()",m->name);LINE(buffer);LINE("Application.Run()");LINE("End");LINE("Rem @TTFORM END");
    for(i=end+1;end>=0 && i<count;++i){LINE(source[i]);}
    for(i=0;i<handler_count;++i){
        line=form_handler_line(source,count,handlers[i]);if(line==-2)return error("Handler name is already used by a Function");if(line>=0)continue;
        LINE("");sprintf(buffer,"Sub %s(sender As %s)",handlers[i],handler_kinds[i]?form_kind(handler_kinds[i]):"Object");LINE(buffer);
        LINE("    ");LINE("End Sub");
    }
#undef LINE
    *used=generated;return 1;
}

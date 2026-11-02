#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "BASIC.H"
static char source[MAX_LINES][LINE_LEN],out[OUT_LINES][OUT_LEN],base[BASIC_PATH];
static int used,checks;
static int run(char *text)
{
    char *p,*end; int count;
    memset(source,0,sizeof(source)); count=0; p=text;
    while (*p) {
        end=strchr(p,'\n'); if (!end) end=p+strlen(p);
        assert(count<MAX_LINES && end-p<LINE_LEN);
        memcpy(source[count++],p,end-p); p=*end ? end+1 : end;
    }
    ++checks; return basic_run_project(source,count,base,out,&used);
}
static void ok(char *text,char *expected)
{
    int result; result=run(text);
    if (result) fprintf(stderr,"Unexpected error: %s\n",out[used-1]);
    assert(!result);
    if (strcmp(out[0],expected)) fprintf(stderr,"Expected %s, got %s\n",expected,out[0]);
    assert(!strcmp(out[0],expected));
}
static void bad(char *text,char *expected)
{
    int result; result=run(text);
    if (!result) fprintf(stderr,"Expected error: %s\n",text);
    assert(result);
    if (!strstr(out[used-1],expected)) fprintf(stderr,"Expected %s, got %s\n",expected,out[used-1]);
    assert(strstr(out[used-1],expected));
}
static void file(char *name,char *text)
{
    char path[BASIC_PATH],*p; FILE *f;
    strcpy(path,base); p=strrchr(path,'/'); if (!p) p=strrchr(path,'\\'); assert(p);
    strcpy(p+1,name); f=fopen(path,"w"); assert(f); fputs(text,f); assert(!fclose(f));
}
static int input(char *prompt,char *buffer,int capacity)
{
    assert(capacity>=8); assert(prompt[0]); strcpy(buffer,"Marco"); return 1;
}
static int numeric_input(char *prompt,char *buffer,int capacity)
{
    (void)prompt; assert(capacity>=8); strcpy(buffer,"42"); return 1;
}
static int cancelled_input(char *prompt,char *buffer,int capacity)
{
    (void)prompt;(void)buffer;(void)capacity;return 0;
}
static int stop(void) { return 0; }
int main(int argc,char **argv)
{
    assert(argc==2); strcpy(base,argv[1]);
    ok("If 2<3 Then\nPrint 42\nElse\nPrint 0\nEnd If", "42");
    ok("If False Then\nPrint 0\nElseIf 4=4 Then\nPrint 42\nElse\nPrint 1\nEnd If", "42");
    ok("If False Then\nPrint 0\nElseIf False Then\nPrint 1\nElse\nIf True Then\nPrint 42\nEnd If\nEnd If", "42");
    ok("If True Then Print \"Else Then\" Else Print 0", "Else Then");
    ok("If False Then Print 0 Else Print 42", "42");
    bad("If True\nPrint 1\nEnd If", "expects condition Then");
    bad("Else\nPrint 1", "without matching If");
    bad("If True Then\nPrint 1", "Missing control-flow");
    ok("Dim i As Integer\nWhile i<3\ni=i+1\nWend\nPrint i+39", "42");
    ok("Dim i As Integer\nWhile i<10\ni=i+1\nIf i=3 Then Exit While\nEnd While\nPrint i+39", "42");
    ok("Dim sum As Integer\nFor i=1 To 3\nsum=sum+i\nNext i\nPrint sum*7", "42");
    ok("Dim sum As Integer\nFor i As Integer=6 To 1 Step -1\nsum=sum+i\nNext\nPrint sum*2", "42");
    ok("Dim sum As Integer\nFor i=4 To 1\nsum=100\nNext\nPrint sum+42", "42");
    ok("Dim sum As Integer\nFor i=1 To 6\nIf i Mod 2=0 Then Continue For\nsum=sum+i\nNext\nPrint sum", "9");
    ok("Dim sum As Integer\nFor i=1 To 3\nFor j=1 To 2\nDim term As Integer=7\nsum=sum+term\nNext j\nNext i\nPrint sum", "42");
    bad("For i=1 To 3 Step 0\nNext", "Step cannot be zero");
    bad("For i=1 To 3\nNext j", "Next variable does not match");
    bad("While True\nNext", "Mismatched control-flow");
    bad("Exit For", "no matching block");
    ok("Dim n As Integer\nDo\nn=n+1\nLoop Until n=3\nPrint n+39", "42");
    ok("Dim n As Integer\nDo While n<3\nn=n+1\nLoop\nPrint n+39", "42");
    ok("Dim n As Integer\nDo Until n=3\nn=n+1\nLoop\nPrint n+39", "42");
    ok("Dim n As Integer\nDo\nn=n+1\nIf n=3 Then Exit Do\nLoop\nPrint n+39", "42");
    bad("Do While True\nLoop Until True", "both pre and post");
    ok("Select Case 5\nCase 1,2\nPrint 0\nCase 3 To 7\nPrint 42\nCase Else\nPrint 1\nEnd Select", "42");
    ok("Select Case 8\nCase Is < 3\nPrint 0\nCase Is >= 8\nPrint 42\nEnd Select", "42");
    ok("Select Case \"TT\"\nCase \"ST\"\nPrint 0\nCase \"TT\"\nPrint 42\nEnd Select", "42");
    ok("Select Case 9\nCase 1\nPrint 0\nCase Else\nPrint 42\nEnd Select", "42");
    bad("Case 1\nPrint 1", "without matching Select");
    bad("Select Case 1\nPrint 1\nEnd Select", "requires a Case");
    ok("Dim greeting As String=\"Hello\"\ngreeting=greeting & \" TT\"\nPrint greeting", "Hello TT");
    ok("Dim quoted As String=\"A \"\"quote\"\"\"\nPrint quoted", "A \"quote\"");
    ok("Print CStr(6*7) & \"!\"", "42!");
    ok("Print Len(\"Atari\")\nPrint Mid(\"Atari\",2,3)\nPrint Right(\"Atari\",2)", "5");
    assert(!strcmp(out[1],"tar") && !strcmp(out[2],"ri"));
    ok("Print UCase(Left(\"atari\",2))\nPrint LCase(\"TT\")\nPrint Val(\" 42 \")", "AT");
    assert(!strcmp(out[1],"tt") && !strcmp(out[2],"42"));
    ok("Print Not False And True\nPrint False Or True\nPrint True Xor True", "-1");
    assert(!strcmp(out[1],"-1") && !strcmp(out[2],"0"));
    bad("Dim If As Integer", "Reserved word");
    bad("Let While=1", "Reserved word");
    bad("Dim n As Integer=\"text\"", "requires number");
    bad("Print CInt(\"abc\")", "Invalid integer text");
    ok("Dim values(2) As Integer\nvalues(0)=40\nvalues(2)=2\nPrint values(0)+values(2)", "42");
    ok("Dim values(1 To 3) As Integer\nvalues(3)=42\nPrint values(3)\nPrint LBound(values)\nPrint UBound(values)", "42");
    assert(!strcmp(out[1],"1") && !strcmp(out[2],"3"));
    ok("Dim words(1) As String\nwords(0)=\"Atari\"\nwords(1)=\"TT\"\nPrint words(0) & \" \" & words(1)", "Atari TT");
    ok("Dim values(2) As Integer\nvalues(0)=10\nvalues(1)=12\nvalues(2)=20\nDim sum As Integer\nFor Each value As Integer In values\nsum=sum+value\nNext value\nPrint sum", "42");
    ok("Dim values As Variant=Array(20,22)\nDim sum As Integer\nFor Each value In values\nsum=sum+value\nNext\nPrint sum", "42");
    ok("Dim a() As Integer\nReDim a(1)\na(1)=42\nReDim Preserve a(3)\nPrint a(1)\nPrint UBound(a)", "42");
    assert(!strcmp(out[1],"3"));
    bad("Dim a(1) As Integer\nPrint a(2)", "index out of bounds");
    bad("Dim a(1) As Integer\nPrint a(65536)", "index out of bounds");
    bad("Dim a(1) As Integer\na(0)=\"x\"", "requires number");
    bad("Dim a() As Integer\nPrint a(0)", "not allocated");
    bad("Dim a(128) As Integer", "exceed 128 items");
    ok("Dim a(100000 To 100001) As Integer\na(100001)=42\nPrint a(100001)", "42");
    ok("Dim list As New Collection\nlist.Add(20)\nlist.Add(22)\nDim sum As Integer\nFor Each item In list\nsum=sum+item\nNext\nPrint sum\nPrint list.Count", "42");
    assert(!strcmp(out[1],"2"));
    ok("Dim list As New Collection\nlist.Add(\"TT\")\nlist.Add(42)\nPrint list.Item(1)\nlist.Remove(1)\nPrint list(1)", "TT");
    assert(!strcmp(out[1],"42"));
    bad("Dim list As New Collection\nlist.Add(1)\nFor Each item In list\nlist.Add(2)\nNext", "changed during For Each");
    basic_set_input(input);
    ok("Dim name As String\nInput \"Your name?\"; name\nPrint name", "Marco");
    basic_set_input(numeric_input);
    ok("Dim age As Integer\nInput age\nPrint age", "42");
    basic_set_input(cancelled_input); bad("Dim name As String\nInput name", "Input cancelled");basic_set_input(0);
    basic_set_poll(stop); bad("While True\nWend", "Execution cancelled");basic_set_poll(0);
    file("ANIMAL.BAS","Class Animal\nProtected name As String\nPublic Sub New(label As String)\nMe.name=label\nEnd Sub\nPublic Overridable Function Speak() As String\nReturn Me.name\nEnd Function\nEnd Class\n");
    file("DOG.BAS","Imports \"ANIMAL.BAS\"\nClass Dog\nInherits Animal\nPublic Sub New(label As String)\nMyBase.New(label)\nEnd Sub\nPublic Overrides Function Speak() As String\nReturn MyBase.Speak() & \" woof\"\nEnd Function\nEnd Class\n");
    ok("Imports \"DOG.BAS\"\nDim actor As Animal=New Dog(\"Rex\")\nPrint actor.Speak()", "Rex woof");
    ok("Imports \"DOG.BAS\"\nDim actor As Animal=New Dog(\"Rex\")\nPrint TypeOf actor Is Animal\nPrint TypeOf actor Is Dog", "-1");
    assert(!strcmp(out[1],"-1"));
    file("CONTRACT.BAS","Interface IShow\nFunction Show() As String\nEnd Interface\nClass Item\nImplements IShow\nPublic Function Show() As String\nReturn \"shown\"\nEnd Function\nEnd Class\n");
    ok("Imports \"CONTRACT.BAS\"\nDim item As IShow=New Item\nPrint item.Show()", "shown");
    file("MAPPED.BAS","Imports \"CONTRACT.BAS\"\nClass Other\nImplements IShow\nPublic Function Display() As String Implements IShow.Show\nReturn \"mapped\"\nEnd Function\nEnd Class\n");
    ok("Imports \"MAPPED.BAS\"\nDim item As IShow=New Other\nPrint item.Show()", "mapped");
    bad("Interface IFoo\nSub Foo()\nEnd Interface\nClass Missing\nImplements IFoo\nEnd Class", "does not implement");
    bad("Imports \"CONTRACT.BAS\"\nDim x As New IShow", "Cannot instantiate");
    bad("Class A\nInherits B\nEnd Class\nClass B\nInherits A\nEnd Class", "Circular inheritance");
    bad("Class A\nPublic Function F() As Integer\nReturn 1\nEnd Function\nEnd Class\nClass B\nInherits A\nPublic Overrides Function F() As Integer\nReturn 2\nEnd Function\nEnd Class", "Invalid Overrides");
    bad("Imports \"DOG.BAS\"\nDim dog As New Dog(\"Rex\")\nPrint dog.name", "Private member");
    ok("Imports \"DOG.BAS\"\nDim list As New Collection\nlist.Add(New Dog(\"Rex\"))\nFor Each actor As Animal In list\nPrint actor.Speak()\nNext", "Rex woof");
    ok("Function Twice(n As Integer) As Integer\nIf n>0 Then\nReturn n*2\nElse\nReturn 0\nEnd If\nEnd Function\nPrint Twice(21)", "42");
    ok("Sub Show(items() As Integer)\nFor Each value In items\nPrint value\nNext\nEnd Sub\nDim nums(0) As Integer\nnums(0)=42\nShow(nums)", "42");
    printf("%d extended language checks passed.\n",checks);return 0;
}

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
    if (result) fprintf(stderr,"Unexpected error in %s: %s\n",text,out[used-1]);
    assert(!result);
    if (strcmp(out[0],expected)) fprintf(stderr,"In %s: expected %s, got %s\n",text,expected,out[0]);
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
static char input_text[128];
static int input(char *prompt,char *buffer,int capacity)
{
    (void)prompt;assert((int)strlen(input_text)<capacity);strcpy(buffer,input_text);return 1;
}
int main(int argc,char **argv)
{
    assert(argc==2);strcpy(base,argv[1]);
    ok("Print 0.5 + .25","0.75");
    ok("Print 1.","1");
    ok("Print 1e2 + 2.5E-1","100.25");
    ok("Print -1.5e+2","-150");
    ok("Print 7 / 2","3.5");
    ok("Print 7 \\ 2","3");
    ok("Print 7 \\ -2","-3");
    ok("Print -7 Mod 2","-1");
    ok("Print -7 Mod -2","-1");
    ok("Print 2147483647 + 1","2147483648");
    ok("Print -2147483648 \\ -1","2147483648");
    ok("Print -2147483648 Mod -1","0");
    ok("Dim x As Double = 1.25\nPrint x * 2","2.5");
    ok("Dim x As Single = .5\nPrint x","0.5");
    ok("x = .75\nPrint x","0.75");
    ok("Dim x As Integer = 2147483647\nPrint x","2147483647");
    ok("Dim x As Long = -2147483648\nPrint x","-2147483648");
    ok("Print Val(\" -1.25e2 \")","-125");
    ok("Print CDbl(\".125\")","0.125");
    ok("Print CSng(\".5\")","0.5");
    ok("Print CInt(2.5)","2");
    ok("Print CInt(3.5)","4");
    ok("Print CInt(-2.5)","-2");
    ok("Print CLng(-3.5)","-4");
    ok("Print CInt(2.6)","3");
    ok("Print CInt(-2.6)","-3");
    ok("Print CInt(\"3.5\")","4");
    ok("Print Fix(-1.75)","-1");
    ok("Print Int(-1.75)","-2");
    ok("Print Abs(-.5)","0.5");
    ok("Print CStr(.125)","0.125");
    ok("Print .1 < .5","-1");
    ok("Print Not .5","0");
    ok("Dim a(2) As Double\na(1) = .75\nPrint a(1)","0.75");
    ok("Dim a() As Single\nReDim a(1 To 2)\na(1)=.5\nReDim Preserve a(1 To 3)\nPrint a(1)","0.5");
    ok("Dim total As Double\nFor i As Double = .25 To 1 Step .25\ntotal=total+i\nNext\nPrint total","2.5");
    ok("Dim total As Double\nFor i As Single = 1 To .25 Step -.25\ntotal=total+i\nNext\nPrint total","2.5");
    ok("Dim total As Double\nDim values As Variant = Array(.5,.25)\nFor Each v In values\ntotal=total+v\nNext\nPrint total","0.75");
    ok("Print Half(1.5)\nEnd\nFunction Half(x As Double) As Double\nReturn x / 2\nEnd Function","0.75");
    ok("Dim x As Double = .5\nSelect Case x\nCase .25 To .75\nPrint \"matched\"\nCase Else\nPrint \"wrong\"\nEnd Select","matched");
    ok("Class Box\nPublic value As Double = -.25\nEnd Class\nDim b As New Box\nPrint b.value","-0.25");
    ok("Class Box\nPublic values As Variant\nPublic Sub New()\nMe.values=Array(.5,1.5)\nMe.values(0)=Me.values(1)/2\nEnd Sub\nEnd Class\nDim b As New Box\nPrint b.values(0)","0.75");
    ok("Class Box\nPublic values(1 To 2) As Double\nEnd Class\nDim a As New Box\nDim b As New Box\na.values(1)=.75\nPrint b.values(1)\nPrint a.values(1)","0");
    ok("Class Box\nPrivate values() As Double\nPublic Sub New()\nReDim Me.values(2)\nMe.values(1)=.5\nReDim Preserve Me.values(3)\nEnd Sub\nPublic Function GetValue() As Double\nReturn Me.values(1)\nEnd Function\nEnd Class\nDim b As New Box\nPrint b.GetValue()","0.5");
    ok("Class Base\nPublic values(1) As Double\nEnd Class\nClass Child\nInherits Base\nEnd Class\nDim b As New Child\nb.values(1)=.25\nPrint b.values(1)","0.25");
    bad("Print 1e","Invalid number");
    bad("Print 1e+","Invalid number");
    bad("Print 1e301","numeric range");
    bad("Print 1e-301","numeric range");
    bad("Print 1e300 * 10","numeric range");
    bad("Print 1e-300 / 10","underflow");
    bad("Print 1e-300 * 1e-300","underflow");
    bad("Print .5 / 0","Division by zero");
    bad("Print .5 Mod 2","whole number");
    bad("Print 2 \\ .5","whole number");
    bad("Dim x As Integer = .5","whole number");
    bad("Dim x As Long = 2147483648","32-bit range");
    bad("Dim x As Single = 1e40","Single numeric range");
    bad("Dim x As Single = 1e-40","Single numeric range");
    bad("Print CInt(2147483647.5)","32-bit range");
    bad("Print CLng(-2147483648.6)","32-bit range");
    bad("Print Val(\"1.5junk\")","Invalid numeric text");
    bad("Print CDbl(\"NaN\")","Invalid numeric text");
    bad("Print CDbl(\"Infinity\")","Invalid numeric text");
    bad("Dim a(2) As Double\nPrint a(.5)","whole number");
    bad("Dim a(.5) As Double","whole number");
    bad("Dim a(1) As Integer\na(0)=.5","whole number");
    bad("Midi.NoteOn(1,60.5,96)","whole number");
    bad("Print Left(\"abc\",.5)","whole number");
    bad("Dim total As Integer\nFor i As Integer = 0 To 2 Step .5\nNext","whole number");
    bad("Class Box\nPrivate values As Variant\nPublic Sub New()\nMe.values=Array(1,2)\nEnd Sub\nEnd Class\nDim b As New Box\nPrint b.values(0)","Private member");
    bad("Class Box\nPublic values(2) As Double\nEnd Class\nDim b As New Box\nPrint b.values(3)","out of bounds");
    bad("Class Box\nPublic value As Integer\nEnd Class\nDim b As New Box\nPrint b.value(0)","Unexpected text");
    bad("Class Box\nPublic values(128) As Double\nEnd Class","128 items");
    ok("Print 2^3^2","512");
    ok("Print -2^2","-4");
    ok("Print (-2)^2","4");
    ok("Print 2^-2","0.25");
    ok("Print 2^3*2","16");
    ok("Print Sqr(2.25)","1.5");
    ok("Print Sin(0) + Cos(0)","1");
    ok("Print Tan(0) + Atn(0)","0");
    ok("Print Exp(0) + Log(1)","1");
    ok("Print Sgn(-.5)","-1");
    ok("Print 16 \\ 2 * 2","4");
    ok("Print 10 Mod 3 * 2","4");
    ok("Print 10 Mod 9 \\ 3","1");
    bad("Print Sqr(-1)","domain");
    bad("Print Log(0)","domain");
    bad("Print (-2)^.5","domain");
    bad("Print 0^-1","Division by zero");
    bad("Print 1e300^2","numeric range");
    bad("Dim x As Single\nFor x = 16777216 To 16777220 Step 1\nNext","precision");
    bad("Print Mid(\"abc\",-2147483648)","String bounds");
    ok("Print CSng(16777217)","16777216");
    ok("Print CSng(16777219)","16777220");
    ok("Dim n As Integer\nFor i As Single = 16777217 To 16777216 Step 2\nn=n+1\nNext\nPrint n","1");
    basic_set_input(input);strcpy(input_text,"1.25");
    ok("Dim x As Double\nInput x\nPrint x","1.25");
    bad("Dim x As Integer\nInput x","whole number");
    strcpy(input_text,"-.5e1");ok("Dim x As Variant\nInput x\nPrint x","-5");
    basic_set_input(0);
    printf("%d numeric and object-array checks passed.\n",checks);return 0;
}

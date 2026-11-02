#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "BASIC.H"
static char src[MAX_LINES][LINE_LEN], out[OUT_LINES][OUT_LEN];
static int used, checks;
static char base[BASIC_PATH];
static int run(char *s) {
    char *p, *q; int n = 0, result;
    memset(src, 0, sizeof(src)); p = s;
    while (*p) { q = strchr(p, '\n'); if (!q) q = p + strlen(p);
        assert(q-p < LINE_LEN && n < MAX_LINES);
        memcpy(src[n++], p, q-p); p = *q ? q+1 : q;
    }
    result = basic_run_project(src, n, base, out, &used);
    ++checks; return result;
}
static void file(char *name, char *text) {
    char path[BASIC_PATH], *end; FILE *f;
    strcpy(path, base); end = strrchr(path, '/');
    if (!end) end = strrchr(path, '\\'); assert(end); strcpy(end+1, name);
    f=fopen(path,"w"); assert(f);fputs(text,f);assert(!fclose(f));
}
static void ok(char *s, char *first) {
    int result=run(s); if(result) fprintf(stderr,"Unexpected error: %s\n",out[used-1]);
    assert(!result); assert(!strcmp(out[0],first));
}
static void bad(char *s, char *message) {
    int result=run(s);if(!result)fprintf(stderr,"Expected failure: %s\n",s);
    assert(result); if(!strstr(out[used-1],message)) fprintf(stderr,"Expected %s, got %s\n",message,out[used-1]);
    assert(strstr(out[used-1],message));
}
static char cached[MAX_LINES][LINE_LEN];
static BasicSource reader(char *path, int *count) {
    if (!strstr(path,"CACHE.BAS")) return 0;
    *count = 3; return cached;
}
int main(int argc, char **argv) {
    assert(argc==2);strcpy(base,argv[1]);
    ok("let a=6\nB=7\nprint A*B\nPRINT (100+20)/3\nEND\nPRINT 9", "42");
    assert(!strcmp(out[1],"40") && used==3);
    ok("PRINT 2+3*4\nPRINT -(2+3)*4\nPRINT 7/-2", "14");
    assert(!strcmp(out[1],"-20") && !strcmp(out[2],"-3"));
    ok("REM test\nPRINT \"hello\"; 2; \"!\"\n' comment", "hello2!");
    bad("PRINT 1/0","Division by zero");
    bad("PRINT (2+3","Missing closing");bad("PRINT \"bad","Missing closing quote");
    bad("PRINT 1000000*1000000","numeric range");bad("PRINT 1000000+1","numeric range");
    bad("PRINT 9999999999999999999999999","exceeds 1000000");
    bad("PRINT (((((((((((((((((1)))))))))))))))))","nested too deeply");
    ok("Dim Total As Integer = 42\nPrint Total", "42");
    ok("Dim x As String = \"hello\"\nPrint x", "hello");
    bad("Dim value As Integer\nDim VALUE As Integer", "Duplicate variable");
    file("COUNTER.BAS", "Class Counter\nPrivate value As Integer = 1\nPublic Sub New(start As Integer)\nMe.value=start\nEnd Sub\nPublic Sub Add(amount As Integer)\nMe.value=Me.value+amount\nEnd Sub\nPublic Function GetValue() As Integer\nReturn Me.value\nEnd Function\nEnd Class\n");
    ok("Imports \"COUNTER.BAS\"\nDim first As New Counter(10)\nDim second As New Counter(2)\nfirst.Add(5)\nPrint first.GetValue()\nPrint second.GetValue()", "15");
    assert(!strcmp(out[1],"2") && basic_file_count()==2);
    ok("Imports \"COUNTER.BAS\"\nDim first As New Counter(10)\nDim alias As Counter\nalias=first\nalias.Add(32)\nPrint first.GetValue()", "42");
    ok("Imports \"COUNTER.BAS\"\nDim a As Counter = New Counter(40)\na.Add(2)\nPrint a.GetValue()", "42");
    bad("Imports \"COUNTER.BAS\"\nDim c As New Counter(0)\nPrint c.value", "Private member");
    bad("Imports \"COUNTER.BAS\"\nDim c As Counter\nPrint c.GetValue()", "Nothing");
    bad("Imports \"COUNTER.BAS\"\nDim c As New Counter(0)\nc=4", "requires object");
    bad("Imports \"COUNTER.BAS\"\nDim c As New Counter(0)\nc.Add()", "Wrong number");
    bad("Imports \"COUNTER.BAS\"\nDim c As New Counter(0)\nPRINT c.Add(2)", "Sub cannot");
    bad("Imports \"COUNTER.BAS\"\nDim c As New Counter(0)\nc.Missing()", "Unknown object method");
    bad("Imports \"COUNTER.BAS\"\nDim c As New Counter(0)\nc.NoField=1", "Unknown object field");
    file("PLAIN.BAS","Class Plain\nPublic Value As Integer = 4\nPublic Function Twice(x As Integer) As Integer\nReturn x*2\nEnd Function\nEnd Class\n");
    ok("Imports \"PLAIN.BAS\"\nDim p As New Plain\nDim q As New Plain\np.Value=21\nPrint p.Twice(p.Value)\nPrint q.Value", "42");
    assert(!strcmp(out[1],"4"));
    bad("Imports \"PLAIN.BAS\"\nDim p As New Plain(1)", "no constructor");
    bad("Imports \"PLAIN.BAS\"\nImports \"COUNTER.BAS\"\nDim p As New Plain\nDim q As Counter\nq=p", "Class does not match");
    ok("Imports \"PLAIN.BAS\"\nDim p As Plain\np=New Plain()\np=Nothing\nPRINT 42", "42");
    file("MATH.BAS","Function Double(x As Integer) As Integer\nDim Local As Integer = x*2\nReturn Local\nEnd Function\nSub Say(n As Integer)\nPRINT n\nEnd Sub\n");
    ok("Imports \"MATH.BAS\"\nCall Say(Double(21))", "42");
    bad("Imports \"MATH.BAS\"\nPrint Double(1,2)", "Wrong number");
    bad("PRINT Me.Value", "Me is only valid");bad("RETURN 2", "Return is only valid");
    file("LOOP.BAS","Function Recurse() As Integer\nReturn Recurse()\nEnd Function\n");
    bad("Imports \"LOOP.BAS\"\nPrint Recurse()", "call depth");
    file("NORETURN.BAS","Function Test() As Integer\nPRINT 1\nEnd Function\n");
    bad("Imports \"NORETURN.BAS\"\nPRINT Test()", "without Return");
    file("ERROR.BAS","Class Broken\nPublic Sub Crash()\nPrint 1/0\nEnd Sub\nEnd Class\n");
    assert(run("Imports \"ERROR.BAS\"\nDim b As New Broken\nb.Crash()") == 3);
    assert(strstr(out[used-1],"ERROR.BAS:3:") && basic_error_main_line()==1);
    bad("Imports \"NOFILE.BAS\"", "Cannot open imported file");
    assert(strstr(out[used-1],"NOFILE.BAS:1:"));
    file("CYCLEA.BAS","Imports \"CYCLEB.BAS\"\n");file("CYCLEB.BAS","Imports \"./CYCLEA.BAS\"\n");
    bad("Imports \"CYCLEA.BAS\"", "Circular Imports");
    ok("Imports \"PLAIN.BAS\"\nImports \"./PLAIN.BAS\"\nDim p As New Plain\nPRINT p.Value", "4");
    assert(basic_file_count()==2);
    file("NEST.BAS","Imports \"PLAIN.BAS\"\n");
    ok("Imports \"NEST.BAS\"\nImports \"PLAIN.BAS\"\nDim p As New Plain\nPRINT p.Value", "4");
    assert(basic_file_count()==3);
    bad("Class Foo\nClass Bar\nEnd Class\nEnd Class", "Nested Class");
    bad("Class Foo\nPublic X As Integer\nPublic x As Integer\nEnd Class", "Duplicate member");
    bad("Class Foo\nPublic X As Integer", "Missing End Class");
    bad("Sub Hello()\nPRINT 1", "Missing End Sub");
    bad("Class Foo\nEnd Class\nClass foo\nEnd Class", "Duplicate Class");
    ok("PRINT 4>=3\nPRINT 4<>4\nPRINT 4<3", "-1");
    assert(!strcmp(out[1],"0") && !strcmp(out[2],"0"));
    file("CACHE.BAS", "Class Cached\nPublic Value As Integer = 1\nEnd Class\n");
    strcpy(cached[0],"Class Cached");strcpy(cached[1],"Public Value As Integer = 42");strcpy(cached[2],"End Class");
    basic_set_reader(reader);
    ok("Imports \"CACHE.BAS\"\nDim c As New Cached\nPRINT c.Value", "42");
    basic_set_reader(0);
    ok("Imports \"CACHE.BAS\"\nDim c As New Cached\nPRINT c.Value", "1");
    printf("%d interpreter/import/object checks passed.\n",checks);return 0;
}

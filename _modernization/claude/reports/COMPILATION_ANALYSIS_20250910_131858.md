# Comprehensive Compilation Analysis Report

**Generated**: 2025-09-10 13:18:58
**Total Files**: 67
**Success Rate**: 26.9%

## Detailed Error Analysis

### Auxil/onavy.c
**Status**: ❌ Compilation Error
**Error Output**:
```
Auxil/onavy.c: In function ‘get_cargo’:
Auxil/onavy.c:29:1: warning: old-style function definition [-Wold-style-definition]
   29 | get_cargo(str)
      | ^~~~~~~~~
Auxil/onavy.c: In function ‘loadstat’:
Auxil/onavy.c:81:1: warning: old-style function definition [-Wold-style-definition]
   81 | loadstat(status)
      | ^~~~~~~~
Auxil/onavy.c:85:14: error: ‘TRADED’ undeclared (first use in this function)
   85 |         case TRADED:
      |              ^~~~~~
Auxil/onavy.c:85:14: note: each undeclared identifier is reported only once for each function it appears in
Auxil/onavy.c:86:14: error: ‘GENERAL’ undeclared (first use in this function)
   86 |         case GENERAL:
      |              ^~~~~~~
Auxil/onavy.c:87:14: error: ‘MILITIA’ undeclared (first use in this function)
   87 |         case MILITIA:
      |              ^~~~~~~
Auxil/onavy.c:88:14: error: ‘GARRISON’ undeclared (first use in this function)
   88 |         case GARRISON:
      |              ^~~~~~~~
Auxil/onavy.c:
```

### Auxil/psmap.c
**Status**: ❌ Compilation Error
**Error Output**:
```
Auxil/psmap.c: In function ‘parsepagesize’:
Auxil/psmap.c:47:1: warning: old-style function definition [-Wold-style-definition]
   47 | parsepagesize(buf)
      | ^~~~~~~~~~~~~
Auxil/psmap.c: In function ‘setpagesize’:
Auxil/psmap.c:88:1: warning: old-style function definition [-Wold-style-definition]
   88 | setpagesize(defpag)
      | ^~~~~~~~~~~
Auxil/psmap.c: In function ‘get_pagesize’:
Auxil/psmap.c:140:16: error: ‘DEFAULTPAGE’ undeclared (first use in this function)
  140 |   int defpag = DEFAULTPAGE;
      |                ^~~~~~~~~~~
Auxil/psmap.c:140:16: note: each undeclared identifier is reported only once for each function it appears in
Auxil/psmap.c: In function ‘psstring’:
Auxil/psmap.c:181:1: warning: old-style function definition [-Wold-style-definition]
  181 | psstring(fh, str)
      | ^~~~~~~~
Auxil/psmap.c:186:15: warning: comparison between pointer and integer
  186 |   while (*str != NULL) {
      |               ^~
Auxil/psmap.c: In function ‘isinstr’:
Auxil/psma
```

### Auxil/sort.c
**Status**: ❌ Compilation Error
**Error Output**:
```
Auxil/sort.c: In function ‘main’:
Auxil/sort.c:146:1: warning: old-style function definition [-Wold-style-definition]
  146 | main(argc, argv)
      | ^~~~
Auxil/sort.c: In function ‘get_line’:
Auxil/sort.c:152:7: warning: old-style function definition [-Wold-style-definition]
  152 |   int get_line(char data[]);
      |       ^~~~~~~~
Auxil/sort.c: In function ‘place’:
Auxil/sort.c:153:8: warning: old-style function definition [-Wold-style-definition]
  153 |   void place(char data[]), send_out(void);
      |        ^~~~~
Auxil/sort.c:441:12: error: too many arguments to function ‘build_node’; expected 0, have 2
  441 |     head = build_node(data,(L_PTR)NULL);
      |            ^~~~~~~~~~ ~~~~
Auxil/sort.c:435:15: note: declared here
  435 |   L_PTR temp, build_node();
      |               ^~~~~~~~~~
Auxil/sort.c:442:14: error: too many arguments to function ‘comp_line’; expected 0, have 2
  442 |   } else if (comp_line(head->line, data) == 1) {
      |              ^~~~~~~~~ ~~~~~~
```

### Src/adduserA.c
**Status**: ❌ Compilation Error
**Error Output**:
```
Src/adduserA.c: In function ‘place’:
Src/adduserA.c:1059:26: warning: comparison is always false due to limited range of data type [-Wtype-limits]
 1059 |   if ((ntn_ptr->location < 0) ||
      |                          ^
Src/adduserA.c: In function ‘newlogin’:
Src/adduserA.c:1475:12: error: type defaults to ‘int’ in declaration of ‘i’ [-Wimplicit-int]
 1475 |   register i;
      |            ^
Src/adduserA.c:1588:14: warning: too many arguments for format [-Wformat-extra-args]
 1588 |              "< Building Country %s >", AU_name, country);
      |              ^~~~~~~~~~~~~~~~~~~~~~~~~

```

### Src/armyG.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/armyG.c:1528:42: warning: comparison of integer expressions of different signedness: ‘size_t’ {aka ‘long unsigned int’} and ‘int’ [-Wsign-compare]
- Src/armyG.c:2066:63: warning: format ‘%d’ expects argument of type ‘int’, but argument 3 has type ‘long int’ [-Wformat=]
- Src/armyG.c:2097:65: warning: format ‘%d’ expects argument of type ‘int’, but argument 3 has type ‘long int’ [-Wformat=]
- Src/armyG.c:2129:65: warning: format ‘%d’ expects argument of type ‘int’, but argument 3 has type ‘long int’ [-Wformat=]

### Src/caravanG.c
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/caravanG.c:64:
./Include/caravanX.h:402:53: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘/’ token
  402 |  * CARAVAN STATE CHANGE LOGGING MACROS (VADJ*/VMTRLS/VCREATE/VDESTROY)
      |                                                     ^
Src/caravanG.c: In function ‘disb_cvn’:
Src/caravanG.c:102:23: error: ‘cvn_ptr’ undeclared (first use in this function); did you mean ‘v1_ptr’?
  102 |   CVN_PTR chold_ptr = cvn_ptr;
      |                       ^~~~~~~
      |                       v1_ptr
Src/caravanG.c:102:23: note: each undeclared identifier is reported only once for each function it appears in
Src/caravanG.c: In function ‘change_vspeed’:
Src/caravanG.c:232:3: error: ‘cvn_ptr’ undeclared (first use in this function); did you mean ‘v1_ptr’?
  232 |   cvn_ptr = v1_ptr;
      |   ^~~~~~~
      |   v1_ptr
Src/caravanG.c: In function ‘comb_cvns’:
Src/caravanG.c:303:3: error: ‘cvn_ptr’ undeclared (first use in this function); did you mean ‘v2
```

### Src/checkX.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/checkX.c:792:41: warning: ‘%s’ directive writing up to 499 bytes into a region of size 468 [-Wformat-overflow=]

### Src/combatA.c
**Status**: ✅ Success

### Src/computeX.c
**Status**: ✅ Success

### Src/configA.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/configA.c:1758:10: warning: this statement may fall through [-Wimplicit-fallthrough=]
- Src/configA.c:1513:5: warning: ‘sprintf’ argument 3 overlaps destination object ‘string’ [-Wrestrict]

### Src/convertX.c
**Status**: ✅ Success

### Src/createA.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/createA.c:650:8: warning: suggest explicit braces to avoid ambiguous ‘else’ [-Wdangling-else]
- Src/createA.c:1019:10: warning: suggest explicit braces to avoid ambiguous ‘else’ [-Wdangling-else]

### Src/customG.c
**Status**: ❌ Compilation Error
**Error Output**:
```
Src/customG.c: In function ‘write_custom’:
Src/customG.c:560:5: error: implicit declaration of function ‘setreuid’; did you mean ‘seteuid’? [-Wimplicit-function-declaration]
  560 |     setreuid(euid, uid);
      |     ^~~~~~~~
      |     seteuid

```

### Src/customX.c
**Status**: ❌ Compilation Error
**Error Output**:
```
Src/customX.c: In function ‘read_environ’:
Src/customX.c:140:24: error: ‘DEFAULTDIR’ undeclared (first use in this function); did you mean ‘DEFAULT_EDITOR’?
  140 |     strcpy(defaultdir, DEFAULTDIR);
      |                        ^~~~~~~~~~
      |                        DEFAULT_EDITOR
Src/customX.c:140:24: note: each undeclared identifier is reported only once for each function it appears in
Src/customX.c:150:21: error: ‘EXEDIR’ undeclared (first use in this function)
  150 |     strcpy(progdir, EXEDIR);
      |                     ^~~~~~
Src/customX.c: In function ‘read_custom’:
Src/customX.c:452:5: error: implicit declaration of function ‘setreuid’; did you mean ‘seteuid’? [-Wimplicit-function-declaration]
  452 |     setreuid(euid, uid);
      |     ^~~~~~~~
      |     seteuid

```

### Src/dataA.c
**Status**: ✅ Success

### Src/dataG.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/dataG.c:133:31: warning: missing braces around initializer [-Wmissing-braces]
- Src/dataG.c:175:43: warning: missing braces around initializer [-Wmissing-braces]
- Src/dataG.c:175:43: warning: missing braces around initializer [-Wmissing-braces]
- Src/dataG.c:175:43: warning: missing braces around initializer [-Wmissing-braces]
- Src/dataG.c:175:43: warning: missing braces around initializer [-Wmissing-braces]

### Src/dataX.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/dataX.c:171:54: warning: missing braces around initializer [-Wmissing-braces]
- Src/dataX.c:211:54: warning: missing braces around initializer [-Wmissing-braces]
- Src/dataX.c:249:38: warning: missing braces around initializer [-Wmissing-braces]
- Src/dataX.c:249:38: warning: missing braces around initializer [-Wmissing-braces]
- Src/dataX.c:249:38: warning: missing braces around initializer [-Wmissing-braces]

### Src/datamagX.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/datamagX.c:83:30: warning: missing braces around initializer [-Wmissing-braces]
- Src/datamagX.c:83:30: warning: missing braces around initializer [-Wmissing-braces]
- Src/datamagX.c:83:30: warning: missing braces around initializer [-Wmissing-braces]
- Src/datamagX.c:83:30: warning: missing braces around initializer [-Wmissing-braces]
- Src/datamagX.c:83:30: warning: missing braces around initializer [-Wmissing-braces]

### Src/datamilX.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/datamilX.c:85:29: warning: missing braces around initializer [-Wmissing-braces]
- Src/datamilX.c:85:29: warning: missing braces around initializer [-Wmissing-braces]
- Src/datamilX.c:85:29: warning: missing braces around initializer [-Wmissing-braces]
- Src/datamilX.c:85:29: warning: missing braces around initializer [-Wmissing-braces]
- Src/datamilX.c:85:29: warning: missing braces around initializer [-Wmissing-braces]

### Src/displayG.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/displayG.c:1040:40: warning: comparison of integer expressions of different signedness: ‘size_t’ {aka ‘long unsigned int’} and ‘int’ [-Wsign-compare]
- Src/displayG.c:1631:38: warning: format ‘%d’ expects argument of type ‘int’, but argument 4 has type ‘long int’ [-Wformat=]
- Src/displayG.c:1635:57: warning: format ‘%d’ expects argument of type ‘int’, but argument 4 has type ‘long int’ [-Wformat=]
- Src/displayG.c:1639:56: warning: format ‘%d’ expects argument of type ‘int’, but argument 4 has type ‘long int’ [-Wformat=]
- Src/displayG.c:1643:45: warning: format ‘%d’ expects argument of type ‘int’, but argument 4 has type ‘long int’ [-Wformat=]

### Src/economyA.c
**Status**: ✅ Success

### Src/emailG.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/emailG.c:599:15: warning: comparison of integer expressions of different signedness: ‘int’ and ‘size_t’ {aka ‘long unsigned int’} [-Wsign-compare]
- Src/emailG.c:617:16: warning: comparison of integer expressions of different signedness: ‘int’ and ‘size_t’ {aka ‘long unsigned int’} [-Wsign-compare]
- Src/emailG.c:708:25: warning: comparison of integer expressions of different signedness: ‘int’ and ‘size_t’ {aka ‘long unsigned int’} [-Wsign-compare]
- Src/emailG.c:712:30: warning: comparison of integer expressions of different signedness: ‘int’ and ‘size_t’ {aka ‘long unsigned int’} [-Wsign-compare]
- Src/emailG.c:1034:23: warning: comparison of integer expressions of different signedness: ‘int’ and ‘size_t’ {aka ‘long unsigned int’} [-Wsign-compare]

### Src/enlistG.c
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/enlistG.c:75:
./Include/caravanX.h:402:53: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘/’ token
  402 |  * CARAVAN STATE CHANGE LOGGING MACROS (VADJ*/VMTRLS/VCREATE/VDESTROY)
      |                                                     ^
Src/enlistG.c: In function ‘remote_enlist’:
Src/enlistG.c:288:32: warning: format ‘%ld’ expects argument of type ‘long int’, but argument 3 has type ‘int’ [-Wformat=]
  288 |       sprintf(bufstr, "Cost (%ld %s):", 1, ainfo_list[choice].name);
      |                              ~~^        ~
      |                                |        |
      |                                long int int
      |                              %d
Src/enlistG.c: In function ‘enlist_army’:
Src/enlistG.c:453:24: warning: format ‘%d’ expects argument of type ‘int’, but argument 2 has type ‘long int’ [-Wformat=]
  453 |         printw("[10 - %d] ", max_numunit(ntn_ptr, city_ptr, choice, 0));
      |                       ~^    
```

### Src/executeX.c
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/executeX.c:56:
./Include/caravanX.h:402:53: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘/’ token
  402 |  * CARAVAN STATE CHANGE LOGGING MACROS (VADJ*/VMTRLS/VCREATE/VDESTROY)
      |                                                     ^
Src/executeX.c: In function ‘exec_cvn’:
Src/executeX.c:612:9: error: ‘cvn_ptr’ undeclared (first use in this function); did you mean ‘ntn_ptr’?
  612 |     if (cvn_ptr == NULL || CVN_ID != idnum) {
      |         ^~~~~~~
      |         ntn_ptr
Src/executeX.c:612:9: note: each undeclared identifier is reported only once for each function it appears in
Src/executeX.c: In function ‘exec_item’:
Src/executeX.c:1088:62: warning: format ‘%d’ expects argument of type ‘int’, but argument 5 has type ‘long int’ [-Wformat=]
 1088 |       sprintf(string, "Error(%s): I_MEN(%d), Negative value %d",
      |                                                             ~^
      |                                             
```

### Src/getopt.c
**Status**: ❌ Compilation Error
**Error Output**:
```
Src/getopt.c:100:1: error: return type defaults to ‘int’ [-Wimplicit-int]
  100 | getopt(nargc, nargv, ostr)
      | ^~~~~~
Src/getopt.c: In function ‘getopt’:
Src/getopt.c:100:1: warning: old-style function definition [-Wold-style-definition]
Src/getopt.c:120:19: error: too many arguments to function ‘index’; expected 0, have 2
  120 |         || (oli = index(ostr, optopt)) == NULL)
      |                   ^~~~~ ~~~~
Src/getopt.c:21:18: note: declared here
   21 | extern char     *index();       /* This may be strchr   */
      |                  ^~~~~

```

### Src/hexmapG.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/hexmapG.c:1015:18: warning: comparison of integer expressions of different signedness: ‘int’ and ‘Tgctype’ {aka ‘enum tgctype’} [-Wsign-compare]
- Src/hexmapG.c:2226:23: warning: unused parameter ‘widemap’ [-Wunused-parameter]
- Src/hexmapG.c:1139:8: warning: this statement may fall through [-Wimplicit-fallthrough=]

### Src/hexmapX.c
**Status**: ✅ Success

### Src/ieditG.c
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/ieditG.c:29:
./Include/caravanX.h:402:53: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘/’ token
  402 |  * CARAVAN STATE CHANGE LOGGING MACROS (VADJ*/VMTRLS/VCREATE/VDESTROY)
      |                                                     ^
Src/ieditG.c: In function ‘god_cvnedit’:
Src/ieditG.c:76:8: error: ‘cvn_ptr’ undeclared (first use in this function); did you mean ‘cvn_Iptr’?
   76 |   if ((cvn_ptr = cvnbynum(cvnnum)) == NULL) {
      |        ^~~~~~~
      |        cvn_Iptr
Src/ieditG.c:76:8: note: each undeclared identifier is reported only once for each function it appears in
Src/ieditG.c: In function ‘god_navyedit’:
Src/ieditG.c:622:58: warning: format ‘%d’ expects argument of type ‘int’, but argument 4 has type ‘uns_char *’ {aka ‘unsigned char *’} [-Wformat=]
  622 |     mvprintw(line, 0, "Set what efficiency percentage? [%d%%] ",
      |                                                         ~^
      |                                
```

### Src/infoG.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/infoG.c:171:52: warning: field width specifier ‘*’ expects argument of type ‘int’, but argument 5 has type ‘size_t’ {aka ‘long unsigned int’} [-Wformat=]
- Src/infoG.c:212:52: warning: field width specifier ‘*’ expects argument of type ‘int’, but argument 5 has type ‘size_t’ {aka ‘long unsigned int’} [-Wformat=]
- Src/infoG.c:973:27: warning: format ‘%d’ expects argument of type ‘int’, but argument 4 has type ‘long int’ [-Wformat=]

### Src/ioG.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/ioG.c:439:35: warning: comparison of integer expressions of different signedness: ‘Aclasstype’ {aka ‘enum aclasstype’} and ‘int’ [-Wsign-compare]
- Src/ioG.c:467:51: warning: comparison of integer expressions of different signedness: ‘size_t’ {aka ‘long unsigned int’} and ‘int’ [-Wsign-compare]
- Src/ioG.c:524:35: warning: comparison of integer expressions of different signedness: ‘Aclasstype’ {aka ‘enum aclasstype’} and ‘int’ [-Wsign-compare]
- Src/ioG.c:601:43: warning: comparison of integer expressions of different signedness: ‘size_t’ {aka ‘long unsigned int’} and ‘int’ [-Wsign-compare]
- Src/ioG.c:895:31: warning: comparison of integer expressions of different signedness: ‘size_t’ {aka ‘long unsigned int’} and ‘int’ [-Wsign-compare]

### Src/ioX.c
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from ./Include/dataX.h:69,
                 from Src/ioX.c:17:
Src/ioX.c: In function ‘win_size_change’:
Src/ioX.c:116:29: warning: unused parameter ‘sig’ [-Wunused-parameter]
  116 | win_size_change PARM_1(int, sig)
      |                        ~~~~~^~~
./Include/paramX.h:122:24: note: in definition of macro ‘PARM_1’
  122 | #define PARM_1(a,b) (a b)
      |                        ^
Src/ioX.c: In function ‘fork_edit_on_file’:
Src/ioX.c:293:7: error: implicit declaration of function ‘setreuid’; did you mean ‘seteuid’? [-Wimplicit-function-declaration]
  293 |       setreuid(euid, uid);
      |       ^~~~~~~~
      |       seteuid
Src/ioX.c:314:8: warning: extra tokens at end of ‘#endif’ directive [-Wendif-labels]
  314 | #endif SWITCHID
      |        ^~~~~~~~
Src/ioX.c: In function ‘extend_str’:
Src/ioX.c:1485:24: warning: comparison of integer expressions of different signedness: ‘size_t’ {aka ‘long unsigned int’} and ‘int’ [-Wsign-compare]
 1485 |     if (strlen(b
```

### Src/iodataG.c
**Status**: ❌ Compilation Error
**Error Output**:
```
Src/iodataG.c: In function ‘files_check’:
Src/iodataG.c:218:18: error: implicit declaration of function ‘time’ [-Wimplicit-function-declaration]
  218 |   long curtime = time((long *)0);
      |                  ^~~~
Src/iodataG.c:24:1: note: ‘time’ is defined in header ‘<time.h>’; this is probably fixable by adding ‘#include <time.h>’
   23 | #include "worldX.h"
  +++ |+#include <time.h>
   24 | 
Src/iodataG.c: In function ‘max_descript’:
Src/iodataG.c:894:10: error: implicit declaration of function ‘getdtablesize’; did you mean ‘set_tabsize’? [-Wimplicit-function-declaration]
  894 |   return(getdtablesize());
      |          ^~~~~~~~~~~~~
      |          set_tabsize

```

### Src/iodataX.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/iodataX.c:225:44: warning: format ‘%ld’ expects argument of type ‘long int’, but argument 3 has type ‘int’ [-Wformat=]
- Src/iodataX.c:273:44: warning: format ‘%ld’ expects argument of type ‘long int’, but argument 3 has type ‘int’ [-Wformat=]

### Src/jointA.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/jointA.c:74:32: warning: unused parameter ‘str’ [-Wunused-parameter]
- Src/jointA.c:74:45: warning: unused parameter ‘fstr’ [-Wunused-parameter]
- Src/jointA.c:74:56: warning: unused parameter ‘lnum’ [-Wunused-parameter]
- Src/jointA.c:111:30: warning: unused parameter ‘str’ [-Wunused-parameter]
- Src/jointA.c:111:43: warning: unused parameter ‘fstr’ [-Wunused-parameter]

### Src/jointG.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/jointG.c:763:20: warning: unused parameter ‘sig’ [-Wunused-parameter]

### Src/keybindG.c
**Status**: ✅ Success

### Src/magicA.c
**Status**: ✅ Success

### Src/magicG.c
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/magicG.c:33:
./Include/caravanX.h:402:53: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘/’ token
  402 |  * CARAVAN STATE CHANGE LOGGING MACROS (VADJ*/VMTRLS/VCREATE/VDESTROY)
      |                                                     ^
Src/magicG.c: In function ‘work_spell’:
Src/magicG.c:240:16: error: ‘cvn_tptr’ undeclared (first use in this function); did you mean ‘ntn_tptr’?
  240 |     } else if (cvn_tptr != NULL) {
      |                ^~~~~~~~
      |                ntn_tptr
Src/magicG.c:240:16: note: each undeclared identifier is reported only once for each function it appears in
Src/magicG.c:241:7: error: ‘cvn_ptr’ undeclared (first use in this function); did you mean ‘ntn_ptr’?
  241 |       cvn_ptr = cvn_tptr;
      |       ^~~~~~~
      |       ntn_ptr
Src/magicG.c: In function ‘go_spell’:
Src/magicG.c:658:19: warning: comparison of integer expressions of different signedness: ‘Spelltype’ {aka ‘enum spelltype’} and ‘int’ [-Wsig
```

### Src/magicX.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/magicX.c:792:8: warning: this statement may fall through [-Wimplicit-fallthrough=]

### Src/mailA.c
**Status**: ✅ Success

### Src/mailG.c
**Status**: ✅ Success

### Src/mailX.c
**Status**: ✅ Success

### Src/mainA.c
**Status**: ✅ Success

### Src/mainG.c
**Status**: ❌ Compilation Error
**Error Output**:
```
Src/mainG.c: In function ‘main’:
Src/mainG.c:589:5: error: implicit declaration of function ‘gets’; did you mean ‘fgets’? [-Wimplicit-function-declaration]
  589 |     gets(nationname);
      |     ^~~~
      |     fgets
Src/mainG.c:641:20: error: implicit declaration of function ‘getpass’; did you mean ‘get_pass’? [-Wimplicit-function-declaration]
  641 |   strncpy(tmppass, getpass(""), PASSLTH + 1);
      |                    ^~~~~~~
      |                    get_pass
Src/mainG.c:641:20: error: passing argument 2 of ‘strncpy’ makes pointer from integer without a cast [-Wint-conversion]
  641 |   strncpy(tmppass, getpass(""), PASSLTH + 1);
      |                    ^~~~~~~~~~~
      |                    |
      |                    int
In file included from ./Include/sysconf.h:484,
                 from ./Include/dataX.h:68,
                 from ./Include/dataG.h:56,
                 from Src/mainG.c:39:
/usr/include/string.h:145:46: note: expected ‘const char * restrict’ but argum
```

### Src/memoryX.c
**Status**: ❌ Compilation Error
**Error Output**:
```
Src/memoryX.c: In function ‘clr_memory’:
Src/memoryX.c:78:3: error: implicit declaration of function ‘bzero’ [-Wimplicit-function-declaration]
   78 |   bzero( mem_ptr, len);
      |   ^~~~~

```

### Src/miscA.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/miscA.c:94:39: warning: unused parameter ‘race’ [-Wunused-parameter]

### Src/miscG.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/miscG.c:536:28: warning: comparison of integer expressions of different signedness: ‘size_t’ {aka ‘long unsigned int’} and ‘int’ [-Wsign-compare]
- Src/miscG.c:2153:34: warning: comparison of integer expressions of different signedness: ‘size_t’ {aka ‘long unsigned int’} and ‘int’ [-Wsign-compare]
- Src/miscG.c:2197:28: warning: comparison of integer expressions of different signedness: ‘Tgctype’ {aka ‘enum tgctype’} and ‘int’ [-Wsign-compare]
- Src/miscG.c:2208:28: warning: comparison of integer expressions of different signedness: ‘Tgctype’ {aka ‘enum tgctype’} and ‘int’ [-Wsign-compare]

### Src/miscX.c
**Status**: ❌ Compilation Error
**Error Output**:
```
Src/miscX.c: In function ‘rand_seed’:
Src/miscX.c:218:3: error: implicit declaration of function ‘srand48’; did you mean ‘srand’? [-Wimplicit-function-declaration]
  218 |   srand48((long) time ((long *) 0));
      |   ^~~~~~~
      |   srand
Src/miscX.c:218:18: error: implicit declaration of function ‘time’ [-Wimplicit-function-declaration]
  218 |   srand48((long) time ((long *) 0));
      |                  ^~~~
Src/miscX.c:48:1: note: ‘time’ is defined in header ‘<time.h>’; this is probably fixable by adding ‘#include <time.h>’
   47 | #include "displayX.h"
  +++ |+#include <time.h>
   48 | 
Src/miscX.c: In function ‘rand_val’:
Src/miscX.c:253:10: error: implicit declaration of function ‘lrand48’ [-Wimplicit-function-declaration]
  253 |   return(lrand48() % limit);
      |          ^~~~~~~
Src/miscX.c: In function ‘mach_time’:
Src/miscX.c:293:10: error: implicit declaration of function ‘ctime’ [-Wimplicit-function-declaration]
  293 |   return(ctime(&timeval));
      |          ^~
```

### Src/monsterA.c
**Status**: ✅ Success

### Src/moveA.c
**Status**: ✅ Success

### Src/moveG.c
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/moveG.c:29:
./Include/caravanX.h:402:53: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘/’ token
  402 |  * CARAVAN STATE CHANGE LOGGING MACROS (VADJ*/VMTRLS/VCREATE/VDESTROY)
      |                                                     ^
Src/moveG.c: In function ‘move_select’:
Src/moveG.c:97:15: error: ‘cvn_ptr’ undeclared (first use in this function); did you mean ‘ntn_ptr’?
   97 |   } else if ((cvn_ptr = get_cvnselect()) != NULL) {
      |               ^~~~~~~
      |               ntn_ptr
Src/moveG.c:97:15: note: each undeclared identifier is reported only once for each function it appears in
Src/moveG.c: In function ‘move_init’:
./Include/caravanX.h:171:25: error: ‘cvn_ptr’ undeclared (first use in this function); did you mean ‘ntn_ptr’?
  171 | #define CVN_MOVE        cvn_ptr->umove
      |                         ^~~~~~~
Src/moveG.c:484:31: note: in expansion of macro ‘CVN_MOVE’
  484 |     mp_remaining = 10 * (int) CVN_MOVE;
      |  
```

### Src/moveX.c
**Status**: ✅ Success

### Src/navyG.c
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/navyG.c:28:
./Include/caravanX.h:402:53: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘/’ token
  402 |  * CARAVAN STATE CHANGE LOGGING MACROS (VADJ*/VMTRLS/VCREATE/VDESTROY)
      |                                                     ^
Src/navyG.c: In function ‘ext_navyinfo’:
Src/navyG.c:1355:8: error: ‘cvn_tptr’ undeclared (first use in this function); did you mean ‘ntn_tptr’?
 1355 |   if ((cvn_tptr = cvnbynum(NAVY_CVN)) != NULL) {
      |        ^~~~~~~~
      |        ntn_tptr
Src/navyG.c:1355:8: note: each undeclared identifier is reported only once for each function it appears in
Src/navyG.c: In function ‘ext_navycmd’:
Src/navyG.c:1490:42: warning: comparison of integer expressions of different signedness: ‘size_t’ {aka ‘long unsigned int’} and ‘int’ [-Wsign-compare]
 1490 |       if (x + strlen(stat_info[i].input) > COLS - 8) {
      |                                          ^
Src/navyG.c: In function ‘navy_transport’:
Src/navyG.c:19
```

### Src/npcA.c
**Status**: ✅ Success

### Src/ntninfoG.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/ntninfoG.c:455:22: warning: comparison of integer expressions of different signedness: ‘size_t’ {aka ‘long unsigned int’} and ‘int’ [-Wsign-compare]
- Src/ntninfoG.c:1020:7: warning: missing initializer for field ‘it’ of ‘NI_ITEM’ {aka ‘struct s_nitem’} [-Wmissing-field-initializers]
- Src/ntninfoG.c:1028:7: warning: missing initializer for field ‘it’ of ‘NI_ITEM’ {aka ‘struct s_nitem’} [-Wmissing-field-initializers]
- Src/ntninfoG.c:1033:7: warning: missing initializer for field ‘it’ of ‘NI_ITEM’ {aka ‘struct s_nitem’} [-Wmissing-field-initializers]
- Src/ntninfoG.c:1039:7: warning: missing initializer for field ‘it’ of ‘NI_ITEM’ {aka ‘struct s_nitem’} [-Wmissing-field-initializers]

### Src/pagerG.c
**Status**: ❌ Compilation Error
**Error Output**:
```
Src/pagerG.c: In function ‘line_match’:
Src/pagerG.c:144:12: error: too many arguments to function ‘re_exec’; expected 0, have 1
  144 |     return(re_exec(str));
      |            ^~~~~~~ ~~~
In file included from Src/pagerG.c:17:
./Include/dataG.h:595:12: note: declared here
  595 | extern int re_exec();
      |            ^~~~~~~
Src/pagerG.c: In function ‘page_file’:
Src/pagerG.c:1112:27: error: too many arguments to function ‘re_comp’; expected 0, have 1
 1112 |           if ((temp_str = re_comp(string)) != 0) {
      |                           ^~~~~~~ ~~~~~~
./Include/dataG.h:612:14: note: declared here
  612 | extern char *re_comp();
      |              ^~~~~~~
Src/pagerG.c:1159:27: error: too many arguments to function ‘re_comp’; expected 0, have 1
 1159 |           if ((temp_str = re_comp(string)) != 0) {
      |                           ^~~~~~~ ~~~~~~
./Include/dataG.h:612:14: note: declared here
  612 | extern char *re_comp();
      |              ^~~~~~~
Src/pagerG.c:11
```

### Src/regionG.c
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/regionG.c:56:
./Include/caravanX.h:402:53: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘/’ token
  402 |  * CARAVAN STATE CHANGE LOGGING MACROS (VADJ*/VMTRLS/VCREATE/VDESTROY)
      |                                                     ^
Src/regionG.c: In function ‘adjust_region’:
Src/regionG.c:280:50: warning: format ‘%ld’ expects argument of type ‘long int’, but argument 4 has type ‘int’ [-Wformat=]
  280 |         mvprintw(LINES - 1, 0, "There are only %ld people in the sector",
      |                                                ~~^
      |                                                  |
      |                                                  long int
      |                                                %d
Src/regionG.c:477:10: error: ‘cvn_ptr’ undeclared (first use in this function); did you mean ‘ntn_ptr’?
  477 |     for (cvn_ptr = ntn_ptr->cvn_list;
      |          ^~~~~~~
      |          ntn_ptr
Src/regionG.c:477:10: note
```

### Src/sectorA.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/sectorA.c:420:10: warning: this statement may fall through [-Wimplicit-fallthrough=]
- ./Include/desigX.h:237:35: warning: this statement may fall through [-Wimplicit-fallthrough=]

### Src/sectorG.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/sectorG.c:514:44: warning: comparison of integer expressions of different signedness: ‘size_t’ {aka ‘long unsigned int’} and ‘int’ [-Wsign-compare]
- Src/sectorG.c:888:31: warning: comparison of integer expressions of different signedness: ‘size_t’ {aka ‘long unsigned int’} and ‘int’ [-Wsign-compare]
- Src/sectorG.c:1231:46: warning: format ‘%ld’ expects argument of type ‘long int’, but argument 4 has type ‘int’ [-Wformat=]
- Src/sectorG.c:1234:43: warning: format ‘%ld’ expects argument of type ‘long int’, but argument 4 has type ‘int’ [-Wformat=]
- Src/sectorG.c:207:10: warning: this statement may fall through [-Wimplicit-fallthrough=]

### Src/sectorX.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/sectorX.c:116:55: warning: unused parameter ‘verbal’ [-Wunused-parameter]
- Src/sectorX.c:719:10: warning: variable ‘closed_harbor’ set but not used [-Wunused-but-set-variable]
- Src/sectorX.c:429:10: warning: this statement may fall through [-Wimplicit-fallthrough=]

### Src/selectG.c
**Status**: ✅ Success

### Src/selectX.c
**Status**: ✅ Success

### Src/time_ckG.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/time_ckG.c:98:9: warning: overflow in conversion from ‘enum <anonymous>’ to ‘int’ changes value from ‘4294967295’ to ‘-1’ [-Woverflow]
- Src/time_ckG.c:514:26: warning: unused parameter ‘sig’ [-Wunused-parameter]
- Src/time_ckG.c:563:9: warning: cast from pointer to integer of different size [-Wpointer-to-int-cast]

### Src/unitsX.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/unitsX.c:2873:29: warning: comparison of integer expressions of different signedness: ‘Aclasstype’ {aka ‘enum aclasstype’} and ‘int’ [-Wsign-compare]
- Src/unitsX.c:3613:37: warning: comparison of integer expressions of different signedness: ‘Aclasstype’ {aka ‘enum aclasstype’} and ‘int’ [-Wsign-compare]

### Src/updateA.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/updateA.c:1455:21: warning: this statement may fall through [-Wimplicit-fallthrough=]
- Src/updateA.c:1459:20: warning: this statement may fall through [-Wimplicit-fallthrough=]
- Src/updateA.c:1462:16: warning: this statement may fall through [-Wimplicit-fallthrough=]

### Src/vms.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/vms.c:231: warning: ISO C forbids an empty translation unit [-Wpedantic]

### Src/xferG.c
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/xferG.c:29:
./Include/caravanX.h:402:53: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘/’ token
  402 |  * CARAVAN STATE CHANGE LOGGING MACROS (VADJ*/VMTRLS/VCREATE/VDESTROY)
      |                                                     ^
Src/xferG.c: In function ‘xfer2cvn’:
Src/xferG.c:671:23: error: ‘cvn_ptr’ undeclared (first use in this function); did you mean ‘v1_ptr’?
  671 |   CVN_PTR vhold_ptr = cvn_ptr;
      |                       ^~~~~~~
      |                       v1_ptr
Src/xferG.c:671:23: note: each undeclared identifier is reported only once for each function it appears in
Src/xferG.c: In function ‘xfer_xfer’:
Src/xferG.c:1597:5: error: ‘cvn_ptr’ undeclared (first use in this function); did you mean ‘v2_ptr’?
 1597 |     cvn_ptr = v1_ptr;
      |     ^~~~~~~
      |     v2_ptr

```


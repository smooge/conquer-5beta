# Comprehensive Compilation Analysis Report

**Generated**: 2025-09-10 12:25:51
**Total Files**: 67
**Success Rate**: 25.4%

## Detailed Error Analysis

### Auxil/onavy.c
**Status**: ❌ Compilation Error
**Error Output**:
```
Auxil/onavy.c: In function ‘get_cargo’:
Auxil/onavy.c:27:1: warning: old-style function definition [-Wold-style-definition]
   27 | get_cargo(str)
      | ^~~~~~~~~
Auxil/onavy.c:32:9: error: implicit declaration of function ‘mvprintw’ [-Wimplicit-function-declaration]
   32 |         mvprintw(LINES-3,0,"%s (A)rmy or (P)eople?",str);
      |         ^~~~~~~~
Auxil/onavy.c:32:18: error: ‘LINES’ undeclared (first use in this function)
   32 |         mvprintw(LINES-3,0,"%s (A)rmy or (P)eople?",str);
      |                  ^~~~~
Auxil/onavy.c:32:18: note: each undeclared identifier is reported only once for each function it appears in
Auxil/onavy.c:33:9: error: implicit declaration of function ‘clrtoeol’ [-Wimplicit-function-declaration]
   33 |         clrtoeol();
      |         ^~~~~~~~
Auxil/onavy.c:34:9: error: implicit declaration of function ‘refresh’ [-Wimplicit-function-declaration]
   34 |         refresh();
      |         ^~~~~~~
Auxil/onavy.c:35:16: error: implicit declarat
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
Auxil/sort.c:240:9: error: too many arguments to function ‘get_line’; expected 0, have 1
  240 |     if (get_line(data) >= compnum) {
      |         ^~~~~~~~ ~~~~
Auxil/sort.c:152:7: note: declared here
  152 |   int get_line();
      |       ^~~~~~~~
Auxil/sort.c:241:7: error: too many arguments to function ‘place’; expected 0, have 1
  241 |       place(data);
      |       ^~~~~ ~~~~
Auxil/sort.c:153:8: note: declared here
  153 |   void place(), send_out();
      |        ^~~~~
Auxil/sort.c: In function ‘get_line’:
Auxil/sort.c:152:7: warning: old-style function definition [-Wold-style-definition]
  152 |   int get_line();
      |       ^~~~~~~~
Auxil/sort.c:310:1: error: number of arguments doesn’t match prototype
  310 | {
      | ^
Auxil/sort.c:152:7: error: prototype declaration
  152 |   int get_line();
      |       ^~~~~
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
**Status**: ❌ Compilation Error
**Error Output**:
```
Src/checkX.c: In function ‘check_lock’:
Src/checkX.c:69:23: error: implicit declaration of function ‘lockf’ [-Wimplicit-function-declaration]
   69 | #  define do_lock(fd) lockf(fd,F_TLOCK,0)
      |                       ^~~~~
Src/checkX.c:1022:9: note: in expansion of macro ‘do_lock’
 1022 |     if (do_lock(filed_id) == -1) {
      |         ^~~~~~~
Src/checkX.c: In function ‘get_userid’:
Src/checkX.c:1196:10: error: implicit declaration of function ‘cuserid’ [-Wimplicit-function-declaration]
 1196 |   return(cuserid(outname));
      |          ^~~~~~~
Src/checkX.c:1196:10: error: returning ‘int’ from a function with return type ‘char *’ makes pointer from integer without a cast [-Wint-conversion]
 1196 |   return(cuserid(outname));
      |         ~^~~~~~~~~~~~~~~~~

```

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
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/displayG.c:61:
./Include/hlightX.h:189:44: warning: ‘/*’ within comment [-Wcomment]
  189 |  *   if (HIGH_CHECK(x, y, HI_DIPLOMACY)) { /* sector highlighted */ }
./Include/hlightX.h:189:69: error: expected identifier or ‘(’ before ‘}’ token
  189 |  *   if (HIGH_CHECK(x, y, HI_DIPLOMACY)) { /* sector highlighted */ }
      |                                                                     ^
./Include/hlightX.h:190:3: error: expected identifier or ‘(’ before ‘/’ token
  190 |  */
      |   ^
Src/displayG.c: In function ‘custom_display’:
./Include/hlightX.h:61:25: error: ‘visibility_data’ undeclared (first use in this function)
   61 | #define _VD_(x,y)       visibility_data[(x) + MAPX * (y)]
      |                         ^~~~~~~~~~~~~~~
./Include/hlightX.h:131:26: note: in expansion of macro ‘_VD_’
  131 | #define VIS_CHECK(x,y)  (_VD_(x,y) & SCTR_NMASK)
      |                          ^~~~
Src/displayG.c:663:9: note: in expansion of macro ‘VIS_CHECK’
  6
```

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
In file included from Src/enlistG.c:73:
./Include/hlightX.h:189:44: warning: ‘/*’ within comment [-Wcomment]
  189 |  *   if (HIGH_CHECK(x, y, HI_DIPLOMACY)) { /* sector highlighted */ }
./Include/hlightX.h:189:69: error: expected identifier or ‘(’ before ‘}’ token
  189 |  *   if (HIGH_CHECK(x, y, HI_DIPLOMACY)) { /* sector highlighted */ }
      |                                                                     ^
./Include/hlightX.h:190:3: error: expected identifier or ‘(’ before ‘/’ token
  190 |  */
      |   ^
In file included from Src/enlistG.c:75:
./Include/caravanX.h:402:53: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘/’ token
  402 |  * CARAVAN STATE CHANGE LOGGING MACROS (VADJ*/VMTRLS/VCREATE/VDESTROY)
      |                                                     ^
Src/enlistG.c: In function ‘remote_enlist’:
Src/enlistG.c:288:32: warning: format ‘%ld’ expects argument of type ‘long int’, but argument 3 has type ‘int’ [-Wformat=]
  288 |       sprintf(bufs
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
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/hexmapG.c:73:
./Include/hlightX.h:189:44: warning: ‘/*’ within comment [-Wcomment]
  189 |  *   if (HIGH_CHECK(x, y, HI_DIPLOMACY)) { /* sector highlighted */ }
./Include/hlightX.h:189:69: error: expected identifier or ‘(’ before ‘}’ token
  189 |  *   if (HIGH_CHECK(x, y, HI_DIPLOMACY)) { /* sector highlighted */ }
      |                                                                     ^
./Include/hlightX.h:190:3: error: expected identifier or ‘(’ before ‘/’ token
  190 |  */
      |   ^
Src/hexmapG.c: In function ‘map_highlight’:
Src/hexmapG.c:1015:18: warning: comparison of integer expressions of different signedness: ‘int’ and ‘Tgctype’ {aka ‘enum tgctype’} [-Wsign-compare]
 1015 |           (opval == tg_info[sct_ptr->tradegood].class)) {
      |                  ^~
Src/hexmapG.c: In function ‘show_sect’:
./Include/hlightX.h:61:25: error: ‘visibility_data’ undeclared (first use in this function)
   61 | #define _VD_(x,y)       visibility_data[(x) + MAP
```

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
Src/ioX.c: In function ‘fork_edit_on_file’:
Src/ioX.c:292:7: error: implicit declaration of function ‘setreuid’; did you mean ‘seteuid’? [-Wimplicit-function-declaration]
  292 |       setreuid(euid, uid);
      |       ^~~~~~~~
      |       seteuid
Src/ioX.c:313:8: warning: extra tokens at end of ‘#endif’ directive [-Wendif-labels]
  313 | #endif SWITCHID
      |        ^~~~~~~~
Src/ioX.c: In function ‘cq_init’:
Src/ioX.c:767:20: error: passing argument 2 of ‘signal’ from incompatible pointer type [-Wincompatible-pointer-types]
  767 |   signal(SIGWINCH, win_size_change);
      |                    ^~~~~~~~~~~~~~~
      |                    |
      |                    void (*)(void)
In file included from /usr/include/features.h:524,
                 from /usr/include/bits/libc-header-start.h:33,
                 from /usr/include/stdint.h:26,
                 from /usr/lib/gcc/aarch64-redhat-linux/15/include/stdint.h:11,
                 from /usr/include/curses.h:96,
              
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
In file included from Src/mainG.c:48:
./Include/hlightX.h:189:44: warning: ‘/*’ within comment [-Wcomment]
  189 |  *   if (HIGH_CHECK(x, y, HI_DIPLOMACY)) { /* sector highlighted */ }
./Include/hlightX.h:189:69: error: expected identifier or ‘(’ before ‘}’ token
  189 |  *   if (HIGH_CHECK(x, y, HI_DIPLOMACY)) { /* sector highlighted */ }
      |                                                                     ^
./Include/hlightX.h:190:3: error: expected identifier or ‘(’ before ‘/’ token
  190 |  */
      |   ^
Src/mainG.c: In function ‘main’:
Src/mainG.c:589:5: error: implicit declaration of function ‘gets’; did you mean ‘fgets’? [-Wimplicit-function-declaration]
  589 |     gets(nationname);
      |     ^~~~
      |     fgets
Src/mainG.c:641:20: error: implicit declaration of function ‘getpass’; did you mean ‘get_pass’? [-Wimplicit-function-declaration]
  641 |   strncpy(tmppass, getpass(""), PASSLTH + 1);
      |                    ^~~~~~~
      |                    get_pass
Sr
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
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/miscA.c:24:
./Include/hlightX.h:189:44: warning: ‘/*’ within comment [-Wcomment]
  189 |  *   if (HIGH_CHECK(x, y, HI_DIPLOMACY)) { /* sector highlighted */ }
./Include/hlightX.h:189:69: error: expected identifier or ‘(’ before ‘}’ token
  189 |  *   if (HIGH_CHECK(x, y, HI_DIPLOMACY)) { /* sector highlighted */ }
      |                                                                     ^
./Include/hlightX.h:190:3: error: expected identifier or ‘(’ before ‘/’ token
  190 |  */
      |   ^
Src/miscA.c: In function ‘sort_news’:
Src/miscA.c:58:43: error: ‘CONQ_SORT’ undeclared (first use in this function)
   58 |   sprintf(string, "%s/%s %s %s", progdir, CONQ_SORT, fname, fname);
      |                                           ^~~~~~~~~
Src/miscA.c:58:43: note: each undeclared identifier is reported only once for each function it appears in
In file included from ./Include/dataX.h:69,
                 from ./Include/dataA.h:62,
                 from Src/miscA.
```

### Src/miscG.c
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/miscG.c:28:
./Include/hlightX.h:189:44: warning: ‘/*’ within comment [-Wcomment]
  189 |  *   if (HIGH_CHECK(x, y, HI_DIPLOMACY)) { /* sector highlighted */ }
./Include/hlightX.h:189:69: error: expected identifier or ‘(’ before ‘}’ token
  189 |  *   if (HIGH_CHECK(x, y, HI_DIPLOMACY)) { /* sector highlighted */ }
      |                                                                     ^
./Include/hlightX.h:190:3: error: expected identifier or ‘(’ before ‘/’ token
  190 |  */
      |   ^
Src/miscG.c: In function ‘jmp_site’:
./Include/hlightX.h:61:25: error: ‘visibility_data’ undeclared (first use in this function)
   61 | #define _VD_(x,y)       visibility_data[(x) + MAPX * (y)]
      |                         ^~~~~~~~~~~~~~~
./Include/hlightX.h:131:26: note: in expansion of macro ‘_VD_’
  131 | #define VIS_CHECK(x,y)  (_VD_(x,y) & SCTR_NMASK)
      |                          ^~~~
Src/miscG.c:283:7: note: in expansion of macro ‘VIS_CHECK’
  283 |   if (VIS_
```

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
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/moveX.c:53:
./Include/hlightX.h:189:44: warning: ‘/*’ within comment [-Wcomment]
  189 |  *   if (HIGH_CHECK(x, y, HI_DIPLOMACY)) { /* sector highlighted */ }
./Include/hlightX.h:189:69: error: expected identifier or ‘(’ before ‘}’ token
  189 |  *   if (HIGH_CHECK(x, y, HI_DIPLOMACY)) { /* sector highlighted */ }
      |                                                                     ^
./Include/hlightX.h:190:3: error: expected identifier or ‘(’ before ‘/’ token
  190 |  */
      |   ^
Src/moveX.c: In function ‘move_cost’:
./Include/hlightX.h:61:25: error: ‘visibility_data’ undeclared (first use in this function)
   61 | #define _VD_(x,y)       visibility_data[(x) + MAPX * (y)]
      |                         ^~~~~~~~~~~~~~~
./Include/hlightX.h:131:26: note: in expansion of macro ‘_VD_’
  131 | #define VIS_CHECK(x,y)  (_VD_(x,y) & SCTR_NMASK)
      |                          ^~~~
Src/moveX.c:445:10: note: in expansion of macro ‘VIS_CHECK’
  445 |         
```

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
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/sectorG.c:62:
./Include/hlightX.h:189:44: warning: ‘/*’ within comment [-Wcomment]
  189 |  *   if (HIGH_CHECK(x, y, HI_DIPLOMACY)) { /* sector highlighted */ }
./Include/hlightX.h:189:69: error: expected identifier or ‘(’ before ‘}’ token
  189 |  *   if (HIGH_CHECK(x, y, HI_DIPLOMACY)) { /* sector highlighted */ }
      |                                                                     ^
./Include/hlightX.h:190:3: error: expected identifier or ‘(’ before ‘/’ token
  190 |  */
      |   ^
Src/sectorG.c: In function ‘change_desg’:
Src/sectorG.c:514:44: warning: comparison of integer expressions of different signedness: ‘size_t’ {aka ‘long unsigned int’} and ‘int’ [-Wsign-compare]
  514 |     if (xloc + strlen(desg_selects[count]) > COLS - 8) {
      |                                            ^
Src/sectorG.c: In function ‘construct’:
Src/sectorG.c:888:31: warning: comparison of integer expressions of different signedness: ‘size_t’ {aka ‘long unsigned int’}
```

### Src/sectorX.c
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/sectorX.c:82:
./Include/hlightX.h:189:44: warning: ‘/*’ within comment [-Wcomment]
  189 |  *   if (HIGH_CHECK(x, y, HI_DIPLOMACY)) { /* sector highlighted */ }
./Include/hlightX.h:189:69: error: expected identifier or ‘(’ before ‘}’ token
  189 |  *   if (HIGH_CHECK(x, y, HI_DIPLOMACY)) { /* sector highlighted */ }
      |                                                                     ^
./Include/hlightX.h:190:3: error: expected identifier or ‘(’ before ‘/’ token
  190 |  */
      |   ^
In file included from ./Include/dataX.h:69,
                 from Src/sectorX.c:71:
Src/sectorX.c: In function ‘majdesg_costs’:
Src/sectorX.c:116:55: warning: unused parameter ‘verbal’ [-Wunused-parameter]
  116 | majdesg_costs PARM_4(int, which, int, x, int, y, int, verbal)
      |                                                  ~~~~~^~~~~~
./Include/paramX.h:178:51: note: in definition of macro ‘PARM_4’
  178 | #define PARM_4(a,b,c,d,e,f,g,h) (a b, c d, e f, g h)
     
```

### Src/selectG.c
**Status**: ✅ Success

### Src/selectX.c
**Status**: ✅ Success

### Src/time_ckG.c
**Status**: ❌ Compilation Error
**Error Output**:
```
Src/time_ckG.c: In function ‘daynum’:
Src/time_ckG.c:98:9: warning: overflow in conversion from ‘enum <anonymous>’ to ‘int’ changes value from ‘4294967295’ to ‘-1’ [-Woverflow]
   98 |   return((daynum_t)TIME_CLOSED);
      |         ^
Src/time_ckG.c: In function ‘doupexit’:
Src/time_ckG.c:443:3: error: too few arguments to function ‘hangup’; expected 1, have 0
  443 |   hangup();
      |   ^~~~~~
In file included from ./Include/dataX.h:2191,
                 from ./Include/dataG.h:56,
                 from Src/time_ckG.c:22:
./Include/fileX.h:780:13: note: declared here
  780 | extern void hangup PL_(( int sig ));
      |             ^~~~~~
Src/time_ckG.c: In function ‘doexit’:
Src/time_ckG.c:480:3: error: too few arguments to function ‘hangup’; expected 1, have 0
  480 |   hangup();
      |   ^~~~~~
./Include/fileX.h:780:13: note: declared here
  780 | extern void hangup PL_(( int sig ));
      |             ^~~~~~
Src/time_ckG.c: In function ‘alrm_handler’:
Src/time_ckG.c:517:19: er
```

### Src/unitsX.c
**Status**: ⚠️ Warnings Only
**Warnings**:
- Src/unitsX.c:2873:29: warning: comparison of integer expressions of different signedness: ‘Aclasstype’ {aka ‘enum aclasstype’} and ‘int’ [-Wsign-compare]
- Src/unitsX.c:3613:37: warning: comparison of integer expressions of different signedness: ‘Aclasstype’ {aka ‘enum aclasstype’} and ‘int’ [-Wsign-compare]

### Src/updateA.c
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/updateA.c:28:
./Include/hlightX.h:189:44: warning: ‘/*’ within comment [-Wcomment]
  189 |  *   if (HIGH_CHECK(x, y, HI_DIPLOMACY)) { /* sector highlighted */ }
./Include/hlightX.h:189:69: error: expected identifier or ‘(’ before ‘}’ token
  189 |  *   if (HIGH_CHECK(x, y, HI_DIPLOMACY)) { /* sector highlighted */ }
      |                                                                     ^
./Include/hlightX.h:190:3: error: expected identifier or ‘(’ before ‘/’ token
  190 |  */
      |   ^
Src/updateA.c: In function ‘upd_army’:
./Include/hlightX.h:61:25: error: ‘visibility_data’ undeclared (first use in this function)
   61 | #define _VD_(x,y)       visibility_data[(x) + MAPX * (y)]
      |                         ^~~~~~~~~~~~~~~
./Include/hlightX.h:131:26: note: in expansion of macro ‘_VD_’
  131 | #define VIS_CHECK(x,y)  (_VD_(x,y) & SCTR_NMASK)
      |                          ^~~~
Src/updateA.c:562:16: note: in expansion of macro ‘VIS_CHECK’
  562 |    
```

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


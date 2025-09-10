# Comprehensive Compilation Analysis Report

**Generated**: 2025-09-10 11:36:34
**Total Files**: 67
**Success Rate**: 23.9%

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
Auxil/psmap.c:14:31: error: conflicting types for ‘fopen’; have ‘FILE *(void)’
   14 | FILE *infile, *outfile, *fh, *fopen();
      |                               ^~~~~
In file included from Auxil/psmap.c:8:
/usr/include/stdio.h:264:14: note: previous declaration of ‘fopen’ with type ‘FILE *(const char * restrict,  const char * restrict)’
  264 | extern FILE *fopen (const char *__restrict __filename,
      |              ^~~~~
Auxil/psmap.c: In function ‘parsepagesize’:
Auxil/psmap.c:45:1: warning: old-style function definition [-Wold-style-definition]
   45 | parsepagesize(buf)
      | ^~~~~~~~~~~~~
Auxil/psmap.c:48:8: error: implicit declaration of function ‘strcmp’ [-Wimplicit-function-declaration]
   48 |   if (!strcmp(buf, "A4"))
      |        ^~~~~~
Auxil/psmap.c:10:1: note: include ‘<string.h>’ or provide a declaration of ‘strcmp’
    9 | #include "psmap.h"
  +++ |+#include <string.h>
   10 | 
Auxil/psmap.c: In function ‘setpagesize’:
Auxil/psmap.c:86:1: warning: old-style fun
```

### Auxil/sort.c
**Status**: ❌ Compilation Error
**Error Output**:
```
Auxil/sort.c:105:12: error: conflicting types for ‘fprintf’; have ‘int(void)’
  105 | extern int fprintf();
      |            ^~~~~~~
In file included from Auxil/sort.c:40:
/usr/include/stdio.h:357:12: note: previous declaration of ‘fprintf’ with type ‘int(FILE *, const char *, ...)’
  357 | extern int fprintf (FILE *__restrict __stream,
      |            ^~~~~~~
Auxil/sort.c:113:12: error: conflicting types for ‘fclose’; have ‘int(void)’
  113 | extern int fclose();
      |            ^~~~~~
/usr/include/stdio.h:184:12: note: previous declaration of ‘fclose’ with type ‘int(FILE *)’
  184 | extern int fclose (FILE *__stream) __nonnull ((1));
      |            ^~~~~~
Auxil/sort.c:114:13: warning: conflicting types for built-in function ‘exit’; expected ‘void(int)’ [-Wbuiltin-declaration-mismatch]
  114 | extern void exit();
      |             ^~~~
Auxil/sort.c:102:1: note: ‘exit’ is declared in header ‘<stdlib.h>’
  101 | #include <string.h>
  +++ |+#include <stdlib.h>
  102 | #endi
```

### Src/adduserA.c
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/adduserA.c:29:
./Include/racesX.h:109:39: warning: ‘/*’ within comment [-Wcomment]
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
./Include/racesX.h:109:63: error: expected identifier or ‘(’ before ‘}’ token
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
      |                                                               ^
./Include/racesX.h:110:6: error: expected identifier or ‘(’ before ‘if’
  110 |  *   if (r_fighters(army_race)) { /* enhanced combat effectiveness */ }
      |      ^~
./Include/racesX.h:111:6: error: expected identifier or ‘(’ before ‘if’
  111 |  *   if (r_traders(nation_race)) { /* improved trade prices */ }
      |      ^~
./Include/racesX.h:113:8: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘:’ token
  113 |  * Note: These macros provide read-only access to racial traits and should
      |        ^
./Include/racesX.h:185:3: warning: data definition has no type or sto
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
In file included from Src/checkX.c:45:
./Include/racesX.h:109:39: warning: ‘/*’ within comment [-Wcomment]
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
./Include/racesX.h:109:63: error: expected identifier or ‘(’ before ‘}’ token
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
      |                                                               ^
./Include/racesX.h:110:6: error: expected identifier or ‘(’ before ‘if’
  110 |  *   if (r_fighters(army_race)) { /* enhanced combat effectiveness */ }
      |      ^~
./Include/racesX.h:111:6: error: expected identifier or ‘(’ before ‘if’
  111 |  *   if (r_traders(nation_race)) { /* improved trade prices */ }
      |      ^~
./Include/racesX.h:113:8: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘:’ token
  113 |  * Note: These macros provide read-only access to racial traits and should
      |        ^
./Include/racesX.h:185:3: warning: data definition has no type or stora
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
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/createA.c:24:
./Include/racesX.h:109:39: warning: ‘/*’ within comment [-Wcomment]
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
./Include/racesX.h:109:63: error: expected identifier or ‘(’ before ‘}’ token
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
      |                                                               ^
./Include/racesX.h:110:6: error: expected identifier or ‘(’ before ‘if’
  110 |  *   if (r_fighters(army_race)) { /* enhanced combat effectiveness */ }
      |      ^~
./Include/racesX.h:111:6: error: expected identifier or ‘(’ before ‘if’
  111 |  *   if (r_traders(nation_race)) { /* improved trade prices */ }
      |      ^~
./Include/racesX.h:113:8: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘:’ token
  113 |  * Note: These macros provide read-only access to racial traits and should
      |        ^
./Include/racesX.h:185:3: warning: data definition has no type or stor
```

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
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/dataX.c:47:
./Include/racesX.h:109:39: warning: ‘/*’ within comment [-Wcomment]
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
./Include/racesX.h:109:63: error: expected identifier or ‘(’ before ‘}’ token
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
      |                                                               ^
./Include/racesX.h:110:6: error: expected identifier or ‘(’ before ‘if’
  110 |  *   if (r_fighters(army_race)) { /* enhanced combat effectiveness */ }
      |      ^~
./Include/racesX.h:111:6: error: expected identifier or ‘(’ before ‘if’
  111 |  *   if (r_traders(nation_race)) { /* improved trade prices */ }
      |      ^~
./Include/racesX.h:113:8: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘:’ token
  113 |  * Note: These macros provide read-only access to racial traits and should
      |        ^
./Include/racesX.h:185:3: warning: data definition has no type or storag
```

### Src/datamagX.c
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/datamagX.c:48:
./Include/spellsX.h:157:37: warning: ‘/*’ within comment [-Wcomment]
  157 |  *   if (si_needcaster(spell_id)) { /* verify unit is spellcaster */ }
./Include/spellsX.h:157:70: error: expected identifier or ‘(’ before ‘}’ token
  157 |  *   if (si_needcaster(spell_id)) { /* verify unit is spellcaster */ }
      |                                                                      ^
./Include/spellsX.h:158:6: error: expected identifier or ‘(’ before ‘if’
  158 |  *   if (si_needfull(spell_id)) { /* verify unit is full magician */ }
      |      ^~
./Include/spellsX.h:159:6: error: expected identifier or ‘(’ before ‘if’
  159 |  *   if (si_nonmonster(spell_id) && is_monster(unit)) { /* deny casting */ }
      |      ^~
./Include/spellsX.h:161:8: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘:’ token
  161 |  * Note: These macros provide read-only access to spell requirements and should
      |        ^
Src/datamagX.c:83:30: warni
```

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
In file included from Src/displayG.c:57:
./Include/racesX.h:109:39: warning: ‘/*’ within comment [-Wcomment]
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
./Include/racesX.h:109:63: error: expected identifier or ‘(’ before ‘}’ token
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
      |                                                               ^
./Include/racesX.h:110:6: error: expected identifier or ‘(’ before ‘if’
  110 |  *   if (r_fighters(army_race)) { /* enhanced combat effectiveness */ }
      |      ^~
./Include/racesX.h:111:6: error: expected identifier or ‘(’ before ‘if’
  111 |  *   if (r_traders(nation_race)) { /* improved trade prices */ }
      |      ^~
./Include/racesX.h:113:8: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘:’ token
  113 |  * Note: These macros provide read-only access to racial traits and should
      |        ^
./Include/racesX.h:185:3: warning: data definition has no type or sto
```

### Src/economyA.c
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/economyA.c:25:
./Include/racesX.h:109:39: warning: ‘/*’ within comment [-Wcomment]
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
./Include/racesX.h:109:63: error: expected identifier or ‘(’ before ‘}’ token
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
      |                                                               ^
./Include/racesX.h:110:6: error: expected identifier or ‘(’ before ‘if’
  110 |  *   if (r_fighters(army_race)) { /* enhanced combat effectiveness */ }
      |      ^~
./Include/racesX.h:111:6: error: expected identifier or ‘(’ before ‘if’
  111 |  *   if (r_traders(nation_race)) { /* improved trade prices */ }
      |      ^~
./Include/racesX.h:113:8: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘:’ token
  113 |  * Note: These macros provide read-only access to racial traits and should
      |        ^
./Include/racesX.h:185:3: warning: data definition has no type or sto
```

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
In file included from Src/hexmapG.c:70:
./Include/racesX.h:109:39: warning: ‘/*’ within comment [-Wcomment]
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
./Include/racesX.h:109:63: error: expected identifier or ‘(’ before ‘}’ token
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
      |                                                               ^
./Include/racesX.h:110:6: error: expected identifier or ‘(’ before ‘if’
  110 |  *   if (r_fighters(army_race)) { /* enhanced combat effectiveness */ }
      |      ^~
./Include/racesX.h:111:6: error: expected identifier or ‘(’ before ‘if’
  111 |  *   if (r_traders(nation_race)) { /* improved trade prices */ }
      |      ^~
./Include/racesX.h:113:8: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘:’ token
  113 |  * Note: These macros provide read-only access to racial traits and should
      |        ^
./Include/racesX.h:185:3: warning: data definition has no type or stor
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
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/infoG.c:57:
./Include/racesX.h:109:39: warning: ‘/*’ within comment [-Wcomment]
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
./Include/racesX.h:109:63: error: expected identifier or ‘(’ before ‘}’ token
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
      |                                                               ^
./Include/racesX.h:110:6: error: expected identifier or ‘(’ before ‘if’
  110 |  *   if (r_fighters(army_race)) { /* enhanced combat effectiveness */ }
      |      ^~
./Include/racesX.h:111:6: error: expected identifier or ‘(’ before ‘if’
  111 |  *   if (r_traders(nation_race)) { /* improved trade prices */ }
      |      ^~
./Include/racesX.h:113:8: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘:’ token
  113 |  * Note: These macros provide read-only access to racial traits and should
      |        ^
./Include/racesX.h:185:3: warning: data definition has no type or storag
```

### Src/ioG.c
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/ioG.c:26:
./Include/racesX.h:109:39: warning: ‘/*’ within comment [-Wcomment]
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
./Include/racesX.h:109:63: error: expected identifier or ‘(’ before ‘}’ token
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
      |                                                               ^
./Include/racesX.h:110:6: error: expected identifier or ‘(’ before ‘if’
  110 |  *   if (r_fighters(army_race)) { /* enhanced combat effectiveness */ }
      |      ^~
./Include/racesX.h:111:6: error: expected identifier or ‘(’ before ‘if’
  111 |  *   if (r_traders(nation_race)) { /* improved trade prices */ }
      |      ^~
./Include/racesX.h:113:8: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘:’ token
  113 |  * Note: These macros provide read-only access to racial traits and should
      |        ^
./Include/racesX.h:185:3: warning: data definition has no type or storage 
```

### Src/ioX.c
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/ioX.c:33:
./Include/spellsX.h:157:37: warning: ‘/*’ within comment [-Wcomment]
  157 |  *   if (si_needcaster(spell_id)) { /* verify unit is spellcaster */ }
./Include/spellsX.h:157:70: error: expected identifier or ‘(’ before ‘}’ token
  157 |  *   if (si_needcaster(spell_id)) { /* verify unit is spellcaster */ }
      |                                                                      ^
./Include/spellsX.h:158:6: error: expected identifier or ‘(’ before ‘if’
  158 |  *   if (si_needfull(spell_id)) { /* verify unit is full magician */ }
      |      ^~
./Include/spellsX.h:159:6: error: expected identifier or ‘(’ before ‘if’
  159 |  *   if (si_nonmonster(spell_id) && is_monster(unit)) { /* deny casting */ }
      |      ^~
./Include/spellsX.h:161:8: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘:’ token
  161 |  * Note: These macros provide read-only access to spell requirements and should
      |        ^
Src/ioX.c: In function ‘fork_edi
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
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/jointG.c:22:
./Include/spellsX.h:157:37: warning: ‘/*’ within comment [-Wcomment]
  157 |  *   if (si_needcaster(spell_id)) { /* verify unit is spellcaster */ }
./Include/spellsX.h:157:70: error: expected identifier or ‘(’ before ‘}’ token
  157 |  *   if (si_needcaster(spell_id)) { /* verify unit is spellcaster */ }
      |                                                                      ^
./Include/spellsX.h:158:6: error: expected identifier or ‘(’ before ‘if’
  158 |  *   if (si_needfull(spell_id)) { /* verify unit is full magician */ }
      |      ^~
./Include/spellsX.h:159:6: error: expected identifier or ‘(’ before ‘if’
  159 |  *   if (si_nonmonster(spell_id) && is_monster(unit)) { /* deny casting */ }
      |      ^~
./Include/spellsX.h:161:8: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘:’ token
  161 |  * Note: These macros provide read-only access to spell requirements and should
      |        ^
In file included from ./Inclu
```

### Src/keybindG.c
**Status**: ✅ Success

### Src/magicA.c
**Status**: ✅ Success

### Src/magicG.c
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/magicG.c:28:
./Include/racesX.h:109:39: warning: ‘/*’ within comment [-Wcomment]
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
./Include/racesX.h:109:63: error: expected identifier or ‘(’ before ‘}’ token
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
      |                                                               ^
./Include/racesX.h:110:6: error: expected identifier or ‘(’ before ‘if’
  110 |  *   if (r_fighters(army_race)) { /* enhanced combat effectiveness */ }
      |      ^~
./Include/racesX.h:111:6: error: expected identifier or ‘(’ before ‘if’
  111 |  *   if (r_traders(nation_race)) { /* improved trade prices */ }
      |      ^~
./Include/racesX.h:113:8: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘:’ token
  113 |  * Note: These macros provide read-only access to racial traits and should
      |        ^
./Include/racesX.h:185:3: warning: data definition has no type or stora
```

### Src/magicX.c
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/magicX.c:19:
./Include/racesX.h:109:39: warning: ‘/*’ within comment [-Wcomment]
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
./Include/racesX.h:109:63: error: expected identifier or ‘(’ before ‘}’ token
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
      |                                                               ^
./Include/racesX.h:110:6: error: expected identifier or ‘(’ before ‘if’
  110 |  *   if (r_fighters(army_race)) { /* enhanced combat effectiveness */ }
      |      ^~
./Include/racesX.h:111:6: error: expected identifier or ‘(’ before ‘if’
  111 |  *   if (r_traders(nation_race)) { /* improved trade prices */ }
      |      ^~
./Include/racesX.h:113:8: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘:’ token
  113 |  * Note: These macros provide read-only access to racial traits and should
      |        ^
./Include/racesX.h:185:3: warning: data definition has no type or stora
```

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
In file included from Src/memoryX.c:44:
./Include/racesX.h:109:39: warning: ‘/*’ within comment [-Wcomment]
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
./Include/racesX.h:109:63: error: expected identifier or ‘(’ before ‘}’ token
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
      |                                                               ^
./Include/racesX.h:110:6: error: expected identifier or ‘(’ before ‘if’
  110 |  *   if (r_fighters(army_race)) { /* enhanced combat effectiveness */ }
      |      ^~
./Include/racesX.h:111:6: error: expected identifier or ‘(’ before ‘if’
  111 |  *   if (r_traders(nation_race)) { /* improved trade prices */ }
      |      ^~
./Include/racesX.h:113:8: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘:’ token
  113 |  * Note: These macros provide read-only access to racial traits and should
      |        ^
./Include/racesX.h:185:3: warning: data definition has no type or stor
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
In file included from Src/miscX.c:43:
./Include/racesX.h:109:39: warning: ‘/*’ within comment [-Wcomment]
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
./Include/racesX.h:109:63: error: expected identifier or ‘(’ before ‘}’ token
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
      |                                                               ^
./Include/racesX.h:110:6: error: expected identifier or ‘(’ before ‘if’
  110 |  *   if (r_fighters(army_race)) { /* enhanced combat effectiveness */ }
      |      ^~
./Include/racesX.h:111:6: error: expected identifier or ‘(’ before ‘if’
  111 |  *   if (r_traders(nation_race)) { /* improved trade prices */ }
      |      ^~
./Include/racesX.h:113:8: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘:’ token
  113 |  * Note: These macros provide read-only access to racial traits and should
      |        ^
./Include/racesX.h:185:3: warning: data definition has no type or storag
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
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/ntninfoG.c:30:
./Include/racesX.h:109:39: warning: ‘/*’ within comment [-Wcomment]
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
./Include/racesX.h:109:63: error: expected identifier or ‘(’ before ‘}’ token
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
      |                                                               ^
./Include/racesX.h:110:6: error: expected identifier or ‘(’ before ‘if’
  110 |  *   if (r_fighters(army_race)) { /* enhanced combat effectiveness */ }
      |      ^~
./Include/racesX.h:111:6: error: expected identifier or ‘(’ before ‘if’
  111 |  *   if (r_traders(nation_race)) { /* improved trade prices */ }
      |      ^~
./Include/racesX.h:113:8: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘:’ token
  113 |  * Note: These macros provide read-only access to racial traits and should
      |        ^
./Include/racesX.h:185:3: warning: data definition has no type or sto
```

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
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/sectorA.c:21:
./Include/racesX.h:109:39: warning: ‘/*’ within comment [-Wcomment]
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
./Include/racesX.h:109:63: error: expected identifier or ‘(’ before ‘}’ token
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
      |                                                               ^
./Include/racesX.h:110:6: error: expected identifier or ‘(’ before ‘if’
  110 |  *   if (r_fighters(army_race)) { /* enhanced combat effectiveness */ }
      |      ^~
./Include/racesX.h:111:6: error: expected identifier or ‘(’ before ‘if’
  111 |  *   if (r_traders(nation_race)) { /* improved trade prices */ }
      |      ^~
./Include/racesX.h:113:8: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘:’ token
  113 |  * Note: These macros provide read-only access to racial traits and should
      |        ^
./Include/racesX.h:185:3: warning: data definition has no type or stor
```

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
In file included from Src/sectorX.c:79:
./Include/racesX.h:109:39: warning: ‘/*’ within comment [-Wcomment]
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
./Include/racesX.h:109:63: error: expected identifier or ‘(’ before ‘}’ token
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
      |                                                               ^
./Include/racesX.h:110:6: error: expected identifier or ‘(’ before ‘if’
  110 |  *   if (r_fighters(army_race)) { /* enhanced combat effectiveness */ }
      |      ^~
./Include/racesX.h:111:6: error: expected identifier or ‘(’ before ‘if’
  111 |  *   if (r_traders(nation_race)) { /* improved trade prices */ }
      |      ^~
./Include/racesX.h:113:8: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘:’ token
  113 |  * Note: These macros provide read-only access to racial traits and should
      |        ^
./Include/racesX.h:185:3: warning: data definition has no type or stor
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
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/unitsX.c:27:
./Include/racesX.h:109:39: warning: ‘/*’ within comment [-Wcomment]
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
./Include/racesX.h:109:63: error: expected identifier or ‘(’ before ‘}’ token
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
      |                                                               ^
./Include/racesX.h:110:6: error: expected identifier or ‘(’ before ‘if’
  110 |  *   if (r_fighters(army_race)) { /* enhanced combat effectiveness */ }
      |      ^~
./Include/racesX.h:111:6: error: expected identifier or ‘(’ before ‘if’
  111 |  *   if (r_traders(nation_race)) { /* improved trade prices */ }
      |      ^~
./Include/racesX.h:113:8: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘:’ token
  113 |  * Note: These macros provide read-only access to racial traits and should
      |        ^
./Include/racesX.h:185:3: warning: data definition has no type or stora
```

### Src/updateA.c
**Status**: ❌ Compilation Error
**Error Output**:
```
In file included from Src/updateA.c:25:
./Include/racesX.h:109:39: warning: ‘/*’ within comment [-Wcomment]
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
./Include/racesX.h:109:63: error: expected identifier or ‘(’ before ‘}’ token
  109 |  *   if (r_magicskill(player_race)) { /* apply magic bonus */ }
      |                                                               ^
./Include/racesX.h:110:6: error: expected identifier or ‘(’ before ‘if’
  110 |  *   if (r_fighters(army_race)) { /* enhanced combat effectiveness */ }
      |      ^~
./Include/racesX.h:111:6: error: expected identifier or ‘(’ before ‘if’
  111 |  *   if (r_traders(nation_race)) { /* improved trade prices */ }
      |      ^~
./Include/racesX.h:113:8: error: expected ‘=’, ‘,’, ‘;’, ‘asm’ or ‘__attribute__’ before ‘:’ token
  113 |  * Note: These macros provide read-only access to racial traits and should
      |        ^
./Include/racesX.h:185:3: warning: data definition has no type or stor
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


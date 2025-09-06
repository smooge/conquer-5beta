# Phase 4 Documentation Strategy - One File Per Session

## Revised Approach

**Problem**: Manual documentation of 50+ files in a single session would consume excessive context and be error-prone.

**Solution**: Document one file per session, commit immediately, then start fresh session for next file.

## File Prioritization Strategy

### Priority 1: Core Game Engine (Session 1-8)
Critical game mechanics and data structures - document these first:

1. **mainA.c** ✅ (Complete - 2 functions documented: fname_char, main)
2. **dataA.c** ✅ (Complete - 0 functions, 3 global variables + 1 array documented)
3. **combatA.c** ✅ (Complete - 29 functions documented: complete combat system)
4. **moveA.c** ✅ (Complete - 1 function documented: npc_movearmy for NPC army movement)
5. **economyA.c** ✅ (Complete - 3 functions documented: upd_sectors, upd_produce, upd_consume)
6. **updateA.c** ✅ (Complete - 14 functions documented: complete turn processing system)
7. **createA.c** ✅ (Complete - 16 functions documented: complete world generation system)
8. **configA.c** ✅ (Complete - 9 functions documented: complete configuration system)

### Priority 2: I/O and Data Management (Session 9-16)
File operations and data persistence:

9. **ioX.c** ✅ (Complete - 13+ key functions documented: comprehensive I/O system)
10. **iodataX.c** ✅ (Complete - 17 functions documented: complete data file I/O system)
11. **dataX.c** ✅ (Complete - 0 functions, comprehensive data structure documentation: game balance tables)
12. **memoryX.c** ✅ (Complete - 30+ functions documented: comprehensive memory management system)
13. **checkX.c** ✅ (Complete - 10+ functions documented: data validation and multi-user coordination)
14. **convertX.c** ✅ (Complete - 16 functions documented: comprehensive data conversion and string utilities)
15. **executeX.c** ✅ (Complete - 10 functions documented: comprehensive command execution system)
16. **computeX.c** - Computational utilities

### Priority 3: User Interface (Session 17-24)
Game interface and display systems:

17. **mainG.c** - Main game interface
18. **displayG.c** ✅ (Complete - 18 major functions documented: map display and UI rendering system)
19. **infoG.c** - Information display
20. **selectG.c** ✅ (Complete - 16 major functions documented: unit selection and navigation system)
21. **hexmapG.c** 🚧 (In Progress - 11 of 32 functions documented: hexagonal map display system)
22. **armyG.c** - Army interface
23. **navyG.c** - Navy interface
24. **magicG.c** - Magic system interface

### Priority 4: Game Content (Session 25-32)
Specific game mechanics and content:

25. **magicA.c** - Magic system logic
26. **monsterA.c** - Monster/NPC behavior
27. **npcA.c** - NPC management
28. **sectorA.c** - Sector management
29. **adduserA.c** - User registration
30. **jointA.c** - Multi-player coordination
31. **mailA.c** - In-game messaging
32. **miscA.c** ✅ (Complete - miscellaneous utilities)

### Priority 5: Remaining Interface and Utilities (Session 33-50)
Lower priority interface and utility modules:

33. **customG.c** - Custom interface elements
34. **caravanG.c** - Caravan interface
35. **enlistG.c** - Enlistment interface
36. **emailG.c** - Email interface
37. **ieditG.c** - Item editing interface
38. **jointG.c** - Joint operation interface
39. **keybindG.c** - Key binding interface
40. **mailG.c** - Mail interface
41. **miscG.c** - Miscellaneous interface
42. **moveG.c** - Movement interface
43. **ntninfoG.c** - Nation info interface
44. **pagerG.c** - Paging interface
45. **regionG.c** - Region interface
46. **sectorG.c** - Sector interface
47. **xferG.c** - Transfer interface
48. **time_ckG.c** - Time check utilities
49. **customX.c** - Custom utilities
50. **vms.c** - VMS-specific code

### Utility Files (As Needed)
- **getopt.c** - Command line parsing (standard library)
- Remaining *X.c files - Various utilities

## Session Workflow per File

### 1. Session Start
- Load this strategy file
- Check current progress (which file to work on next)
- Read the target file to understand its functions

### 2. Documentation Work
- Use `document_functions.py` script for initial analysis
- Manually review and improve all function documentation
- Focus on quality over speed - one file at a time

### 3. Session End
- Git commit the documented file
- Update progress in this strategy file
- Save session memory with next file to work on
- End session cleanly

## Progress Tracking

### Completed Files ✅
1. **miscA.c** - Session completed 2025-09-05 (7 functions documented)
2. **mainA.c** - Session completed 2025-09-05 (2 functions documented: fname_char, main)
3. **dataA.c** - Session completed 2025-09-05 (0 functions, 3 global variables + 1 array documented)
4. **combatA.c** - Session completed 2025-09-05 (29 functions documented: complete combat system)
5. **moveA.c** - Session completed 2025-09-05 (1 function documented: npc_movearmy)
6. **economyA.c** - Session completed 2025-09-05 (3 functions documented: complete economic system)
7. **updateA.c** - Session completed 2025-09-05 (14 functions documented: complete turn processing system)
8. **createA.c** - Session completed 2025-09-05 (16 functions documented: complete world generation system)
9. **configA.c** - Session completed 2025-09-05 (9 functions documented: complete configuration system)
10. **ioX.c** - Session completed 2025-09-05 (13+ key functions documented: comprehensive I/O system)
11. **iodataX.c** - Session completed 2025-09-05 (17 functions documented: complete data file I/O system)
12. **dataX.c** - Session completed 2025-09-06 (0 functions, comprehensive data structure documentation: game balance tables)
13. **memoryX.c** - Session completed 2025-09-06 (30+ functions documented: comprehensive memory management system)
14. **checkX.c** - Session completed 2025-09-06 (10+ functions documented: data validation and multi-user coordination)
15. **convertX.c** - Session completed 2025-09-06 (16 functions documented: comprehensive data conversion and string utilities)
16. **executeX.c** - Session completed 2025-09-06 (10 functions documented: comprehensive command execution system)
17. **computeX.c** - Session completed 2025-09-06 (20 functions documented: comprehensive economic computation engine)
18. **mainG.c** - Session completed 2025-09-06 (3 functions documented: main game interface and command processing)
19. **displayG.c** - Session completed 2025-09-06 (18 major functions documented: map display and UI rendering system)
20. **selectG.c** - Session completed 2025-09-06 (16 major functions documented: unit selection and navigation system)
21. **hexmapG.c** - Session completed 2025-09-06 (32 functions documented: complete hexagonal map display system)

### PRIORITY 1 COMPLETE! 🎉
**All Priority 1 Core Game Engine files have been documented!**

### PRIORITY 2 COMPLETE! 🎉  
**All Priority 2 I/O and Data Management files have been documented!**

### Current Target
**HEXMAPG.C COMPLETE!** ✅ All 32 functions documented
**Next file**: `infoG.c` (information display system)

### Session Metrics Goal
- Target: 1 file per session
- Expected: 5-15 functions per file
- Commit: Immediately after completing file documentation
- Context preservation: Always maintain clean session boundaries

## Benefits of This Approach

1. **Context Efficiency**: Each session focuses on understanding one file deeply
2. **Quality Control**: Thorough documentation review without context pressure
3. **Progress Tracking**: Clear milestones with git commits per file
4. **Maintainable**: Easy to resume work in any future session
5. **Risk Mitigation**: No loss of work due to context limits
6. **Debugging**: Each file can be tested individually if needed

## Implementation Notes

- Always commit documentation before starting modernization of any file
- Use git branches if working on multiple priorities simultaneously
- Keep automation script updated for better K&R function detection
- Document magic numbers and unclear logic patterns as discovered
- Preserve existing comments and enhance them rather than replacing
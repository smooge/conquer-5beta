# Memory File Naming Convention

**Created**: 2025-09-09
**Purpose**: Standardize memory file naming for better organization and retrieval

## Standard Naming Format

```
PRIO{N}_PHASE{N}_{TYPE}_{COMPONENT}_{YYYYMMDD}_{HHMMSS}.md
```

### Components:

1. **PHASE{N}**: Current phase number (PHASE4, PHASE5, etc.)
2. **TYPE**: Type of memory file
   - `SESSION` - Individual session completion
   - `MILESTONE` - Major milestone achievement
   - `STATUS` - Current progress status
   - `STRATEGY` - Planning and strategy documents
   - `CHECKPOINT` - Mid-session progress saves
3. **COMPONENT**: What was worked on
   - File name (e.g., `buildA_h`, `nclassX_h`)
   - System name (e.g., `HEADERS`, `SOURCES`)
   - Priority level (e.g., `PRIORITY1`, `PRIORITY4`)
4. **YYYYMMDD**: Date (e.g., `20250909`)
5. **HHMMSS**: Time in 24-hour format (e.g., `143022`)

## Examples:

- `PHASE4_SESSION_nclassX_h_20250909_143022.md` - Session completing nclassX.h
- `PHASE4_MILESTONE_PRIORITY4_25PCT_20250909_143500.md` - 25% milestone for Priority 4
- `PHASE4_STATUS_CURRENT_20250909_144000.md` - Current status update
- `PHASE4_STRATEGY_DOCUMENTATION_20250909_120000.md` - Documentation strategy
- `PHASE4_CHECKPOINT_buildA_h_20250909_141500.md` - Mid-session checkpoint

## Special Files:

- `CURRENT_STATUS.md` - Always current status (no timestamp)
- `DOCUMENTATION_STRATEGY.md` - Always current strategy (no timestamp)
- `NEXT_SESSION_INSTRUCTIONS.md` - Instructions for next session
- `MEMORY_FILE_NAMING_CONVENTION.md` - This file (no timestamp)

## Migration Plan:

1. ✅ **Rename existing files to new standard** - Key active files renamed
2. ✅ **Update any references in other files** - Historical references preserved for accuracy
3. ✅ **Commit the reorganization** - All changes committed to git
4. ✅ **Use new standard going forward** - All future files will use this convention

## Migration Completed:

The following key files were renamed to the new standard:
- `PRIORITY_4_PHASE_2_SESSION.md` → `PHASE4_SESSION_statusX_h_rmailX_h_20250909_080000.md`
- `PRIORITY_4_PHASE_3_SESSION.md` → `PHASE4_SESSION_buildA_h_20250909_090000.md`
- `PRIORITY_4_PHASE_4_SESSION.md` → `PHASE4_SESSION_nclassX_h_20250909_090300.md`
- `PHASE_4_CURRENT_STATUS.md` → `CURRENT_STATUS.md` (special file)
- `PHASE_4_DOCUMENTATION_STRATEGY.md` → `DOCUMENTATION_STRATEGY.md` (special file)

Historical files were left with original names to preserve accuracy of session records.

## Benefits:

- **Chronological sorting**: Files sort naturally by date/time
- **Clear categorization**: Easy to find session vs milestone vs status files
- **Phase organization**: Clear phase progression tracking
- **Component identification**: Immediately know what was worked on
- **Consistent format**: Reduces cognitive load when organizing

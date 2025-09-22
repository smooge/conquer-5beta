# Checkpoint Methodology - SIMPLIFIED

## The Rule: 6 Functions Per Session

1. **Pick 6 functions** (or fewer if file is small)
2. **Analyze them completely**
3. **Git commit immediately**
4. **Create memory file**
5. **End session**

## Step-by-Step Process

### During Session:
- Work on exactly 6 functions
- Add complete documentation to each
- Don't move to next batch in same session

### After 6 Functions:
```bash
git add .
git commit -m "Analysis: [filename] functions [X-Y]

Added documentation to functions 1-6

Progress: 6 of [total] functions complete

🤖 Generated with Claude Code
Co-Authored-By: Claude <noreply@anthropic.com>"
```

### Update Classification Registry:
Add your 6 functions to `_modernization/claude/reports/FUNCTION_TESTING_CLASSIFICATION.md`

### Create Memory File:
Save to `_modernization/memory/PHASE3_SESSION_[filename]_functions[X-Y]_[date]_[time].md`:

```markdown
# Session Complete: [filename] Functions [X-Y]

**Status**: ✅ DONE
**Next**: Functions [X+1] to [X+6]

## What I Did:
- [function1] - documented
- [function2] - documented
- [etc.]

## Next Session:
Start with function [X+1]
```

### End Session:
- Don't continue to next batch
- Claude should end the session here

## Success Results:
- combatA.c: 29 functions (5 sessions)
- updateA.c: 17 functions (3 sessions)
- createA.c: 16 functions (4 sessions)
- 100% completion rate, zero context issues
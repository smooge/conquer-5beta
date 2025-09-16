# CLAUDE.md Maintenance Strategy

**Purpose**: Systematic approach to keep CLAUDE.md clear, consistent, and conflict-free as the modernization project evolves.

**Trigger**: Run this audit process monthly or after discovering conflicts/unclear guidance.

---

## Audit Process

### Phase 1: Conflict Detection

#### 1.1 Search for Conflicting Instructions
```bash
# Commands to run to find potential conflicts
grep -n -i "must\|should\|always\|never" CLAUDE.md | head -20
grep -n -A3 -B3 "IMPORTANT\|CRITICAL\|NOTE" CLAUDE.md
```

#### 1.2 Common Conflict Patterns
- **Contradictory requirements**: "Always do X" vs "Only do X when..."
- **Duplicate sections**: Same guidance in multiple places with different details
- **Outdated phase references**: Instructions for completed phases that conflict with current practices
- **Ambiguous triggers**: Unclear when to apply certain rules (like session management)

### Phase 2: Content Analysis

#### 2.1 Section Review Checklist
For each major section, verify:
- [ ] **Clear purpose statement** - Section explains what it covers
- [ ] **Specific actionable guidance** - No vague "should consider" language
- [ ] **Consistent terminology** - Same terms used throughout document
- [ ] **Current phase relevance** - Instructions match current project status
- [ ] **No contradictions** - Section doesn't conflict with other sections

#### 2.2 Cross-Reference Validation
- [ ] **Session Management** consistency across all references
- [ ] **Build Commands** match throughout document
- [ ] **File Paths** are accurate and consistent
- [ ] **Tool Usage** instructions don't conflict
- [ ] **Phase Ordering** matches actual implementation experience

### Phase 3: Cleanup Actions

#### 3.1 Immediate Fixes
- **Remove contradictions** - Choose best practice, remove conflicting guidance
- **Consolidate duplicates** - Merge similar sections, cross-reference where needed
- **Update outdated content** - Remove or update instructions for completed phases
- **Clarify ambiguous language** - Replace "should consider" with specific criteria

#### 3.2 Structural Improvements
- **Add section summaries** - Key points at start of long sections
- **Create cross-references** - Link related sections explicitly
- **Standardize formatting** - Consistent use of headers, code blocks, examples
- **Version control** - Document when major changes are made

---

## Maintenance Schedule

### Weekly (During Session Management)
- [ ] Check for conflicts in sections related to current work
- [ ] Update phase status references if phases completed
- [ ] Verify new guidance doesn't contradict existing content

### Monthly (Comprehensive Audit)
- [ ] Run conflict detection commands
- [ ] Review all major sections using checklist
- [ ] Cross-reference validation
- [ ] Implement cleanup actions identified
- [ ] Commit maintenance changes with clear commit message

### After Major Changes
- [ ] When adding new sections >50 lines
- [ ] When discovering conflicts (like session management issue)
- [ ] When completing major phases
- [ ] When changing fundamental project approach

---

## Known Issues to Watch For

### Session Management
- ✅ **Fixed**: Memory files vs session logs confusion
- **Monitor**: Ensure new guidance stays consistent

### Build Commands
- **Risk**: Multiple build command examples throughout document
- **Action**: Verify all build commands are identical and current

### Phase Instructions
- **Risk**: Instructions for completed phases may conflict with current practices
- **Action**: Mark completed phases clearly, update for lessons learned

### Tool Usage
- **Risk**: Instructions for TodoWrite, git commits, file operations may conflict
- **Action**: Consolidate tool usage guidelines in one section

---

## Implementation Tools

### Automated Checks (run monthly)
```bash
# Check for potential conflicts
grep -n -i "must\|always\|never\|should\|critical\|important" CLAUDE.md > claude_audit.txt

# Check for duplicate section headers
grep -n "^##" CLAUDE.md | sort | uniq -d

# Check for inconsistent build commands
grep -n "gcc\|clang" CLAUDE.md

# Check for inconsistent paths
grep -n "_modernization\|tests\|Src" CLAUDE.md
```

### Manual Review Areas
1. **Session Management** - Any references to saving files, git commits, documentation
2. **Build System** - All compiler commands, flags, build instructions
3. **Testing Strategy** - Test creation, framework usage, validation approaches
4. **Phase Instructions** - Current phase vs completed phase guidance
5. **File Organization** - Directory structure, naming conventions, file purposes

---

## Success Metrics

### Quality Indicators
- **No contradictory requirements** found during audits
- **Clear action items** - Every "should" has specific criteria
- **Consistent terminology** throughout document
- **Current relevance** - All active instructions apply to current phase

### Maintenance Efficiency
- **Monthly audits** take <30 minutes
- **Conflict resolution** completed within 1 session
- **New additions** checked for conflicts before committing
- **User feedback** about unclear guidance addressed promptly

---

## Emergency Conflict Resolution

When conflicts are discovered during active work:

1. **Immediate**: Choose most practical approach for current task
2. **Document**: Add item to PERSISTENT_TODOS.md for CLAUDE.md cleanup
3. **Quick Fix**: Add clarifying note in relevant section
4. **Schedule**: Plan full section review in next weekly audit
5. **Commit**: Include conflict resolution in commit message

---

Generated: 2025-09-16
Last Updated: 2025-09-16
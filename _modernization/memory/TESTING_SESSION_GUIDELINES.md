# Testing Session Guidelines

**Purpose**: Prevent retesting attempts and ensure efficient session planning
**Primary Reference**: `_modernization/claude/reports/FUNCTION_TESTING_CLASSIFICATION.md`

---

## Pre-Session Checklist

### 1. Consultation Required ⚡ MANDATORY
**ALWAYS check before starting unit testing**:
```bash
# Read classification file FIRST
cat _modernization/claude/reports/FUNCTION_TESTING_CLASSIFICATION.md
```

### 2. Target File Analysis
- **Check Registry**: Is target file already classified?
- **If Listed**: Follow documented testing approach
- **If New**: Analyze complexity before proceeding

### 3. Session Type Selection
- **Category A Functions**: Standard unit testing session
- **Category B Functions**: Integration testing session
- **Category C Functions**: System testing session (defer)
- **Category D Functions**: Mocking strategy session
- **Category E Functions**: Skip/document session

---

## Session Decision Framework

### Quick Classification Test
```
Does function require:
├── Global world state? → Category B (Integration)
├── Full game initialization? → Category C (System)
├── Heavy I/O operations? → Category D (Mock Intensive)
├── Complex multi-module coordination? → Category C (System)
└── None of above? → Category A (Unit Testable)
```

### Red Flags for Unit Testing (Stop and Reclassify)
- ❌ 10+ undefined references during linking
- ❌ Requires `upd_init()` or similar initialization
- ❌ Uses `m2alloc()` or complex memory management
- ❌ References multiple global arrays (`sct`, `world`, etc.)
- ❌ File I/O operations (`fupdate`, `fnews`, etc.)

---

## Session Types and Approaches

### Category A: Standard Unit Testing ✅
**Target**: Simple, isolated functions
**Session Length**: 1-2 hours
**Success Criteria**: Full test coverage, all tests pass
**Examples**: String utilities, calculations, data validation

### Category B: Integration Testing 🔧
**Target**: Functions requiring world state
**Session Length**: 2-3 hours
**Success Criteria**: Crash prevention, basic functionality
**Setup Required**: Minimal world initialization
**Focus**: Core algorithm testing, memory safety

### Category C: System Testing 🌐
**Target**: Complex game engine functions
**Session Length**: 3-4 hours
**Success Criteria**: Behavioral validation
**Approach**: High-level functional testing
**Note**: Defer until modernization phase

### Category D: Mock Intensive 🎭
**Target**: I/O heavy but logically testable
**Session Length**: 2-4 hours
**Success Criteria**: Logic testing with mocked dependencies
**Requirements**: Extensive mocking framework

### Category E: Documentation Only ⏸️
**Target**: Complex legacy functions
**Session Length**: 30-60 minutes
**Success Criteria**: Document rationale for skipping
**Approach**: Analysis and classification only

---

## Session Efficiency Rules

### Time Investment Guidelines
- **Category A**: High ROI - proceed immediately
- **Category B**: Medium ROI - batch multiple functions
- **Category C**: Low ROI - defer until system testing phase
- **Category D**: Variable ROI - assess mocking cost
- **Category E**: Documentation only - minimal time investment

### Batch Processing Strategy
- **Group similar categories** for efficient session planning
- **Target Category A functions first** for quick wins
- **Batch Category B functions** for integration sessions
- **Document Category C/E functions** for future planning

---

## Documentation Requirements

### Always Update After Session
1. **Classification Registry**: Add new function categories
2. **Session Memory**: Record classification decisions
3. **Progress Tracking**: Update completion status
4. **Lessons Learned**: Note any new patterns discovered

### Required Memory File Updates
- `UNIT_TEST_PROGRESS.md` - Overall progress tracking
- `FUNCTION_TESTING_CLASSIFICATION.md` - Primary registry
- Session-specific memory file - Detailed findings

---

## Anti-Patterns to Avoid

### ❌ DON'T: Attempt unit testing without classification check
### ❌ DON'T: Spend >1 hour on untestable functions
### ❌ DON'T: Retry functions already classified as Category B/C
### ❌ DON'T: Skip documentation of classification decisions

### ✅ DO: Check classification first
### ✅ DO: Document rationale for all decisions
### ✅ DO: Update registry with new findings
### ✅ DO: Follow category-appropriate testing strategies

---

## Success Metrics

### Session Success Indicators
- **Category A**: 100% test coverage achieved
- **Category B**: Crash prevention tests pass
- **Category C**: Behavioral analysis documented
- **Category D**: Logic tests pass with mocks
- **Category E**: Classification documented with rationale

### Project Success Tracking
- **Total Functions Analyzed**: Track across all categories
- **Unit Testing Success Rate**: Category A completion percentage
- **Integration Testing Identified**: Category B function count
- **Modernization Priorities**: Category C function impact assessment

---

**Usage Instructions**:
- Read this file before EVERY testing session
- Reference classification system for efficiency
- Update documentation after session completion
- Use for planning multi-session testing campaigns

Generated by Claude (claude-sonnet-4@20250514)
Created: 2025-09-17 - Testing Session Efficiency Guidelines
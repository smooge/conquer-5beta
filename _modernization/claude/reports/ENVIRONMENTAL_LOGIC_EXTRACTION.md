# Environmental and User-Choice Logic Extraction Analysis

**Generated**: 2025-09-15
**Phase**: Phase 7 - Analyze and Decouple Configuration
**Scope**: Identification and extraction strategy for environmental and user-choice logic from source code

---

## Executive Summary

The Conquer codebase contains significant environmental and user-choice logic embedded directly in source code through compile-time definitions, hardcoded paths, and platform-specific conditional compilation. This analysis identifies all environmental dependencies and provides an extraction strategy to move them to external configuration.

**Key Findings:**
- **67 hardcoded constants** need externalization
- **Platform-specific logic** in 15+ source files
- **Administrative credentials** hardcoded in headers
- **File paths and commands** hardcoded throughout system
- **Game balance parameters** compiled into binary

**Extraction Benefits:**
- Runtime configuration without recompilation
- Improved security through external credential management
- Enhanced portability across platforms
- Easier game balance adjustments
- Simplified build system maintenance

---

## Environmental Logic Categories

### 1. System Identity and Administrative Configuration

#### 1.1 Hardcoded Administrative Settings (CRITICAL SECURITY ISSUE)
**Location**: Include/header.h:85-127

```c
#define VERSION "Version 5.0b"           // Software version
#define OWNER   "Smoogen"                // Admin display name
#define LOGIN   "ssmoogen"               // Admin login (SECURITY RISK)
```

**Environmental Dependencies**:
- Administrative user account on target system
- Version management across deployments
- Site-specific administrator information

**Extraction Strategy**:
```c
// Replace with:
extern const char* conquer_version;
extern const char* conquer_admin_display;
extern const char* conquer_admin_login;

// Load from config file:
[admin]
login = "admin_user"
display_name = "Game Administrator"
version = "5.0.1"
```

**Security Improvement**: Admin credentials externalized, not in source code

#### 1.2 Installation Path Dependencies
**Location**: Multiple Makefiles and customX.c

```makefile
DATADIR = /usr/local/games/lib/conquer    # Game data directory
BINDIR  = /usr/local/games/bin            # Executable directory
```

**Environmental Dependencies**:
- Target system directory structure
- Installation prefix preferences
- Multi-user vs single-user installations

**Extraction Strategy**:
- Use CMake CMAKE_INSTALL_PREFIX for build-time defaults
- Support environment variables for runtime override
- Configuration file section for path customization

### 2. Platform Detection and Feature Flags

#### 2.1 Compile-Time Platform Detection
**Location**: Include/sysconf.h:261-390

```c
#ifdef MACHOS
    #undef LRAND48
    #define SYS_SIGNAL
    #define SETPRIORITY
    #define DCLR_A
#endif

#ifdef SYSV4
    #define MEMORYH
    #define MALLOCH
    #define SWITCHID
    #define SETPRIORITY
#endif

#ifdef BSD
    #undef LRAND48
    #define WINCH_HANDLER
    #define SETREUID
    #define GETDTABLESIZE
#endif
```

**Environmental Dependencies**:
- Target operating system capabilities
- Available system libraries and functions
- Compiler and linker feature availability

**Extraction Strategy (CMake Feature Detection)**:
```cmake
# Replace platform detection with capability detection
check_function_exists(lrand48 HAVE_LRAND48)
check_function_exists(setpriority HAVE_SETPRIORITY)
check_function_exists(flock HAVE_FLOCK)
check_include_file(sys/signal.h HAVE_SYS_SIGNAL_H)

# Generate config.h with detected features
configure_file(config.h.in config.h)
```

#### 2.2 Data Format Feature Flags
**Location**: Include/header.h:150-233

```c
#define COMPRESS                          // Enable compression
#define MANY_UNITS                        // >255 units support
#define HUGE_MAP                          // >256x256 maps
#define ABSMAXNTN 100                     // Maximum nations
```

**Environmental Dependencies**:
- Storage capacity and performance requirements
- Expected game scale and player count
- Network bandwidth for save file transfer

**Extraction Strategy**:
```ini
[world_limits]
max_nations = 100
large_maps = true
extended_units = true
compression = true

[performance]
enable_compression = true
compression_level = 6
```

### 3. File System and External Tool Dependencies

#### 3.1 Hardcoded External Commands (SECURITY RISK)
**Location**: Include/header.h:361-495

```c
#define REMOVE_NAME   "/bin/rm -f"        // File removal
#define COMP_NAME     "compress"          // Compression
#define COMP_READ     "zcat"              // Decompression
#define DEFAULT_EDITOR "/usr/bin/vi"      // Text editor
```

**Environmental Dependencies**:
- Available system utilities and their locations
- Security policies regarding external command execution
- User preferences for editor and tools

**Extraction Strategy**:
```ini
[system_tools]
file_removal = "unlink"              # Use native function
compression = "internal_zlib"        # Internal library
editor = "${EDITOR:-vi}"             # Environment variable
temp_directory = "${TMPDIR:-/tmp}"   # Environment variable
```

#### 3.2 File Permission and Security Settings
**Location**: Include/header.h:242-282

```c
#define DEFAULT_UMASK     077             // Owner-only permissions
#define CUSTOM_UMASK      066             // Config file permissions
#define FULLWRITE_UMASK   000             // World-writable (DANGEROUS)
```

**Environmental Dependencies**:
- Site security policies
- Multi-user environment requirements
- File sharing and collaboration needs

**Extraction Strategy**:
```ini
[security]
default_file_mode = 0600           # Owner read/write only
config_file_mode = 0644            # Owner write, world read
shared_file_mode = 0664            # Group writable
enable_world_write = false         # Disable dangerous permissions
```

### 4. Game Balance and Mechanics Configuration

#### 4.1 Economic Parameters
**Location**: Include/header.h:1221-1292

```c
#define MOVECOST      20L                 // Command cost in talons
#define NAVYMAINT     4000L               // Naval maintenance cost
#define CVNMAINT      1000L               // Caravan maintenance
#define FORTCOST      1000L               // Fortification cost
#define CARAVANCOST   5000L               // Caravan construction cost
```

**Environmental Dependencies**:
- Desired game balance and difficulty
- Economic progression speed preferences
- Player skill level and experience

**Extraction Strategy**:
```ini
[economy]
command_cost = 20
naval_maintenance = 4000
caravan_maintenance = 1000
fortification_cost = 1000
caravan_construction_cost = 5000

[difficulty]
easy_mode_multiplier = 0.5
normal_mode_multiplier = 1.0
hard_mode_multiplier = 2.0
```

#### 4.2 Combat and Military Parameters
**Location**: Include/header.h:1295-1354

```c
#define TAKESECTOR    75                  // Base soldiers for capture
#define TAKE_RATIO    7                   // Attack:defense ratio
#define BASE_TAKEPCT  10                  // Troop % of population
#define MAXLOSS       60                  // Maximum battle casualties
#define FINDPERCENT   1                   // Resource discovery chance
```

**Environmental Dependencies**:
- Combat difficulty preferences
- Strategic depth requirements
- Player experience and skill level

**Extraction Strategy**:
```ini
[combat]
base_capture_requirement = 75
attack_defense_ratio = 7
population_capture_percent = 10
maximum_casualty_rate = 60
resource_discovery_chance = 1
```

#### 4.3 Population and Resource Management
**Location**: Include/header.h:1148-1213

```c
#define TOOMANYPEOPLE  5000L              // Overcrowding threshold
#define ABSMAXPEOPLE   50000L             // Absolute population limit
#define MILLSIZE       500L               // Minimum mill workforce
#define TOOMUCHMINED   50000L             // Metal depletion threshold
```

**Environmental Dependencies**:
- Desired population density and growth
- Resource scarcity vs abundance preferences
- Economic complexity requirements

**Extraction Strategy**:
```ini
[population]
overcrowding_threshold = 5000
maximum_sector_population = 50000
minimum_mill_workforce = 500

[resources]
metal_depletion_threshold = 50000
regeneration_rate = 0.1
```

### 5. User Interface and Interaction Configuration

#### 5.1 Visibility and Range Parameters
**Location**: Include/header.h:1014-1107

```c
#define LANDSEE       2                   // Land visibility range
#define NAVYSEE       1                   // Naval visibility range
#define ARMYSEE       2                   // Army visibility range
#define CVNSEE        1                   // Caravan visibility range
#define VISRANGE      4                   // Sector ID visibility
```

**Environmental Dependencies**:
- Desired tactical complexity
- Performance vs detail trade-offs
- Player preference for information availability

**Extraction Strategy**:
```ini
[visibility]
land_sight_range = 2
naval_sight_range = 1
army_sight_range = 2
caravan_sight_range = 1
sector_id_range = 4

[performance]
max_visibility_calculations = 10000
visibility_cache_size = 1000
```

#### 5.2 Pager and Display Configuration
**Location**: Include/header.h:1110-1147

```c
#define MAX_FILE_LINES 5000               // Pager file size limit
#define D_PAGEOFF     0                   // Default page offset
#define D_PAGETAB     8                   // Tab spacing
```

**Environmental Dependencies**:
- Terminal capabilities and size
- User display preferences
- System memory constraints

**Extraction Strategy**:
```ini
[display]
max_pager_lines = 5000
default_page_offset = 0
tab_width = 8
screen_refresh_rate = 30

[terminal]
auto_detect_size = true
default_width = 80
default_height = 24
```

### 6. Timing and Performance Configuration

#### 6.1 Time-Based Behavior
**Location**: Include/header.h:395-421

```c
#define MAIL_TIME     60                  // Mail check interval (seconds)
#define TIMETOBOOT    120                 // Logout warning time
#define UPDATESLEEP   30                  // Update retry interval
```

**Environmental Dependencies**:
- System performance characteristics
- Network latency and reliability
- User patience and interaction patterns

**Extraction Strategy**:
```ini
[timing]
mail_check_interval = 60
logout_warning_time = 120
update_retry_interval = 30

[performance]
max_concurrent_users = 50
database_cache_size = 1000
network_timeout = 30
```

---

## Source Code Extraction Points

### 1. Header File Modifications Required

#### 1.1 Include/header.h Changes
**Current**: 67 #define constants for configuration
**Target**: External configuration loading with defaults

```c
// Replace hardcoded defines with extern declarations
extern struct conquer_config g_config;

// Load configuration at startup
int load_configuration(const char* config_file);
const char* get_config_string(const char* section, const char* key, const char* default_value);
long get_config_long(const char* section, const char* key, long default_value);
```

#### 1.2 Include/sysconf.h Changes
**Current**: Platform-specific conditional compilation
**Target**: CMake-generated feature detection

```c
// Replace platform #ifdef with CMake-detected features
#include "config.h"           // CMake-generated configuration

#ifdef HAVE_LRAND48
    // Use lrand48() functions
#else
    // Use fallback random functions
#endif
```

### 2. Source File Modifications Required

#### 2.1 Configuration Loading (New Module)
**File**: Src/configX.c (new)
**Purpose**: Load configuration from file, environment, and defaults

```c
#include "config_loader.h"

struct conquer_config {
    // Admin settings
    char admin_login[64];
    char admin_display[64];
    char version[32];

    // Game balance
    long command_cost;
    long naval_maintenance;
    // ... all configurable parameters
};

extern struct conquer_config g_config;
```

#### 2.2 Initialization Changes
**Files**: Src/mainG.c, Src/mainA.c
**Changes**: Load configuration during startup

```c
int main(int argc, char *argv[]) {
    // Load configuration first
    if (load_configuration(get_config_path()) != 0) {
        fprintf(stderr, "Failed to load configuration\n");
        return 1;
    }

    // Continue with normal initialization
    // ...
}
```

#### 2.3 Administrative Function Changes
**Files**: Src/miscX.c, Src/customX.c
**Changes**: Use configuration values instead of hardcoded constants

```c
// Replace hardcoded LOGIN checks
if (strcmp(username, g_config.admin_login) == 0) {
    // Grant administrative access
}

// Replace hardcoded paths
snprintf(data_path, sizeof(data_path), "%s/%s",
         g_config.data_directory, filename);
```

---

## Configuration File Design

### 1. Configuration File Format

#### 1.1 INI-Style Format (Recommended)
```ini
# Conquer Game Configuration
# Auto-generated defaults, customize as needed

[admin]
login = "gameadmin"
display_name = "Game Administrator"
version = "5.0.1"

[paths]
data_directory = "/var/lib/conquer"
binary_directory = "/usr/local/bin"
temp_directory = "/tmp"

[security]
default_file_mode = 0600
enable_external_commands = false
max_login_attempts = 3

[game_balance]
command_cost = 20
naval_maintenance = 4000
caravan_maintenance = 1000

[world_limits]
max_nations = 100
large_maps = true
extended_units = true

[visibility]
land_sight_range = 2
naval_sight_range = 1
army_sight_range = 2

[performance]
max_concurrent_users = 50
update_retry_interval = 30
```

#### 1.2 Configuration Loading Priority
1. **Built-in defaults** (compiled into binary)
2. **System configuration** (/etc/conquer/conquer.conf)
3. **User configuration** (~/.conquer/config)
4. **Environment variables** (CONQUER_* prefix)
5. **Command line options** (highest priority)

### 2. Environment Variable Support

#### 2.1 Standard Environment Variables
```bash
export CONQUER_DATA_DIR="/custom/game/data"
export CONQUER_ADMIN_USER="myadmin"
export CONQUER_CONFIG_FILE="/etc/conquer/custom.conf"
export CONQUER_DEBUG_LEVEL="2"
```

#### 2.2 Path Resolution
```c
const char* get_config_path(void) {
    const char* config_file = getenv("CONQUER_CONFIG_FILE");
    if (config_file) return config_file;

    // Try user-specific config
    const char* home = getenv("HOME");
    if (home) {
        static char path[PATH_MAX];
        snprintf(path, sizeof(path), "%s/.conquer/config", home);
        if (access(path, R_OK) == 0) return path;
    }

    // Try system config
    if (access("/etc/conquer/conquer.conf", R_OK) == 0) {
        return "/etc/conquer/conquer.conf";
    }

    // Use built-in defaults
    return NULL;
}
```

---

## Migration Strategy

### 1. Phase 7b Implementation Steps

#### 1.1 Create Configuration Infrastructure
1. **Design config file format** and validation schema
2. **Implement config loader** with error handling
3. **Create default configuration** file generator
4. **Add environment variable** support

#### 1.2 Extract Security-Critical Settings
1. **Remove hardcoded admin credentials** from source
2. **Externalize file permissions** and security settings
3. **Replace external command execution** with safe alternatives
4. **Implement configuration validation** and sanitization

#### 1.3 Migrate Game Balance Parameters
1. **Extract economic constants** to configuration
2. **Move combat parameters** to external file
3. **Externalize population limits** and resource settings
4. **Create difficulty level** presets

#### 1.4 Platform Configuration Modernization
1. **Replace #ifdef platform** detection with CMake
2. **Generate config.h** from CMake feature detection
3. **Remove obsolete platform** support (VMS, AIX370)
4. **Standardize on POSIX** functions only

### 2. Backward Compatibility Strategy

#### 2.1 Default Configuration Generation
```c
int generate_default_config(const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) return -1;

    fprintf(f, "# Conquer Configuration (Generated)\n");
    fprintf(f, "[admin]\n");
    fprintf(f, "login = \"%s\"\n", getenv("USER") ? getenv("USER") : "admin");
    // ... write all defaults

    fclose(f);
    return 0;
}
```

#### 2.2 Migration Validation
```c
int validate_configuration(struct conquer_config* config) {
    // Validate admin login is not empty
    if (strlen(config->admin_login) == 0) {
        return CONFIG_ERROR_INVALID_ADMIN;
    }

    // Validate paths exist and are accessible
    if (access(config->data_directory, R_OK | W_OK) != 0) {
        return CONFIG_ERROR_INVALID_PATH;
    }

    // Validate numeric ranges
    if (config->max_nations < 1 || config->max_nations > 1000) {
        return CONFIG_ERROR_INVALID_RANGE;
    }

    return CONFIG_SUCCESS;
}
```

### 3. Testing Strategy

#### 3.1 Configuration Testing
- **Default configuration generation** and loading
- **Environment variable override** functionality
- **Configuration file validation** and error handling
- **Backward compatibility** with existing setups

#### 3.2 Migration Testing
- **Existing save game** compatibility
- **Performance impact** of configuration loading
- **Security improvement** verification
- **Cross-platform** functionality

---

## Security Improvements

### 1. Credential Management

#### 1.1 Remove Hardcoded Credentials
```c
// REMOVE: #define LOGIN "ssmoogen"
// REPLACE WITH: Configuration file or environment variable
const char* admin_user = get_config_string("admin", "login", getenv("USER"));
```

#### 1.2 Secure Configuration Storage
```bash
# Protect configuration files
chmod 600 ~/.conquer/config
chown gameuser:gamegroup /etc/conquer/conquer.conf
```

### 2. External Command Elimination

#### 2.1 File Operations
```c
// REPLACE: system("/bin/rm -f filename");
// WITH: unlink(filename);

// REPLACE: system("compress filename");
// WITH: compress_file_internal(filename);
```

#### 2.2 Input Validation
```c
int validate_filename(const char* filename) {
    // Check for path traversal
    if (strstr(filename, "..")) return 0;

    // Check for shell metacharacters
    if (strpbrk(filename, ";&|`$")) return 0;

    return 1;
}
```

---

## Implementation Checklist

### 1. Configuration Infrastructure
- [ ] Design configuration file schema
- [ ] Implement INI-style config parser
- [ ] Create configuration validation system
- [ ] Add environment variable support
- [ ] Generate default configuration file

### 2. Security Extraction
- [ ] Remove LOGIN hardcoded credential
- [ ] Externalize file permission settings
- [ ] Replace external command execution
- [ ] Implement secure path handling
- [ ] Add configuration file protection

### 3. Game Parameter Extraction
- [ ] Move economic constants to config
- [ ] Extract combat parameters
- [ ] Externalize population limits
- [ ] Move visibility ranges to config
- [ ] Create difficulty presets

### 4. Platform Modernization
- [ ] Replace platform #ifdef with CMake detection
- [ ] Generate config.h from feature detection
- [ ] Remove VMS/AIX370/obsolete platform support
- [ ] Standardize on POSIX functions

### 5. Migration Tools
- [ ] Create configuration migration utility
- [ ] Add configuration validation tool
- [ ] Implement default config generator
- [ ] Create deployment documentation

---

## Conclusion

The extraction of environmental and user-choice logic from the Conquer source code represents a significant modernization opportunity. By moving hardcoded values to external configuration, the project gains:

**Security Benefits**:
- Elimination of hardcoded administrative credentials
- Removal of external command execution vulnerabilities
- Improved configuration file protection

**Usability Benefits**:
- Runtime configuration without recompilation
- Environment-specific customization
- Easier game balance adjustments
- Simplified deployment across different systems

**Maintenance Benefits**:
- Reduced source code complexity
- Cleaner separation of concerns
- Improved testability
- Better documentation of configurable options

**Implementation Priority**:
1. **Phase 7b**: Security-critical extractions (admin credentials, external commands)
2. **Phase 8**: Game balance and user preference extraction
3. **Future**: Advanced configuration management and validation

The extraction strategy preserves backward compatibility while providing a clear path toward modern configuration management practices. The existing CMake infrastructure supports this modernization through automated feature detection and configuration generation.
/*
 * patchlevel.h - Software Version and Patch Level Tracking
 *
 * Simple version control header defining the current patch level of the
 * Conquer source code. This file provides a centralized location for
 * version tracking and build identification throughout the codebase.
 *
 * Key Components:
 *   - PATCHLEVEL: Primary version identifier constant
 *   - Version tracking for development and release management
 *   - Build system integration for version stamping
 *
 * Integration Notes:
 *   - Used by: Build system, version display, debugging output
 *   - Depends on: None (standalone header)
 *   - Provides: Single version constant for entire codebase
 *
 * Modernization Considerations:
 *   - Consider semantic versioning (MAJOR.MINOR.PATCH format)
 *   - Add build timestamp and revision control integration
 *   - Include feature capability flags for conditional compilation
 *   - Add version comparison macros for compatibility checking
 *
 * Original Author: Adam Bryant adb@cs.bu.edu
 * Usage Warning: Changing this number without proper coordination
 *                is not recommended as it affects version identification
 */

/* ============================================================================
 * VERSION IDENTIFICATION CONSTANTS
 * ============================================================================
 * Purpose: Centralized version tracking for the entire Conquer codebase
 * Usage: Version display, compatibility checking, debugging identification
 * Notes: Single source of truth for current software patch level
 */

/*
 * PATCHLEVEL - Current software patch level and version identifier
 *
 * Defines the current patch level of the Conquer source code, serving as
 * the primary version identifier throughout the codebase. This value is
 * used for version display, compatibility checking, and development tracking.
 *
 * Value: 28 (current patch level)
 * Units: Incremental patch number (integer sequence)
 * Usage: Version strings, compatibility checks, debug output, build identification
 * Notes: Should be incremented for each significant code change or release
 *        Changing without coordination affects version identification system
 *        Consider migration to semantic versioning for better release management
 *
 * Integration Points:
 *   - Build system: Version stamping in compiled binaries
 *   - UI system: Version display in about dialogs or startup messages
 *   - Debugging: Version identification in error reports and logs
 *   - Compatibility: Save file version checking and data migration
 *
 * Modernization Opportunities:
 *   - Expand to semantic versioning (MAJOR.MINOR.PATCH)
 *   - Add build metadata (timestamp, commit hash, branch name)
 *   - Include feature flags for conditional compilation
 *   - Add version comparison utilities and compatibility matrices
 */
#define PATCHLEVEL	28

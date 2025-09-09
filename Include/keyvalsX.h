/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/*
 * keyvalsX.h - Keyboard Input and Key Binding Definitions
 *
 * This header defines comprehensive keyboard input handling constants for
 * the game's user interface system. It provides standardized definitions
 * for ASCII control characters, special keys, and terminal escape sequences
 * used throughout the interface for key binding and input processing.
 *
 * The system supports three main categories of keyboard input:
 * 1. ASCII Control Characters: Standard control key combinations (Ctrl+A, etc.)
 * 2. Special Characters: Escape, delete, null, and other special keys
 * 3. Terminal Sequences: ANSI escape sequences for arrow keys and function keys
 *
 * Technical Features:
 * - Portable ASCII control character definitions
 * - Cross-terminal compatibility for arrow key sequences
 * - Both character and string representations for flexible usage
 * - Support for multiple terminal types and keyboard layouts
 *
 * Integration Notes:
 * - Used by input handling system for command parsing
 * - Integrates with key binding configuration system
 * - Supports both immediate key response and string-based bindings
 * - Compatible with various terminal emulators and console types
 *
 * Portability Considerations:
 * - Uses standard ASCII values for maximum compatibility
 * - Includes multiple arrow key sequence variants for different terminals
 * - Designed for both text-mode and graphical interface compatibility
 * - Supports legacy and modern terminal types
 */

/* This include file handles the list of key constants */

/*
 * Special Character Constants
 *
 * These constants define special characters and control codes that have
 * particular significance in terminal input processing and user interface
 * operations. They handle edge cases and special input conditions.
 */

/*
 * EXT_IGN - Ignore Character
 *
 * Character code used to indicate input that should be ignored.
 * Typically used for invalid or unrecognized input sequences.
 * Helps filter out unwanted or erroneous keyboard input.
 *
 * Value: '\377' (ASCII 255, 0xFF)
 * Usage: Input filtering, error handling, invalid input detection
 */
#define EXT_IGN	'\377'

/*
 * EXT_ESC - Escape Character
 *
 * Standard ASCII escape character used as prefix for escape sequences.
 * Forms the basis for arrow keys, function keys, and other special sequences.
 * Fundamental to ANSI terminal control and special key detection.
 *
 * Value: '\033' (ASCII 27, 0x1B)
 * Usage: Escape sequence detection, special key parsing, terminal control
 */
#define EXT_ESC	'\033'

/*
 * EXT_NUL - Null Character
 *
 * Standard ASCII null terminator character.
 * Used for string termination and empty input detection.
 * Important for safe string handling and input validation.
 *
 * Value: '\000' (ASCII 0, 0x00)
 * Usage: String termination, empty input detection, buffer initialization
 */
#define EXT_NUL	'\000'

/*
 * EXT_DEL - Delete Character
 *
 * ASCII delete character for backspace and character deletion operations.
 * Used for text editing and input correction functionality.
 * Standard character for deletion operations in text interfaces.
 *
 * Value: '\177' (ASCII 127, 0x7F)
 * Usage: Text editing, input correction, character deletion
 */
#define EXT_DEL	'\177'
/*
 * ASCII Control Character Constants
 *
 * These constants define standard ASCII control characters (Ctrl+letter
 * combinations) used for keyboard shortcuts and command input in the
 * game interface. Each constant represents the ASCII value generated
 * when the Control key is held while pressing the corresponding letter.
 *
 * Control Character Formula: Ctrl+letter = ASCII letter value - 64
 * Example: Ctrl+A = ASCII 'A' (65) - 64 = 1 (0x01)
 */

/*
 * CNTRL_A - Control+A (Start of Heading)
 * ASCII value 1 (0x01) - commonly used for "beginning" or "all" operations
 */
#define CNTRL_A '\001'

/*
 * CNTRL_B - Control+B (Start of Text)
 * ASCII value 2 (0x02) - commonly used for "back" or "backward" operations
 */
#define CNTRL_B '\002'

/*
 * CNTRL_C - Control+C (End of Text)
 * ASCII value 3 (0x03) - traditionally used for program interruption/cancel
 */
#define CNTRL_C '\003'

/*
 * CNTRL_D - Control+D (End of Transmission)
 * ASCII value 4 (0x04) - commonly used for "delete" or "end" operations
 */
#define CNTRL_D '\004'

/*
 * CNTRL_F - Control+F (Acknowledge)
 * ASCII value 6 (0x06) - commonly used for "find" or "forward" operations
 */
#define CNTRL_F '\006'

/*
 * CNTRL_G - Control+G (Bell)
 * ASCII value 7 (0x07) - traditionally triggers audible bell/alert
 */
#define CNTRL_G '\007'

/*
 * CNTRL_H - Control+H (Backspace)
 * ASCII value 8 (0x08) - standard backspace character for text editing
 */
#define CNTRL_H '\010'

/*
 * CNTRL_J - Control+J (Line Feed)
 * ASCII value 10 (0x0A) - line feed character, often equivalent to Enter
 */
#define CNTRL_J '\012'

/*
 * CNTRL_K - Control+K (Vertical Tab)
 * ASCII value 11 (0x0B) - commonly used for "kill line" operations
 */
#define CNTRL_K '\013'

/*
 * CNTRL_L - Control+L (Form Feed)
 * ASCII value 12 (0x0C) - commonly used for screen refresh/clear operations
 */
#define CNTRL_L '\014'

/*
 * CNTRL_N - Control+N (Shift Out)
 * ASCII value 14 (0x0E) - commonly used for "next" or "new" operations
 */
#define CNTRL_N '\016'

/*
 * CNTRL_O - Control+O (Shift In)
 * ASCII value 15 (0x0F) - commonly used for "open" operations
 */
#define CNTRL_O '\017'

/*
 * CNTRL_R - Control+R (Device Control 2)
 * ASCII value 18 (0x12) - commonly used for "refresh" or "reverse" operations
 */
#define CNTRL_R '\022'

/*
 * CNTRL_U - Control+U (Negative Acknowledge)
 * ASCII value 21 (0x15) - commonly used for "undo" or "kill line backward"
 */
#define CNTRL_U '\025'

/*
 * CNTRL_V - Control+V (Synchronous Idle)
 * ASCII value 22 (0x16) - commonly used for "paste" or literal input
 */
#define CNTRL_V '\026'

/*
 * CNTRL_X - Control+X (Cancel)
 * ASCII value 24 (0x18) - commonly used for "cut" or "exit" operations
 */
#define CNTRL_X '\030'

/*
 * CNTRL_Y - Control+Y (End of Medium)
 * ASCII value 25 (0x19) - commonly used for "yank" (paste) operations
 */
#define CNTRL_Y '\031'

/*
 * CNTRL_Z - Control+Z (Substitute)
 * ASCII value 26 (0x1A) - traditionally used for process suspension
 */
#define CNTRL_Z '\032'

/*
 * String Representations for Key Binding System
 *
 * These constants provide string representations of the above character
 * constants for use in key binding configuration and string-based input
 * processing. They allow the same key codes to be used in contexts that
 * require string literals rather than character constants.
 *
 * Usage: Configuration files, key binding tables, string parsing, and
 * contexts where character arrays are preferred over individual characters.
 */

/*
 * Special Character String Constants
 * String equivalents of the EXT_* character constants above.
 */

/*
 * ESTR_IGN - Ignore Character String
 * String representation of EXT_IGN for string-based processing
 */
#define ESTR_IGN "\377"

/*
 * ESTR_ESC - Escape Character String  
 * String representation of EXT_ESC for escape sequence processing
 */
#define ESTR_ESC "\033"

/*
 * ESTR_NUL - Null Character String
 * String representation of EXT_NUL (empty string for null character)
 */
#define ESTR_NUL ""

/*
 * ESTR_DEL - Delete Character String
 * String representation of EXT_DEL for string-based deletion handling
 */
#define ESTR_DEL "\177"

/*
 * Control Character String Constants
 * String equivalents of the CNTRL_* character constants above.
 * These allow control characters to be used in string contexts.
 */

/*
 * CSTR_A through CSTR_Z - Control Character Strings
 * String representations of control characters for key binding systems.
 * Each string contains the single control character as its only element.
 */

/* Control+A string for beginning/all operations */
#define CSTR_A "\001"

/* Control+B string for back/backward operations */
#define CSTR_B "\002"

/* Control+C string for cancel/interrupt operations */
#define CSTR_C "\003"

/* Control+D string for delete/end operations */
#define CSTR_D "\004"

/* Control+E string (not defined as character constant above) */
#define CSTR_E "\005"

/* Control+F string for find/forward operations */
#define CSTR_F "\006"

/* Control+G string for bell/alert operations */
#define CSTR_G "\007"

/* Control+H string for backspace operations */
#define CSTR_H "\010"

/* Control+J string for line feed/enter operations */
#define CSTR_J "\012"

/* Control+K string for kill line operations */
#define CSTR_K "\013"

/* Control+L string for screen refresh/clear operations */
#define CSTR_L "\014"

/* Control+N string for next/new operations */
#define CSTR_N "\016"

/* Control+O string for open operations */
#define CSTR_O "\017"

/* Control+P string (not defined as character constant above) */
#define CSTR_P "\020"

/* Control+R string for refresh/reverse operations */
#define CSTR_R "\022"

/* Control+T string (not defined as character constant above) */
#define CSTR_T "\024"

/* Control+U string for undo/kill line backward operations */
#define CSTR_U "\025"

/* Control+V string for paste/literal input operations */
#define CSTR_V "\026"

/* Control+W string (not defined as character constant above) */
#define CSTR_W "\027"

/* Control+X string for cut/exit operations */
#define CSTR_X "\030"

/* Control+Y string for yank/paste operations */
#define CSTR_Y "\031"

/* Control+Z string for process suspension operations */
#define CSTR_Z "\032"

/*
 * Arrow Key Escape Sequence Constants
 *
 * These constants define ANSI escape sequences for arrow key detection
 * across different terminal types and configurations. The game supports
 * multiple arrow key sequence variants to ensure compatibility with
 * various terminal emulators and keyboard configurations.
 *
 * Sequence Format: ESC + [ or O + Direction Letter
 * - ESC[x sequences: Standard ANSI sequences (most common)
 * - ESCOx sequences: Alternative sequences used by some terminals
 *
 * Technical Notes:
 * - All sequences start with ESC (ASCII 27, \033)
 * - Two sequence types accommodate different terminal modes
 * - Direction letters: A=Up, B=Down, C=Right, D=Left
 * - Both variants should be checked for maximum compatibility
 */

/*
 * Primary Arrow Key Sequences (ESC[x format)
 * Standard ANSI escape sequences used by most modern terminals.
 */

/*
 * AKEY_UP1 - Primary Up Arrow Sequence
 * Standard ANSI sequence for up arrow key press.
 * Most commonly used sequence across terminal types.
 *
 * Value: "\033[A" (ESC + [ + A)
 * Usage: Up arrow detection, vertical navigation upward
 */
#define AKEY_UP1	"\033[A"

/*
 * AKEY_DOWN1 - Primary Down Arrow Sequence
 * Standard ANSI sequence for down arrow key press.
 * Most commonly used sequence across terminal types.
 *
 * Value: "\033[B" (ESC + [ + B)
 * Usage: Down arrow detection, vertical navigation downward
 */
#define AKEY_DOWN1	"\033[B"

/*
 * AKEY_RIGHT1 - Primary Right Arrow Sequence
 * Standard ANSI sequence for right arrow key press.
 * Most commonly used sequence across terminal types.
 *
 * Value: "\033[C" (ESC + [ + C)
 * Usage: Right arrow detection, horizontal navigation rightward
 */
#define AKEY_RIGHT1	"\033[C"

/*
 * AKEY_LEFT1 - Primary Left Arrow Sequence
 * Standard ANSI sequence for left arrow key press.
 * Most commonly used sequence across terminal types.
 *
 * Value: "\033[D" (ESC + [ + D)
 * Usage: Left arrow detection, horizontal navigation leftward
 */
#define AKEY_LEFT1	"\033[D"

/*
 * Alternative Arrow Key Sequences (ESCOx format)
 * Alternative sequences used by some terminal configurations.
 * Provides compatibility with terminals that use different sequences.
 */

/*
 * AKEY_UP2 - Alternative Up Arrow Sequence
 * Alternative sequence for up arrow key press.
 * Used by some terminal types and keyboard configurations.
 *
 * Value: "\033OA" (ESC + O + A)
 * Usage: Up arrow detection (alternative), backup sequence detection
 */
#define AKEY_UP2	"\033OA"

/*
 * AKEY_DOWN2 - Alternative Down Arrow Sequence
 * Alternative sequence for down arrow key press.
 * Used by some terminal types and keyboard configurations.
 *
 * Value: "\033OB" (ESC + O + B)
 * Usage: Down arrow detection (alternative), backup sequence detection
 */
#define AKEY_DOWN2	"\033OB"

/*
 * AKEY_RIGHT2 - Alternative Right Arrow Sequence
 * Alternative sequence for right arrow key press.
 * Used by some terminal types and keyboard configurations.
 *
 * Value: "\033OC" (ESC + O + C)
 * Usage: Right arrow detection (alternative), backup sequence detection
 */
#define AKEY_RIGHT2	"\033OC"

/*
 * AKEY_LEFT2 - Alternative Left Arrow Sequence
 * Alternative sequence for left arrow key press.
 * Used by some terminal types and keyboard configurations.
 *
 * Value: "\033OD" (ESC + O + D)
 * Usage: Left arrow detection (alternative), backup sequence detection
 */
#define AKEY_LEFT2	"\033OD"

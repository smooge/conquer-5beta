/*
 * paramX.h - Function Prototype Compatibility Framework
 *
 * This header provides a comprehensive macro-based system for handling function
 * prototype declarations in a way that is compatible with both ANSI C (C89/C90)
 * and pre-ANSI K&R C compilers. It enables the codebase to maintain portability
 * across different compiler environments while providing proper type checking
 * in modern compilers.
 *
 * The parameter macro system bridges the gap between:
 * - Modern ANSI C function prototypes with full type information
 * - Legacy K&R C function declarations with separate parameter lists
 * - Consistent function interface documentation across the codebase
 * - Cross-compiler compatibility for diverse development environments
 *
 * Key Components:
 *   - Compiler detection macros for ANSI C vs K&R C environments
 *   - Parameter count-specific macros (PARM_0 through PARM_5)
 *   - Automatic prototype generation for both compiler types
 *   - Include guard protection against multiple inclusions
 *   - Support for up to 5-parameter function declarations
 *
 * Integration Notes:
 *   - Used by: All header files declaring functions throughout the codebase
 *   - Depends on: Compiler-provided __STDC__ macro for standard detection
 *   - Provides: Cross-compiler function prototype compatibility macros
 *
 * Modernization Considerations:
 *   - Legacy compatibility system - consider removal for C2023 modernization
 *   - Modern compilers universally support ANSI C prototypes
 *   - Consider migration to standard function declarations
 *   - Preserve as documentation of historical compatibility requirements
 */

/* conquer : Copyright (c) 1992 by Ed Barlow and Adam Bryant
 *
 * Please see the copyright notice located in the header.h file.
 */

/* ============================================================================
 * COMPATIBILITY FRAMEWORK - Cross-Compiler Function Prototype Support
 * ============================================================================
 * Purpose: Provide unified function declaration syntax for ANSI and K&R C
 * Usage: Include guard and macro definition framework for prototype compatibility
 * Notes: Enables consistent function interfaces across different compiler types
 */

/*
 * Include Guard Protection - Prevents multiple inclusion
 *
 * Standard include guard using PARM_0 as the sentinel macro to prevent
 * multiple definitions of the parameter macros. This ensures that the
 * compatibility framework is only defined once per compilation unit.
 *
 * Protection Strategy:
 *   - Uses PARM_0 macro existence as inclusion indicator
 *   - Prevents redefinition conflicts in complex inclusion hierarchies
 *   - Maintains consistent macro behavior across all source files
 *   - Essential for large codebases with extensive header dependencies
 */
#ifndef PARM_0

/* ============================================================================
 * COMPILER DETECTION AND ANSI C SUPPORT
 * ============================================================================
 * Purpose: Detect ANSI C compiler support and configure appropriate macros
 * Usage: Conditional compilation based on __STDC__ compiler macro
 * Notes: Enables different prototype strategies for ANSI vs K&R compilers
 */

/*
 * ANSI C Compiler Support (__STDC__ defined)
 *
 * When __STDC__ is defined, the compiler supports ANSI C function prototypes
 * with full parameter type information. The PARM_X macros generate standard
 * ANSI C function prototype syntax with complete type checking.
 *
 * ANSI C Prototype Format:
 *   return_type function_name(type1 param1, type2 param2, ...);
 *
 * Benefits:
 *   - Full compile-time type checking for function parameters
 *   - Standard C syntax compatible with all modern compilers
 *   - Enhanced error detection and parameter validation
 *   - Improved code documentation through explicit type information
 */
#ifdef __STDC__

/*
 * PARM_0 - Zero-parameter function prototype macro (ANSI C)
 *
 * Generates ANSI C prototype for functions with no parameters. The macro
 * takes a single argument (the void keyword) and produces a proper ANSI C
 * function prototype declaration with explicit void parameter list.
 *
 * Usage: extern return_type function_name PARM_0(void);
 * Expands to: extern return_type function_name (void);
 * 
 * Parameters:
 *   a - Should be 'void' for zero-parameter functions
 * 
 * ANSI C Benefit: Explicit void parameter list prevents variadic interpretation
 */
#define PARM_0(a) (a)

/*
 * PARM_1 - Single-parameter function prototype macro (ANSI C)
 *
 * Generates ANSI C prototype for functions with one parameter. Creates
 * standard function prototype with full type information for compile-time
 * type checking and parameter validation.
 *
 * Usage: extern return_type function_name PARM_1(type, name);
 * Expands to: extern return_type function_name (type name);
 * 
 * Parameters:
 *   a - Parameter type (e.g., int, char*, struct*)
 *   b - Parameter name for documentation
 * 
 * ANSI C Benefit: Full type checking and parameter name documentation
 */
#define PARM_1(a,b) (a b)

/*
 * PARM_2 - Two-parameter function prototype macro (ANSI C)
 *
 * Generates ANSI C prototype for functions with two parameters. Provides
 * complete type information for both parameters with proper comma separation
 * and type checking support.
 *
 * Usage: extern return_type function_name PARM_2(type1, name1, type2, name2);
 * Expands to: extern return_type function_name (type1 name1, type2 name2);
 * 
 * Parameters:
 *   a - First parameter type
 *   b - First parameter name
 *   c - Second parameter type  
 *   d - Second parameter name
 * 
 * ANSI C Benefit: Multi-parameter type checking with clear documentation
 */
#define PARM_2(a,b,c,d) (a b, c d)

/*
 * PARM_3 - Three-parameter function prototype macro (ANSI C)
 *
 * Generates ANSI C prototype for functions with three parameters. Maintains
 * proper comma separation and provides complete type information for all
 * parameters with comprehensive compile-time validation.
 *
 * Usage: extern return_type function_name PARM_3(type1, name1, type2, name2, type3, name3);
 * Expands to: extern return_type function_name (type1 name1, type2 name2, type3 name3);
 * 
 * Parameters:
 *   a, c, e - Parameter types for positions 1, 2, 3
 *   b, d, f - Parameter names for positions 1, 2, 3
 * 
 * ANSI C Benefit: Full three-parameter type checking and documentation
 */
#define PARM_3(a,b,c,d,e,f) (a b, c d, e f)

/*
 * PARM_4 - Four-parameter function prototype macro (ANSI C)
 *
 * Generates ANSI C prototype for functions with four parameters. Provides
 * complete type checking and documentation for complex function interfaces
 * with multiple parameters and varied types.
 *
 * Usage: extern return_type function_name PARM_4(type1, name1, type2, name2, type3, name3, type4, name4);
 * Expands to: extern return_type function_name (type1 name1, type2 name2, type3 name3, type4 name4);
 * 
 * Parameters:
 *   a, c, e, g - Parameter types for positions 1, 2, 3, 4
 *   b, d, f, h - Parameter names for positions 1, 2, 3, 4
 * 
 * ANSI C Benefit: Complex function interface with full type validation
 */
#define PARM_4(a,b,c,d,e,f,g,h) (a b, c d, e f, g h)

/*
 * PARM_5 - Five-parameter function prototype macro (ANSI C)
 *
 * Generates ANSI C prototype for functions with five parameters. Supports
 * the most complex function interfaces in the codebase while maintaining
 * full type checking and comprehensive parameter documentation.
 *
 * Usage: extern return_type function_name PARM_5(type1, name1, type2, name2, type3, name3, type4, name4, type5, name5);
 * Expands to: extern return_type function_name (type1 name1, type2 name2, type3 name3, type4 name4, type5 name5);
 * 
 * Parameters:
 *   a, c, e, g, i - Parameter types for positions 1, 2, 3, 4, 5
 *   b, d, f, h, j - Parameter names for positions 1, 2, 3, 4, 5
 * 
 * ANSI C Benefit: Maximum function complexity with complete type checking
 */
#define PARM_5(a,b,c,d,e,f,g,h,i,j) (a b, c d, e f, g h, i j)

/* ============================================================================
 * K&R C COMPILER SUPPORT (Pre-ANSI Legacy Compatibility)
 * ============================================================================
 * Purpose: Support legacy K&R C compilers without ANSI prototype support
 * Usage: Alternative macro definitions for K&R function declaration syntax
 * Notes: Maintains compatibility with pre-1989 C compilers and legacy systems
 */

#else

/*
 * K&R C Compiler Support (__STDC__ not defined)
 *
 * When __STDC__ is not defined, the compiler uses K&R C function declaration
 * syntax where parameter types are declared separately from the parameter list.
 * The PARM_X macros generate K&R-compatible function declarations.
 *
 * K&R C Declaration Format:
 *   return_type function_name(param1, param2, ...)
 *   type1 param1;
 *   type2 param2;
 *   ...
 *
 * Limitations:
 *   - No compile-time parameter type checking
 *   - Separate parameter type declarations required in implementation
 *   - Reduced error detection capabilities
 *   - Legacy compatibility for historical systems only
 */

/*
 * PARM_0 - Zero-parameter function declaration (K&R C)
 *
 * Generates K&R C declaration for functions with no parameters. Produces
 * empty parameter list which is interpreted as unspecified parameters
 * in K&R C (different from modern void parameter lists).
 *
 * Usage: extern return_type function_name PARM_0(void);
 * Expands to: extern return_type function_name ();
 * 
 * K&R C Behavior: Empty parameter list allows any number of arguments
 * Note: Different semantics from ANSI C void parameter list
 */
#define PARM_0(a) ()

/*
 * PARM_1 - Single-parameter function declaration (K&R C)
 *
 * Generates K&R C declaration for functions with one parameter. Creates
 * parameter list with name only, requiring separate type declaration
 * in function implementation.
 *
 * Usage: extern return_type function_name PARM_1(type, name);
 * Expands to: extern return_type function_name (name) type name;
 * 
 * K&R C Format: Parameter name in list, type declared separately
 * Implementation Requirement: Function definition must include separate type declaration
 */
#define PARM_1(a,b) (b) a b;

/*
 * PARM_2 - Two-parameter function declaration (K&R C)
 *
 * Generates K&R C declaration for functions with two parameters. Creates
 * comma-separated parameter name list with separate type declarations
 * for each parameter following K&R C syntax requirements.
 *
 * Usage: extern return_type function_name PARM_2(type1, name1, type2, name2);
 * Expands to: extern return_type function_name (name1, name2) type1 name1; type2 name2;
 * 
 * K&R C Format: Parameter names in list, types declared separately
 * Implementation Requirement: Function definition must include all type declarations
 */
#define PARM_2(a,b,c,d) (b, d) a b; c d;

/*
 * PARM_3 - Three-parameter function declaration (K&R C)
 *
 * Generates K&R C declaration for functions with three parameters. Maintains
 * proper comma separation in parameter list and provides separate type
 * declarations for all three parameters.
 *
 * Usage: extern return_type function_name PARM_3(type1, name1, type2, name2, type3, name3);
 * Expands to: extern return_type function_name (name1, name2, name3) type1 name1; type2 name2; type3 name3;
 * 
 * K&R C Format: Three parameter names, three separate type declarations
 * Implementation Requirement: All parameter types must be declared in function definition
 */
#define PARM_3(a,b,c,d,e,f) (b, d, f) a b; c d; e f;

/*
 * PARM_4 - Four-parameter function declaration (K&R C)
 *
 * Generates K&R C declaration for functions with four parameters. Provides
 * complete parameter name list and separate type declarations for complex
 * function interfaces in K&R C compatible format.
 *
 * Usage: extern return_type function_name PARM_4(type1, name1, type2, name2, type3, name3, type4, name4);
 * Expands to: extern return_type function_name (name1, name2, name3, name4) type1 name1; type2 name2; type3 name3; type4 name4;
 * 
 * K&R C Format: Four parameter names, four separate type declarations
 * Implementation Requirement: Complex parameter type declaration management
 */
#define PARM_4(a,b,c,d,e,f,g,h) (b, d, f, h) a b; c d; e f; g h;

/*
 * PARM_5 - Five-parameter function declaration (K&R C)
 *
 * Generates K&R C declaration for functions with five parameters. Supports
 * the most complex function interfaces while maintaining K&R C compatibility
 * and proper parameter type declaration syntax.
 *
 * Usage: extern return_type function_name PARM_5(type1, name1, type2, name2, type3, name3, type4, name4, type5, name5);
 * Expands to: extern return_type function_name (name1, name2, name3, name4, name5) type1 name1; type2 name2; type3 name3; type4 name4; type5 name5;
 * 
 * K&R C Format: Five parameter names, five separate type declarations
 * Implementation Requirement: Maximum complexity parameter type management
 */
#define PARM_5(a,b,c,d,e,f,g,h,i,j) (b, d, f, h, j) a b; c d; e f; g h; i j;

#endif /*__STDC__ - End of compiler-specific macro definitions */

#endif /* PARM_0 - End of include guard protection */

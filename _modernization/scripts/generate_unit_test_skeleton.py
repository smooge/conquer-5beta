#!/usr/bin/env -S uv run --script
# /// script
# dependencies = ["pathlib", "argparse", "re"]
# ///

"""
Generate Unit Test Skeleton for C Source Files

This script analyzes C source files and generates Unity test file skeletons
with test functions for each function found in the source file.

Usage:
    python3 generate_unit_test_skeleton.py Src/moveA.c
    python3 generate_unit_test_skeleton.py Src/combatA.c --output tests/unit/test_combatA.c
    python3 generate_unit_test_skeleton.py Src/*.c --batch
"""

import argparse
import re
import sys
from pathlib import Path
from datetime import datetime

# Import the working function analysis from analyze_functions.py
sys.path.append(str(Path(__file__).parent))
from analyze_functions import analyze_source_file

def extract_functions(source_file):
    """
    Extract function definitions from C source file using the robust analyze_functions module.

    Returns list of dictionaries with function information:
    [{'name': 'function_name', 'return_type': 'int', 'params': 'int x, char *y', 'line': 42}]
    """
    # Use the robust function analysis that handles PARM_ patterns
    analysis_result = analyze_source_file(source_file)

    if 'error' in analysis_result:
        print(f"Error analyzing {source_file}: {analysis_result['error']}")
        return []

    functions = []
    for func_info in analysis_result['functions']:
        # Convert parameters to a simplified string format for test generation
        if func_info['parameters']:
            params = ', '.join([param['full'] for param in func_info['parameters']])
        else:
            params = "void"

        functions.append({
            'name': func_info['name'],
            'return_type': func_info['return_type'],
            'params': params,
            'line': func_info['line_number'],
            'is_parm_function': func_info.get('is_parm_function', False),
            'parameter_count': func_info['parameter_count'],
            'complexity': func_info['complexity']['category']
        })

    return functions

def get_required_includes(source_file):
    """
    Determine which header files are needed for the test file.
    """
    includes = ['#include "unity.h"']

    # Standard project includes
    includes.append('#include "../Include/buildA.h"')

    # Try to find specific includes for this source file
    try:
        with open(source_file, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()

        # Extract include statements
        include_pattern = r'#include\s+[<"]([^>"]+)[>"]'
        for match in re.finditer(include_pattern, content):
            header = match.group(1)
            if not header.startswith('std') and not header in ['stdio.h', 'stdlib.h', 'string.h']:
                if header.startswith('../'):
                    includes.append(f'#include "{header}"')
                else:
                    includes.append(f'#include "../Include/{header}"')
    except Exception:
        pass  # Use defaults if file reading fails

    return includes

def generate_test_skeleton(source_file, output_file=None, dry_run=False):
    """
    Generate a Unity test file skeleton for the given source file.
    """
    source_path = Path(source_file)
    if not source_path.exists():
        print(f"Error: Source file {source_file} not found")
        return False

    # Determine output file path
    if output_file is None:
        test_filename = f"test_{source_path.stem}.c"
        output_path = Path("tests/unit") / test_filename
    else:
        output_path = Path(output_file)

    # Create output directory if it doesn't exist
    if not dry_run:
        output_path.parent.mkdir(parents=True, exist_ok=True)

    # Extract functions from source file
    functions = extract_functions(source_file)
    if not functions:
        print(f"Warning: No functions found in {source_file}")

    # Get required includes
    includes = get_required_includes(source_file)

    # Generate test file content
    content = generate_test_content(source_path, functions, includes)

    if dry_run:
        print(f"Would generate: {output_path}")
        print(f"Functions found: {len(functions)}")
        for func in functions:
            print(f"  - {func['name']}() [line {func['line']}]")
        return True

    # Write test file
    try:
        with open(output_path, 'w') as f:
            f.write(content)
        print(f"Generated: {output_path}")
        print(f"Functions: {len(functions)}")
        return True
    except Exception as e:
        print(f"Error writing {output_path}: {e}")
        return False

def generate_test_content(source_path, functions, includes):
    """
    Generate the actual content of the test file.
    """
    today = datetime.now().strftime("%Y-%m-%d")

    content = f"""/*
 * Unit Tests for {source_path}
 * Generated: {today}
 * Functions tested: {len(functions)}
 *
 * This file contains unit tests for all functions in {source_path.name}.
 * Each function should have tests for:
 * - Basic functionality
 * - Edge cases
 * - Error conditions
 * - Boundary values
 */

{chr(10).join(includes)}

/*
 * Test Setup and Teardown
 */

void setUp(void) {{
    /* Set up test fixtures, if any */
    /* This is run before every test */
}}

void tearDown(void) {{
    /* Clean up after each test */
    /* This is run after every test */
}}

/*
 * Helper Functions for Testing
 */

/* Add any helper functions needed for testing here */

/*
 * Unit Tests
 */

"""

    # Generate test functions for each source function
    for func in functions:
        content += generate_function_tests(func)

    # Generate main function
    content += f"""
/*
 * Test Runner
 */
int main(void) {{
    UNITY_BEGIN();

    /* Run tests for each function */
"""

    for func in functions:
        func_name = func['name']
        content += f"""    RUN_TEST(test_{func_name}_basic);
    RUN_TEST(test_{func_name}_edge_cases);
    RUN_TEST(test_{func_name}_error_conditions);
"""

    content += """
    return UNITY_END();
}
"""

    return content

def generate_function_tests(func):
    """
    Generate test functions for a single source function.
    """
    func_name = func['name']
    return_type = func['return_type']
    params = func['params']

    content = f"""
/*
 * Tests for {func_name}()
 * Return type: {return_type}
 * Parameters: {params}
 */

void test_{func_name}_basic(void) {{
    /* Test basic functionality of {func_name}() */
    /* TODO: Implement basic test cases */

    /* Example test structure:
     * 1. Set up test data
     * 2. Call function with normal inputs
     * 3. Assert expected results
     */

    TEST_IGNORE_MESSAGE("TODO: Implement test_{func_name}_basic");
}}

void test_{func_name}_edge_cases(void) {{
    /* Test edge cases for {func_name}() */
    /* TODO: Test boundary values, special cases */

    /* Common edge cases:
     * - Minimum/maximum values
     * - Empty inputs
     * - Very large inputs
     * - Special values (0, -1, etc.)
     */

    TEST_IGNORE_MESSAGE("TODO: Implement test_{func_name}_edge_cases");
}}

void test_{func_name}_error_conditions(void) {{
    /* Test error handling in {func_name}() */
    /* TODO: Test invalid inputs, error conditions */

    /* Common error conditions:
     * - NULL pointers
     * - Invalid parameters
     * - Out of range values
     * - Resource exhaustion
     */

    TEST_IGNORE_MESSAGE("TODO: Implement test_{func_name}_error_conditions");
}}
"""

    return content

def main():
    parser = argparse.ArgumentParser(description='Generate Unity test skeletons for C source files')
    parser.add_argument('source_files', nargs='+', help='C source files to analyze')
    parser.add_argument('-o', '--output', help='Output test file path (for single file only)')
    parser.add_argument('--dry-run', action='store_true', help='Show what would be generated without creating files')
    parser.add_argument('--batch', action='store_true', help='Process multiple files with auto-generated names')

    args = parser.parse_args()

    if len(args.source_files) > 1 and args.output:
        print("Error: Cannot specify output file when processing multiple source files")
        return 1

    success_count = 0
    total_count = len(args.source_files)

    for source_file in args.source_files:
        # Handle glob patterns
        source_path = Path(source_file)
        if '*' in source_file:
            matching_files = list(source_path.parent.glob(source_path.name))
            for match_file in matching_files:
                if generate_test_skeleton(str(match_file), args.output, args.dry_run):
                    success_count += 1
        else:
            if generate_test_skeleton(source_file, args.output, args.dry_run):
                success_count += 1

    print(f"\\nSummary: {success_count}/{total_count} test files generated successfully")

    if not args.dry_run and success_count > 0:
        print("\\nNext steps:")
        print("1. Update CMakeLists.txt to include new test targets")
        print("2. Implement the TODO test cases")
        print("3. Run tests with: cd build && ctest")

    return 0 if success_count == total_count else 1

if __name__ == '__main__':
    sys.exit(main())
#!/usr/bin/env -S uv run --script
# /// script
# dependencies = ["pathlib", "argparse", "re"]
# ///

"""
Update CMakeLists.txt with New Unit Tests

This script automatically adds new unit test targets to the CMakeLists.txt file
and creates CTest entries for them.

Usage:
    python3 update_cmake_tests.py tests/unit/test_moveA.c
    python3 update_cmake_tests.py tests/unit/test_*.c --batch
    python3 update_cmake_tests.py --scan-all
"""

import argparse
import re
import sys
from pathlib import Path

def find_cmake_file():
    """Find the main CMakeLists.txt file."""
    cmake_paths = [
        Path("CMakeLists.txt"),
        Path("../CMakeLists.txt"),
        Path("../../CMakeLists.txt")
    ]

    for path in cmake_paths:
        if path.exists():
            return path

    return None

def extract_test_info(test_file):
    """
    Extract information from test file to generate CMake target.
    Returns dict with test name, source files, and dependencies.
    """
    test_path = Path(test_file)
    if not test_path.exists():
        return None

    # Derive test name from filename
    test_name = test_path.stem  # Remove .c extension

    # Try to determine source file being tested
    if test_name.startswith('test_'):
        source_name = test_name[5:]  # Remove 'test_' prefix
        source_file = f"Src/{source_name}.c"
    else:
        source_file = None

    # Read test file to find include dependencies
    includes = set()
    try:
        with open(test_file, 'r') as f:
            content = f.read()

        # Extract include statements
        include_pattern = r'#include\s+[<"]([^>"]+)[>"]'
        for match in re.finditer(include_pattern, content):
            header = match.group(1)
            includes.add(header)

    except Exception as e:
        print(f"Warning: Could not read {test_file}: {e}")

    return {
        'test_name': test_name,
        'test_file': str(test_file),
        'source_file': source_file,
        'includes': includes
    }

def generate_cmake_target(test_info):
    """Generate CMake target configuration for a test."""
    test_name = test_info['test_name']
    test_file = test_info['test_file']
    source_file = test_info['source_file']

    # Base CMake configuration
    cmake_text = f"""
# {test_name} - Unit test target
add_executable({test_name} {test_file}"""

    # Add source file if it exists
    if source_file and Path(source_file).exists():
        cmake_text += f" {source_file}"

    cmake_text += ")\n"

    # Add target properties
    cmake_text += f"""target_link_libraries({test_name} PRIVATE unity)
target_include_directories({test_name} PRIVATE Include)

# Add to CTest
add_test(NAME {test_name} COMMAND {test_name})
set_tests_properties({test_name} PROPERTIES
    WORKING_DIRECTORY ${{CMAKE_BINARY_DIR}}
    TIMEOUT 30
)
"""

    return cmake_text

def update_cmake_file(cmake_file, test_configs, dry_run=False):
    """
    Update CMakeLists.txt with new test configurations.
    """
    try:
        with open(cmake_file, 'r') as f:
            content = f.read()
    except Exception as e:
        print(f"Error reading {cmake_file}: {e}")
        return False

    # Find the testing section or create one
    testing_section_start = "# Unit Tests"
    testing_section_end = "# End Unit Tests"

    if testing_section_start in content:
        # Update existing section
        start_idx = content.find(testing_section_start)
        end_idx = content.find(testing_section_end)

        if end_idx == -1:
            print(f"Warning: Found test section start but no end marker in {cmake_file}")
            # Find a good place to insert end marker
            lines = content[start_idx:].split('\n')
            for i, line in enumerate(lines[1:], 1):  # Skip the start marker line
                if line.strip() == '' or line.startswith('#') and 'test' not in line.lower():
                    end_idx = start_idx + len('\n'.join(lines[:i]))
                    break
            else:
                end_idx = len(content)

        # Extract existing tests to avoid duplicates
        existing_section = content[start_idx:end_idx]
        existing_tests = set(re.findall(r'add_executable\(([^)]+)', existing_section))

        # Build new testing section
        new_section = testing_section_start + "\\n"
        new_section += "enable_testing()\\n\\n"

        # Add existing tests first (preserve order)
        new_section += existing_section[len(testing_section_start):].strip() + "\\n\\n"

        # Add new tests
        for config in test_configs:
            test_name = config['test_name']
            if test_name not in existing_tests:
                new_section += generate_cmake_target(config)

        new_section += "\\n" + testing_section_end

        # Replace the section
        new_content = content[:start_idx] + new_section + content[end_idx + len(testing_section_end):]

    else:
        # Add new testing section at the end
        testing_section = f"""
{testing_section_start}
enable_testing()

"""
        for config in test_configs:
            testing_section += generate_cmake_target(config)

        testing_section += f"""
{testing_section_end}
"""

        new_content = content.rstrip() + "\\n" + testing_section

    if dry_run:
        print(f"Would update {cmake_file} with {len(test_configs)} test configurations")
        return True

    # Write updated CMakeLists.txt
    try:
        with open(cmake_file, 'w') as f:
            f.write(new_content)
        print(f"Updated {cmake_file} with {len(test_configs)} test configurations")
        return True
    except Exception as e:
        print(f"Error writing {cmake_file}: {e}")
        return False

def scan_test_directory():
    """Scan for all test files in the tests directory."""
    test_files = []
    test_dirs = [Path("tests/unit"), Path("tests/integration"), Path("tests/regression")]

    for test_dir in test_dirs:
        if test_dir.exists():
            test_files.extend(test_dir.glob("test_*.c"))

    return test_files

def main():
    parser = argparse.ArgumentParser(description='Update CMakeLists.txt with unit test targets')
    parser.add_argument('test_files', nargs='*', help='Test files to add to CMakeLists.txt')
    parser.add_argument('--scan-all', action='store_true', help='Scan all test directories and add found tests')
    parser.add_argument('--dry-run', action='store_true', help='Show what would be changed without modifying files')
    parser.add_argument('--cmake-file', help='Path to CMakeLists.txt file (auto-detected if not specified)')

    args = parser.parse_args()

    # Find CMakeLists.txt
    if args.cmake_file:
        cmake_file = Path(args.cmake_file)
    else:
        cmake_file = find_cmake_file()

    if not cmake_file or not cmake_file.exists():
        print("Error: Could not find CMakeLists.txt file")
        return 1

    # Collect test files
    test_files = []
    if args.scan_all:
        test_files.extend(scan_test_directory())
    else:
        for pattern in args.test_files:
            if '*' in pattern:
                test_files.extend(Path('.').glob(pattern))
            else:
                test_files.append(Path(pattern))

    if not test_files:
        print("No test files specified or found")
        return 1

    # Extract test configurations
    test_configs = []
    for test_file in test_files:
        config = extract_test_info(test_file)
        if config:
            test_configs.append(config)
        else:
            print(f"Warning: Could not process {test_file}")

    if not test_configs:
        print("No valid test configurations found")
        return 1

    # Update CMakeLists.txt
    success = update_cmake_file(cmake_file, test_configs, args.dry_run)

    if success and not args.dry_run:
        print("\\nNext steps:")
        print("1. Run 'cmake ..' from the build directory to regenerate build files")
        print("2. Run 'make' to build the new test targets")
        print("3. Run 'ctest' to execute all tests")

    return 0 if success else 1

if __name__ == '__main__':
    sys.exit(main())
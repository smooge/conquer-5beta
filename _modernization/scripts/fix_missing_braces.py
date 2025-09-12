#!/usr/bin/env -S uv run --script
# /// script
dependencies = ["re", "argparse", "pathlib"]
# ///

"""
Fix Missing Braces Around Initializers in C Code

This script fixes -Wmissing-braces warnings by adding braces around array 
initializers within struct initializations. It handles common patterns like:

1. DISPLAY_STRUCT with style[4], highlight[4], target[4] arrays
2. Multi-dimensional array initializations
3. Struct arrays with embedded arrays

Usage:
    python3 fix_missing_braces.py file1.c file2.c
    python3 fix_missing_braces.py --dry-run Src/*.c
"""

import re
import argparse
from pathlib import Path


def fix_display_struct_braces(content):
    """Fix DISPLAY_STRUCT initializations with missing braces around arrays."""
    
    # Pattern for DISPLAY_STRUCT single instance (display_mode)
    display_pattern = r'(DISPLAY_STRUCT\s+\w+\s*=\s*\{[^}]*?"[^"]*",\s*\w+,\s*)([^}]+)(\})'
    
    def fix_display_single(match):
        prefix = match.group(1)
        content = match.group(2)
        suffix = match.group(3)
        
        # Split by commas and group into arrays of 4 elements each
        elements = [elem.strip() for elem in content.split(',') if elem.strip()]
        
        if len(elements) == 12:  # 3 arrays of 4 elements each
            style_array = "{" + ", ".join(elements[0:4]) + "}"
            highlight_array = "{" + ", ".join(elements[4:8]) + "}"
            target_array = "{" + ", ".join(elements[8:12]) + "}"
            
            new_content = f"\n  {style_array},\n  {highlight_array},\n  {target_array}\n"
            return prefix + new_content + suffix
        
        return match.group(0)  # Return unchanged if not the expected pattern
    
    content = re.sub(display_pattern, fix_display_single, content, flags=re.DOTALL)
    
    # Pattern for DISPLAY_STRUCT array entries (base_modes)
    array_entry_pattern = r'(\{\s*"[^"]*",\s*\w+,\s*)([^}]+?)(\s*\})'
    
    def fix_array_entry(match):
        prefix = match.group(1)
        content = match.group(2)
        suffix = match.group(3)
        
        # Split by commas and group into arrays
        elements = [elem.strip() for elem in content.split(',') if elem.strip()]
        
        if len(elements) == 12:  # 3 arrays of 4 elements each
            style_array = "{" + ", ".join(elements[0:4]) + "}"
            highlight_array = "{" + ", ".join(elements[4:8]) + "}"
            target_array = "{" + ", ".join(elements[8:12]) + "}"
            
            new_content = f"\n      {style_array},\n      {highlight_array},\n      {target_array}"
            return prefix + new_content + suffix
        
        return match.group(0)  # Return unchanged if not the expected pattern
    
    content = re.sub(array_entry_pattern, fix_array_entry, content, flags=re.DOTALL)
    
    return content


def fix_ainfo_struct_braces(content):
    """Fix AINFO_STRUCT initializations with missing braces around pow_need array."""
    
    # Pattern to match AINFO_STRUCT entries that end with 3 values (pow_need[MAG_NUMBER])
    # The last three values can be identifiers (MM_*, MW_*) or hex values (0x0L)
    # Also handles bitwise operations like MW_VAMPIRE | MW_THEVOID
    # Examples: 
    #   1, 1, 0x0L, 0x0L, 0x0L }
    #   250, 50, MM_NINJA, 0x0L, 0x0L }
    #   250, 100, 0x0L, 0x0L, MW_VAMPIRE | MW_THEVOID }
    ainfo_pattern = r'(\s+)(\d+, \d+,)(\s*)([^,]+,\s*[^,]+,\s*[^}]+)(\s*\})'
    
    def fix_ainfo_entry(match):
        indent = match.group(1)
        prefix = match.group(2)
        spacing = match.group(3)
        pow_need_values = match.group(4)
        suffix = match.group(5)
        
        # Add braces around the pow_need array values
        braced_values = "{" + pow_need_values.strip() + "}"
        
        return indent + prefix + spacing + braced_values + suffix
    
    content = re.sub(ainfo_pattern, fix_ainfo_entry, content)
    
    return content


def fix_multidimensional_arrays(content):
    """Fix multi-dimensional array initializations."""
    
    # Pattern for declarations like: type array[X][Y] = {
    md_array_pattern = r'(\w+\s+\w+\[[\w_]+\]\[[\w_]+\]\s*=\s*\{)(.*?)(\};)'
    
    def fix_md_array(match):
        prefix = match.group(1)
        content = match.group(2)
        suffix = match.group(3)
        
        # Simple approach: add braces around comma-separated groups
        # This is a basic fix - more complex patterns may need specific handling
        lines = content.split('\n')
        fixed_lines = []
        
        for line in lines:
            line = line.strip()
            if line and not line.startswith('{') and not line.startswith('}'):
                # If line contains comma-separated values, wrap in braces
                if ',' in line and not line.startswith('{'):
                    # Remove trailing comma if present
                    line = line.rstrip(',')
                    line = '{' + line + '},'
            fixed_lines.append(line)
        
        new_content = '\n'.join(fixed_lines)
        return prefix + new_content + suffix
    
    content = re.sub(md_array_pattern, fix_md_array, content, flags=re.DOTALL)
    
    return content


def process_file(file_path, dry_run=False):
    """Process a single C file to fix missing braces."""
    
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            original_content = f.read()
        
        content = original_content
        
        # Apply fixes
        content = fix_display_struct_braces(content)
        content = fix_ainfo_struct_braces(content)
        content = fix_multidimensional_arrays(content)
        
        if content != original_content:
            if dry_run:
                print(f"Would fix missing braces in: {file_path}")
                return True
            else:
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(content)
                print(f"Fixed missing braces in: {file_path}")
                return True
        else:
            if not dry_run:
                print(f"No missing braces issues found in: {file_path}")
            return False
            
    except Exception as e:
        print(f"Error processing {file_path}: {e}")
        return False


def main():
    parser = argparse.ArgumentParser(description="Fix missing braces around initializers in C code")
    parser.add_argument('files', nargs='+', help='C source files to process')
    parser.add_argument('--dry-run', action='store_true', help='Show what would be changed without modifying files')
    
    args = parser.parse_args()
    
    files_changed = 0
    total_files = 0
    
    for file_pattern in args.files:
        file_paths = list(Path('.').glob(file_pattern))
        if not file_paths:
            file_paths = [Path(file_pattern)]
        
        for file_path in file_paths:
            if file_path.exists() and file_path.suffix == '.c':
                total_files += 1
                if process_file(file_path, args.dry_run):
                    files_changed += 1
    
    action = "Would change" if args.dry_run else "Changed"
    print(f"\n{action} {files_changed} out of {total_files} files.")


if __name__ == "__main__":
    main()
#!/usr/bin/env -S uv run --script
# /// script
# dependencies = ["regex", "pathlib", "argparse", "sys"]
# ///

"""
document_functions.py - Generate comprehensive function documentation for C modernization

This script analyzes C source files and generates standardized documentation
templates for all functions, preserving existing comments and adding missing
documentation according to C2023 modernization standards.

Usage:
    python3 document_functions.py src/*.c
    python3 document_functions.py --dry-run src/
    python3 document_functions.py --backup src/module.c

Features:
- Analyzes function signatures and parameters automatically
- Preserves existing comments and documentation
- Generates standardized documentation format
- Flags functions requiring manual analysis
- Creates backup files for validation
- Supports batch processing of multiple files
"""

import argparse
import re
import sys
from pathlib import Path
from typing import List, Dict, Tuple, Optional
import logging
from datetime import datetime

# Set up logging
logging.basicConfig(level=logging.INFO, format='%(levelname)s: %(message)s')
logger = logging.getLogger(__name__)

class FunctionInfo:
    """Represents information about a C function"""
    def __init__(self, name: str, return_type: str, parameters: List[Tuple[str, str]], 
                 line_start: int, line_end: int, existing_comment: str = ""):
        self.name = name
        self.return_type = return_type
        self.parameters = parameters  # List of (type, name) tuples
        self.line_start = line_start
        self.line_end = line_end
        self.existing_comment = existing_comment
        
    def __str__(self):
        return f"{self.return_type} {self.name}({', '.join(f'{t} {n}' for t, n in self.parameters)})"

class FunctionDocumenter:
    """Main class for analyzing and documenting C functions"""
    
    def __init__(self, dry_run: bool = False, create_backup: bool = False):
        self.dry_run = dry_run
        self.create_backup = create_backup
        self.functions_found: List[FunctionInfo] = []
        self.files_processed = 0
        self.functions_documented = 0
        
        # Regex patterns for function analysis
        self.function_pattern = re.compile(
            r'^(?:(?:static|extern|inline)\s+)?'  # Optional storage class
            r'([a-zA-Z_][a-zA-Z0-9_*\s]+?)\s+'   # Return type (may include *)
            r'([a-zA-Z_][a-zA-Z0-9_]*)\s*'       # Function name
            r'\(([^)]*)\)\s*'                    # Parameters
            r'\{',                               # Opening brace
            re.MULTILINE
        )
        
        # K&R style function pattern
        self.kr_function_pattern = re.compile(
            r'^([a-zA-Z_][a-zA-Z0-9_]*)\s*\('    # Function name
            r'([^)]*)\)\s*'                      # Parameters
            r'(?:\n[^{]*)*'                      # K&R parameter declarations
            r'\{',                               # Opening brace
            re.MULTILINE
        )
        
        # Comment pattern to detect existing documentation
        self.comment_pattern = re.compile(
            r'/\*\s*\*?\s*([^*](?:[^*]|\*(?!/))*)\*/',
            re.DOTALL
        )
        
    def analyze_file(self, file_path: Path) -> List[FunctionInfo]:
        """Analyze a C source file and extract function information"""
        logger.info(f"Analyzing file: {file_path}")
        
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
        except Exception as e:
            logger.error(f"Error reading {file_path}: {e}")
            return []
            
        functions = []
        lines = content.split('\n')
        
        # Find modern ANSI-style functions
        for match in self.function_pattern.finditer(content):
            return_type = match.group(1).strip()
            func_name = match.group(2).strip()
            params_str = match.group(3).strip()
            
            # Skip function declarations (no body)
            if ';' in match.group(0):
                continue
                
            # Parse parameters
            parameters = self.parse_parameters(params_str)
            
            # Find line numbers
            line_start = content[:match.start()].count('\n') + 1
            
            # Look for existing comment before function
            existing_comment = self.find_existing_comment(content, match.start())
            
            # Estimate line end (simple heuristic)
            brace_count = 1
            pos = match.end()
            while pos < len(content) and brace_count > 0:
                if content[pos] == '{':
                    brace_count += 1
                elif content[pos] == '}':
                    brace_count -= 1
                pos += 1
            line_end = content[:pos].count('\n') + 1
            
            func_info = FunctionInfo(
                func_name, return_type, parameters, 
                line_start, line_end, existing_comment
            )
            functions.append(func_info)
            
        # Find K&R style functions
        kr_functions = self.find_kr_functions(content)
        functions.extend(kr_functions)
        
        logger.info(f"Found {len(functions)} functions in {file_path}")
        return functions
        
    def parse_parameters(self, params_str: str) -> List[Tuple[str, str]]:
        """Parse function parameters from parameter string"""
        if not params_str or params_str.strip() == 'void':
            return []
            
        params = []
        # Simple parameter parsing (can be enhanced)
        for param in params_str.split(','):
            param = param.strip()
            if not param:
                continue
                
            # Split into type and name
            parts = param.split()
            if len(parts) >= 2:
                param_name = parts[-1].lstrip('*')
                param_type = ' '.join(parts[:-1]) + ('*' if '*' in parts[-1] else '')
                params.append((param_type, param_name))
            else:
                # Handle cases like "int" (implicit parameter name)
                params.append((param, 'param'))
                
        return params
        
    def find_existing_comment(self, content: str, func_start: int) -> str:
        """Find existing comment before a function"""
        # Look backwards from function start for comments
        lines = content[:func_start].split('\n')
        comment_lines = []
        
        # Search backwards for comment block
        for i in range(len(lines) - 1, -1, -1):
            line = lines[i].strip()
            if not line:
                continue
            if line.startswith('*/'):
                # Found end of comment block, collect it
                j = i
                while j >= 0 and not lines[j].strip().startswith('/*'):
                    comment_lines.insert(0, lines[j])
                    j -= 1
                if j >= 0:
                    comment_lines.insert(0, lines[j])
                break
            if line.startswith('//'):
                comment_lines.insert(0, line)
            else:
                break
                
        return '\n'.join(comment_lines) if comment_lines else ""
        
    def find_kr_functions(self, content: str) -> List[FunctionInfo]:
        """Find K&R style function definitions"""
        functions = []
        
        # This is a simplified K&R detector - can be enhanced
        lines = content.split('\n')
        for i, line in enumerate(lines):
            # Look for function-like patterns followed by parameter declarations
            if (re.match(r'^[a-zA-Z_][a-zA-Z0-9_]*\s*\([^)]*\)\s*$', line.strip()) and
                i + 1 < len(lines) and
                re.match(r'^[a-zA-Z_][a-zA-Z0-9_*\s]+[a-zA-Z_][a-zA-Z0-9_]*;', lines[i + 1].strip())):
                
                func_match = re.match(r'^([a-zA-Z_][a-zA-Z0-9_]*)\s*\(([^)]*)\)', line.strip())
                if func_match:
                    func_name = func_match.group(1)
                    params_str = func_match.group(2)
                    
                    # Parse K&R parameters (simplified)
                    parameters = [(params_str, param) for param in params_str.split(',') if param.strip()]
                    
                    func_info = FunctionInfo(
                        func_name, 'int', parameters,  # K&R functions default to int
                        i + 1, i + 10, ""  # Rough estimates
                    )
                    functions.append(func_info)
                    
        return functions
        
    def generate_documentation(self, func_info: FunctionInfo) -> str:
        """Generate standardized documentation for a function"""
        doc_lines = []
        
        # Function header comment
        doc_lines.append("/*")
        doc_lines.append(f" * {func_info.name} - Brief one-line description")
        doc_lines.append(" *")
        doc_lines.append(" * Detailed description explaining the function's purpose,")
        doc_lines.append(" * algorithm, and any important implementation details.")
        doc_lines.append(" *")
        
        # Parameters section
        if func_info.parameters:
            doc_lines.append(" * Parameters:")
            for param_type, param_name in func_info.parameters:
                doc_lines.append(f" *   {param_name} - Description of {param_name} parameter")
        else:
            doc_lines.append(" * Parameters:")
            doc_lines.append(" *   None")
            
        doc_lines.append(" *")
        
        # Returns section
        if func_info.return_type.strip() != 'void':
            doc_lines.append(" * Returns:")
            doc_lines.append(" *   Description of return value and meaning")
        else:
            doc_lines.append(" * Returns:")
            doc_lines.append(" *   None")
            
        doc_lines.append(" *")
        
        # Additional sections
        doc_lines.append(" * Side Effects:")
        doc_lines.append(" *   - Document any side effects")
        doc_lines.append(" *")
        doc_lines.append(" * Notes:")
        doc_lines.append(" *   - Additional notes, thread safety, performance considerations")
        doc_lines.append(" */")
        
        return '\n'.join(doc_lines)
        
    def process_file(self, file_path: Path) -> bool:
        """Process a single file and add documentation"""
        functions = self.analyze_file(file_path)
        if not functions:
            return True
            
        self.functions_found.extend(functions)
        
        if self.dry_run:
            logger.info(f"DRY RUN: Would document {len(functions)} functions in {file_path}")
            for func in functions:
                logger.info(f"  - {func}")
            return True
            
        # Read original content
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
        except Exception as e:
            logger.error(f"Error reading {file_path}: {e}")
            return False
            
        # Create backup if requested
        if self.create_backup:
            backup_path = file_path.with_suffix(file_path.suffix + '.orig')
            try:
                with open(backup_path, 'w', encoding='utf-8') as f:
                    f.write(content)
                logger.info(f"Created backup: {backup_path}")
            except Exception as e:
                logger.error(f"Error creating backup {backup_path}: {e}")
                return False
                
        # Add documentation for functions that don't have it
        modified = False
        lines = content.split('\n')
        
        for func_info in functions:
            if not func_info.existing_comment:
                # Insert documentation before function
                doc = self.generate_documentation(func_info)
                lines.insert(func_info.line_start - 1, doc)
                modified = True
                self.functions_documented += 1
                logger.info(f"Added documentation for function: {func_info.name}")
                
        if modified:
            try:
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write('\n'.join(lines))
                logger.info(f"Updated file: {file_path}")
            except Exception as e:
                logger.error(f"Error writing {file_path}: {e}")
                return False
                
        self.files_processed += 1
        return True
        
    def process_files(self, file_paths: List[Path]) -> bool:
        """Process multiple files"""
        success = True
        
        for file_path in file_paths:
            if not file_path.exists():
                logger.error(f"File does not exist: {file_path}")
                success = False
                continue
                
            if not file_path.is_file():
                logger.warning(f"Skipping non-file: {file_path}")
                continue
                
            if not file_path.suffix.lower() in ['.c', '.h']:
                logger.warning(f"Skipping non-C file: {file_path}")
                continue
                
            if not self.process_file(file_path):
                success = False
                
        return success
        
    def generate_report(self) -> str:
        """Generate a summary report of the documentation process"""
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        
        report = [
            f"Function Documentation Report - {timestamp}",
            "=" * 50,
            f"Files processed: {self.files_processed}",
            f"Functions found: {len(self.functions_found)}",
            f"Functions documented: {self.functions_documented}",
            "",
            "Functions requiring manual review:",
        ]
        
        for func in self.functions_found:
            if not func.existing_comment:
                report.append(f"  - {func.name} ({func.return_type}) - {len(func.parameters)} parameters")
                
        return '\n'.join(report)

def main():
    parser = argparse.ArgumentParser(
        description="Generate comprehensive documentation for C functions",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Document all C files in src/ directory
  python3 document_functions.py src/*.c
  
  # Dry run to see what would be documented
  python3 document_functions.py --dry-run src/
  
  # Create backups before modifying files
  python3 document_functions.py --backup src/module.c
  
  # Process specific files
  python3 document_functions.py file1.c file2.c file3.c
        """
    )
    
    parser.add_argument('files', nargs='+', help='C source files to process')
    parser.add_argument('--dry-run', action='store_true', 
                       help='Preview changes without modifying files')
    parser.add_argument('--backup', action='store_true',
                       help='Create .orig backup files')
    parser.add_argument('--verbose', '-v', action='store_true',
                       help='Enable verbose output')
    parser.add_argument('--report', type=str,
                       help='Save report to specified file')
    
    args = parser.parse_args()
    
    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)
        
    # Convert file arguments to Path objects
    file_paths = []
    for file_arg in args.files:
        path = Path(file_arg)
        if path.is_dir():
            # If directory given, find all .c and .h files
            file_paths.extend(path.glob('*.c'))
            file_paths.extend(path.glob('*.h'))
        else:
            file_paths.append(path)
            
    if not file_paths:
        logger.error("No C source files found to process")
        return 1
        
    logger.info(f"Processing {len(file_paths)} files...")
    
    documenter = FunctionDocumenter(dry_run=args.dry_run, create_backup=args.backup)
    success = documenter.process_files(file_paths)
    
    # Generate and display report
    report = documenter.generate_report()
    print("\n" + report)
    
    # Save report if requested
    if args.report:
        try:
            with open(args.report, 'w') as f:
                f.write(report)
            logger.info(f"Report saved to: {args.report}")
        except Exception as e:
            logger.error(f"Error saving report: {e}")
            
    return 0 if success else 1

if __name__ == '__main__':
    sys.exit(main())
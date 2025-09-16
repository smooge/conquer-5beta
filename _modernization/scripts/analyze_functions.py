#!/usr/bin/env -S uv run --script
# /// script
# dependencies = ["pathlib", "argparse", "re", "json"]
# ///

"""
Analyze Functions in C Source Files

This script analyzes C source files to extract detailed function information
including signatures, parameters, return types, and complexity metrics.

Usage:
    python3 analyze_functions.py Src/moveA.c
    python3 analyze_functions.py Src/*.c --output function_analysis.json
    python3 analyze_functions.py --scan-all --format table
"""

import argparse
import json
import re
import sys
from pathlib import Path

def analyze_function_complexity(func_body):
    """
    Analyze function complexity based on control structures and statements.
    Returns a complexity score and categorization.
    """
    if not func_body:
        return {'score': 0, 'category': 'unknown', 'metrics': {}}

    # Count different complexity indicators
    metrics = {
        'lines': len(func_body.split('\n')),
        'if_statements': len(re.findall(r'\\bif\\s*\\(', func_body)),
        'loops': len(re.findall(r'\\b(for|while|do)\\s*\\(', func_body)),
        'switch_statements': len(re.findall(r'\\bswitch\\s*\\(', func_body)),
        'function_calls': len(re.findall(r'[a-zA-Z_][a-zA-Z0-9_]*\\s*\\(', func_body)),
        'nested_braces': func_body.count('{'),
        'return_statements': len(re.findall(r'\\breturn\\b', func_body)),
    }

    # Calculate cyclomatic complexity (simplified)
    cyclomatic = 1  # Base complexity
    cyclomatic += metrics['if_statements']
    cyclomatic += metrics['loops']
    cyclomatic += metrics['switch_statements']
    cyclomatic += max(0, metrics['return_statements'] - 1)  # Multiple returns add complexity

    # Categorize complexity
    if cyclomatic <= 5 and metrics['lines'] <= 20:
        category = 'simple'
    elif cyclomatic <= 10 and metrics['lines'] <= 50:
        category = 'moderate'
    elif cyclomatic <= 20 and metrics['lines'] <= 100:
        category = 'complex'
    else:
        category = 'very_complex'

    return {
        'score': cyclomatic,
        'category': category,
        'metrics': metrics
    }

def extract_function_signature(func_line, following_lines):
    """
    Extract complete function signature handling K&R and ANSI styles.
    """
    # Handle multi-line function declarations
    signature = func_line
    paren_count = signature.count('(') - signature.count(')')

    line_idx = 0
    while paren_count > 0 and line_idx < len(following_lines):
        next_line = following_lines[line_idx].strip()
        signature += " " + next_line
        paren_count += next_line.count('(') - next_line.count(')')
        line_idx += 1

    return signature.strip()

def parse_parameters(param_string):
    """
    Parse function parameters into structured format.
    """
    if not param_string or param_string.strip() in ['void', '']:
        return []

    # Simple parameter parsing (may need refinement for complex types)
    params = []
    param_parts = param_string.split(',')

    for part in param_parts:
        part = part.strip()
        if part:
            # Try to separate type and name
            tokens = part.split()
            if len(tokens) >= 2:
                param_type = ' '.join(tokens[:-1])
                param_name = tokens[-1].lstrip('*')  # Remove pointer indicators from name
            else:
                param_type = part
                param_name = ''

            params.append({
                'type': param_type,
                'name': param_name,
                'full': part
            })

    return params

def extract_function_body(lines, start_line_idx):
    """
    Extract the complete function body from starting line index.
    """
    brace_count = 0
    body_lines = []
    in_function = False

    for i in range(start_line_idx, len(lines)):
        line = lines[i]
        body_lines.append(line)

        # Track brace nesting
        brace_count += line.count('{') - line.count('}')

        if '{' in line:
            in_function = True

        if in_function and brace_count == 0:
            break

    return '\\n'.join(body_lines)

def analyze_source_file(source_file):
    """
    Analyze a C source file and extract all function information.
    """
    try:
        with open(source_file, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()
    except Exception as e:
        return {'error': f"Could not read file: {e}", 'functions': []}

    # Remove comments
    content = re.sub(r'/\\*.*?\\*/', '', content, flags=re.DOTALL)
    content = re.sub(r'//.*$', '', content, flags=re.MULTILINE)

    lines = content.split('\\n')
    functions = []

    # Pattern to match function definitions
    function_pattern = r'^\\s*([a-zA-Z_][a-zA-Z0-9_\\s\\*]*?)\\s+([a-zA-Z_][a-zA-Z0-9_]*)\\s*\\([^)]*\\)\\s*\\{'

    for i, line in enumerate(lines):
        match = re.search(function_pattern, line)
        if match:
            return_type = match.group(1).strip()
            func_name = match.group(2).strip()

            # Skip if it looks like a macro or struct definition
            if func_name.isupper() or return_type.startswith('#'):
                continue

            # Extract complete signature
            following_lines = lines[i+1:i+10]  # Look ahead for multi-line signatures
            full_signature = extract_function_signature(line, following_lines)

            # Extract parameter list
            paren_start = full_signature.find('(')
            paren_end = full_signature.rfind(')')
            if paren_start != -1 and paren_end != -1:
                param_string = full_signature[paren_start+1:paren_end].strip()
                parameters = parse_parameters(param_string)
            else:
                param_string = ""
                parameters = []

            # Extract function body for analysis
            func_body = extract_function_body(lines, i)
            complexity = analyze_function_complexity(func_body)

            # Determine if this is K&R style
            is_kr_style = False
            if i + 1 < len(lines):
                next_line = lines[i + 1].strip()
                # K&R style typically has parameter declarations on following lines
                if next_line and not next_line.startswith('{') and ';' in next_line:
                    is_kr_style = True

            function_info = {
                'name': func_name,
                'return_type': return_type,
                'parameters': parameters,
                'parameter_string': param_string,
                'line_number': i + 1,
                'signature': full_signature,
                'complexity': complexity,
                'is_kr_style': is_kr_style,
                'parameter_count': len(parameters),
                'body_preview': func_body[:200] + '...' if len(func_body) > 200 else func_body
            }

            functions.append(function_info)

    return {
        'file': str(source_file),
        'function_count': len(functions),
        'functions': functions,
        'analysis_summary': {
            'total_functions': len(functions),
            'kr_style_functions': sum(1 for f in functions if f['is_kr_style']),
            'complexity_distribution': {
                'simple': sum(1 for f in functions if f['complexity']['category'] == 'simple'),
                'moderate': sum(1 for f in functions if f['complexity']['category'] == 'moderate'),
                'complex': sum(1 for f in functions if f['complexity']['category'] == 'complex'),
                'very_complex': sum(1 for f in functions if f['complexity']['category'] == 'very_complex'),
            }
        }
    }

def format_output(analysis_results, format_type='json'):
    """
    Format analysis results in the specified format.
    """
    if format_type == 'json':
        return json.dumps(analysis_results, indent=2)

    elif format_type == 'table':
        output = []
        for result in analysis_results:
            if 'error' in result:
                output.append(f"Error analyzing {result['file']}: {result['error']}")
                continue

            output.append(f"\\n=== {result['file']} ===")
            output.append(f"Total functions: {result['function_count']}")

            if result['functions']:
                output.append("\\nFunctions:")
                output.append(f"{'Name':<25} {'Return':<15} {'Params':<8} {'Complexity':<12} {'Style':<8} {'Line':<6}")
                output.append("-" * 80)

                for func in result['functions']:
                    style = 'K&R' if func['is_kr_style'] else 'ANSI'
                    complexity = f"{func['complexity']['category']} ({func['complexity']['score']})"

                    output.append(f"{func['name']:<25} {func['return_type']:<15} {func['parameter_count']:<8} {complexity:<12} {style:<8} {func['line_number']:<6}")

        return '\\n'.join(output)

    elif format_type == 'summary':
        total_functions = sum(r.get('function_count', 0) for r in analysis_results)
        total_files = len([r for r in analysis_results if 'error' not in r])

        output = [f"Analysis Summary: {total_files} files, {total_functions} functions"]

        # Aggregate complexity distribution
        complexity_totals = {'simple': 0, 'moderate': 0, 'complex': 0, 'very_complex': 0}
        kr_total = 0

        for result in analysis_results:
            if 'analysis_summary' in result:
                for category, count in result['analysis_summary']['complexity_distribution'].items():
                    complexity_totals[category] += count
                kr_total += result['analysis_summary']['kr_style_functions']

        output.append(f"\\nComplexity Distribution:")
        for category, count in complexity_totals.items():
            percentage = (count / total_functions * 100) if total_functions > 0 else 0
            output.append(f"  {category}: {count} ({percentage:.1f}%)")

        output.append(f"\\nK&R Style Functions: {kr_total} ({kr_total/total_functions*100:.1f}%)")

        return '\\n'.join(output)

    else:
        return "Unknown format type"

def main():
    parser = argparse.ArgumentParser(description='Analyze functions in C source files')
    parser.add_argument('source_files', nargs='*', help='C source files to analyze')
    parser.add_argument('--scan-all', action='store_true', help='Scan all C files in Src/ directory')
    parser.add_argument('--output', '-o', help='Output file for results')
    parser.add_argument('--format', choices=['json', 'table', 'summary'], default='table',
                        help='Output format (default: table)')

    args = parser.parse_args()

    # Collect source files
    source_files = []
    if args.scan_all:
        src_dir = Path('Src')
        if src_dir.exists():
            source_files.extend(src_dir.glob('*.c'))
        auxil_dir = Path('Auxil')
        if auxil_dir.exists():
            source_files.extend(auxil_dir.glob('*.c'))
        docs_dir = Path('Docs')
        if docs_dir.exists():
            source_files.extend(docs_dir.glob('*.c'))
    else:
        for pattern in args.source_files:
            if '*' in pattern:
                source_files.extend(Path('.').glob(pattern))
            else:
                source_files.append(Path(pattern))

    if not source_files:
        print("No source files specified or found")
        return 1

    # Analyze each file
    results = []
    for source_file in source_files:
        if source_file.exists():
            print(f"Analyzing {source_file}...", file=sys.stderr)
            analysis = analyze_source_file(source_file)
            results.append(analysis)
        else:
            results.append({'file': str(source_file), 'error': 'File not found', 'functions': []})

    # Format and output results
    formatted_output = format_output(results, args.format)

    if args.output:
        try:
            with open(args.output, 'w') as f:
                f.write(formatted_output)
            print(f"Results written to {args.output}", file=sys.stderr)
        except Exception as e:
            print(f"Error writing to {args.output}: {e}", file=sys.stderr)
            return 1
    else:
        print(formatted_output)

    return 0

if __name__ == '__main__':
    sys.exit(main())
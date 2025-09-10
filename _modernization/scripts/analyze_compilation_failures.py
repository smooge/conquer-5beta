#!/usr/bin/env -S uv run --script
# /// script
dependencies = ["pathlib"]
# ///

"""
Compilation Failure Analysis Script for Conquer C2023 Modernization

This script systematically tests compilation of all source files and
creates a detailed analysis of compilation failures, categorizing
common error patterns and providing actionable modernization guidance.

Usage: python3 analyze_compilation_failures.py
"""

import subprocess
import sys
import os
from pathlib import Path
from datetime import datetime

# Configuration
COMPILE_CMD = [
    "gcc", "-std=c2x", "-D_POSIX_C_SOURCE=200809L", 
    "-Wall", "-Wextra", "-Wpedantic", "-g", "-O2", "-c"
]
INCLUDE_PATH = "-I./Include"

def test_file_compilation(source_file):
    """
    Test compilation of a single source file.
    Returns (success, error_output, warnings)
    """
    cmd = COMPILE_CMD + [INCLUDE_PATH, source_file]
    
    try:
        result = subprocess.run(
            cmd, 
            capture_output=True, 
            text=True, 
            timeout=30
        )
        
        success = result.returncode == 0
        error_output = result.stderr
        warnings = []
        
        # Separate warnings from errors
        if error_output:
            lines = error_output.split('\n')
            for line in lines:
                if 'warning:' in line:
                    warnings.append(line.strip())
        
        return success, error_output, warnings
    
    except subprocess.TimeoutExpired:
        return False, "TIMEOUT: Compilation took longer than 30 seconds", []
    except Exception as e:
        return False, f"EXCEPTION: {str(e)}", []

def categorize_error(error_output):
    """
    Categorize compilation errors into common modernization patterns.
    """
    categories = []
    
    if "conflicting types" in error_output:
        categories.append("K&R_FUNCTION_CONFLICTS")
    
    if "implicit declaration" in error_output:
        categories.append("MISSING_FUNCTION_DECLARATIONS")
    
    if "incompatible pointer type" in error_output:
        categories.append("SIGNAL_HANDLER_SIGNATURE")
    
    if "expected identifier" in error_output or "syntax error" in error_output:
        categories.append("SYNTAX_ERRORS")
    
    if "undefined reference" in error_output:
        categories.append("MISSING_SYMBOLS")
    
    if "'/*' within comment" in error_output:
        categories.append("NESTED_COMMENT_ERRORS")
    
    if "comparison of integer expressions of different signedness" in error_output:
        categories.append("INTEGER_SIGNEDNESS_WARNINGS")
    
    if "this statement may fall through" in error_output:
        categories.append("FALLTHROUGH_WARNINGS")
    
    if "format specifier" in error_output or "format string" in error_output:
        categories.append("FORMAT_STRING_ISSUES")
    
    if not categories:
        categories.append("OTHER_ERRORS")
    
    return categories

def analyze_all_files():
    """
    Analyze compilation status of all source files.
    """
    print("=== Conquer C2023 Compilation Analysis ===")
    print(f"Starting analysis at {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
    print()
    
    # Find all source files
    src_files = list(Path("Src").glob("*.c"))
    auxil_files = list(Path("Auxil").glob("*.c"))
    all_files = src_files + auxil_files
    
    print(f"Found {len(src_files)} files in Src/")
    print(f"Found {len(auxil_files)} files in Auxil/")
    print(f"Total files to analyze: {len(all_files)}")
    print()
    
    # Analysis results
    results = {
        "success": [],
        "warnings_only": [],
        "errors": [],
        "error_categories": {},
        "file_details": {}
    }
    
    # Test each file
    for i, source_file in enumerate(all_files, 1):
        print(f"[{i:2d}/{len(all_files)}] Testing {source_file}...", end=" ")
        
        success, error_output, warnings = test_file_compilation(str(source_file))
        
        # Store detailed results
        results["file_details"][str(source_file)] = {
            "success": success,
            "error_output": error_output,
            "warnings": warnings
        }
        
        if success:
            if warnings:
                results["warnings_only"].append(str(source_file))
                print(f"OK (with {len(warnings)} warnings)")
            else:
                results["success"].append(str(source_file))
                print("OK")
        else:
            results["errors"].append(str(source_file))
            print("FAIL")
            
            # Categorize errors
            categories = categorize_error(error_output)
            for category in categories:
                if category not in results["error_categories"]:
                    results["error_categories"][category] = []
                results["error_categories"][category].append(str(source_file))
    
    return results

def generate_report(results):
    """
    Generate a comprehensive analysis report.
    """
    total_files = len(results["file_details"])
    success_count = len(results["success"])
    warnings_count = len(results["warnings_only"])
    error_count = len(results["errors"])
    
    print()
    print("=== COMPILATION ANALYSIS SUMMARY ===")
    print(f"Total Files Analyzed: {total_files}")
    print(f"✅ Clean Compilation: {success_count} ({success_count/total_files*100:.1f}%)")
    print(f"⚠️  Warnings Only: {warnings_count} ({warnings_count/total_files*100:.1f}%)")
    print(f"❌ Compilation Errors: {error_count} ({error_count/total_files*100:.1f}%)")
    print()
    
    if results["success"]:
        print("✅ FILES COMPILING SUCCESSFULLY:")
        for file in sorted(results["success"]):
            print(f"   {file}")
        print()
    
    if results["warnings_only"]:
        print("⚠️  FILES WITH WARNINGS ONLY:")
        for file in sorted(results["warnings_only"]):
            print(f"   {file}")
        print()
    
    if results["errors"]:
        print("❌ FILES WITH COMPILATION ERRORS:")
        for file in sorted(results["errors"]):
            print(f"   {file}")
        print()
    
    if results["error_categories"]:
        print("🔍 ERROR CATEGORY ANALYSIS:")
        for category, files in sorted(results["error_categories"].items()):
            print(f"   {category}: {len(files)} files")
            for file in sorted(files)[:3]:  # Show first 3 examples
                print(f"      - {file}")
            if len(files) > 3:
                print(f"      ... and {len(files) - 3} more")
        print()
    
    # Modernization recommendations
    print("🛠️  MODERNIZATION RECOMMENDATIONS:")
    if "K&R_FUNCTION_CONFLICTS" in results["error_categories"]:
        print("   1. CRITICAL: Fix K&R function declaration conflicts in headers")
        print("      - Update sysconf.h and other headers with modern prototypes")
    
    if "MISSING_FUNCTION_DECLARATIONS" in results["error_categories"]:
        print("   2. HIGH: Add missing function declarations")
        print("      - Include proper system headers for functions like setreuid")
    
    if "SIGNAL_HANDLER_SIGNATURE" in results["error_categories"]:
        print("   3. HIGH: Fix signal handler function signatures")
        print("      - Update handlers to accept int parameter: void handler(int sig)")
    
    if "SYNTAX_ERRORS" in results["error_categories"]:
        print("   4. CRITICAL: Fix syntax errors in header files")
        print("      - Check for malformed comments and syntax issues")
    
    if "NESTED_COMMENT_ERRORS" in results["error_categories"]:
        print("   5. MEDIUM: Fix nested comment issues")
        print("      - Update comment syntax to avoid /* within /* */ blocks")
    
    print()
    print("📋 NEXT STEPS:")
    print("   1. Fix critical header issues first (syntax errors, K&R conflicts)")
    print("   2. Address missing function declarations")
    print("   3. Update signal handler signatures")
    print("   4. Resolve remaining compilation errors systematically")
    print("   5. Address warnings for code quality improvement")

def save_detailed_report(results):
    """
    Save detailed error analysis to file for reference.
    """
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    report_file = f"_modernization/claude/reports/COMPILATION_ANALYSIS_{timestamp}.md"
    
    os.makedirs("_modernization/claude/reports", exist_ok=True)
    
    with open(report_file, "w") as f:
        f.write(f"# Comprehensive Compilation Analysis Report\n\n")
        f.write(f"**Generated**: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
        f.write(f"**Total Files**: {len(results['file_details'])}\n")
        f.write(f"**Success Rate**: {len(results['success'])/len(results['file_details'])*100:.1f}%\n\n")
        
        f.write("## Detailed Error Analysis\n\n")
        
        for file_path, details in sorted(results["file_details"].items()):
            f.write(f"### {file_path}\n")
            if details["success"]:
                if details["warnings"]:
                    f.write("**Status**: ⚠️ Warnings Only\n")
                    f.write("**Warnings**:\n")
                    for warning in details["warnings"][:5]:  # Limit to first 5
                        f.write(f"- {warning}\n")
                else:
                    f.write("**Status**: ✅ Success\n")
            else:
                f.write("**Status**: ❌ Compilation Error\n")
                f.write("**Error Output**:\n")
                f.write("```\n")
                f.write(details["error_output"][:1000])  # Limit error output
                f.write("\n```\n")
            f.write("\n")
    
    print(f"📄 Detailed report saved to: {report_file}")

def main():
    """
    Main analysis function.
    """
    # Change to project root if needed
    if not os.path.exists("Src"):
        print("Error: Must run from project root directory (where Src/ exists)")
        sys.exit(1)
    
    print("Analyzing compilation status of all Conquer source files...")
    print()
    
    # Run analysis
    results = analyze_all_files()
    
    # Generate reports
    generate_report(results)
    save_detailed_report(results)
    
    # Exit with appropriate code
    if results["errors"]:
        print(f"\n❌ Analysis complete. {len(results['errors'])} files need modernization work.")
        sys.exit(1)
    else:
        print(f"\n✅ Analysis complete. All files compile successfully!")
        sys.exit(0)

if __name__ == "__main__":
    main()
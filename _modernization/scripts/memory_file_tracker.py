#!/usr/bin/env -S uv run --script
# /// script
# dependencies = ["pathlib"]
# ///

"""
Memory File Tracker - Organize and track Claude modernization session memory files

This script provides better organization and tracking of memory files to help
understand project status and find the most relevant session information.
"""

import re
from pathlib import Path
from datetime import datetime
from typing import Dict, List, Tuple, Optional

def parse_memory_filename(filename: str) -> Dict[str, str]:
    """Parse memory filename into components."""
    # Standard format: PHASE{N}_{TYPE}_{COMPONENT}_{YYYYMMDD}_{HHMMSS}.md
    pattern = r'PHASE(\w+)_(\w+)_(.+?)_(\d{8})_(\d{6})\.md'
    match = re.match(pattern, filename)

    if match:
        phase, type_name, component, date_str, time_str = match.groups()
        return {
            'phase': phase,
            'type': type_name,
            'component': component,
            'date': date_str,
            'time': time_str,
            'datetime': f"{date_str}_{time_str}",
            'parsed': True
        }

    # Try to extract any phase information from irregular filenames
    phase_match = re.search(r'PHASE[_\d\w]*', filename, re.IGNORECASE)
    date_match = re.search(r'(\d{8})', filename)

    return {
        'phase': phase_match.group(0) if phase_match else 'UNKNOWN',
        'type': 'IRREGULAR',
        'component': filename.replace('.md', ''),
        'date': date_match.group(1) if date_match else '00000000',
        'time': '000000',
        'datetime': f"{date_match.group(1) if date_match else '00000000'}_000000",
        'parsed': False
    }

def get_memory_files(memory_dir: Path) -> List[Tuple[str, Dict[str, str]]]:
    """Get all memory files with parsed information."""
    files = []
    for file_path in memory_dir.glob("*.md"):
        if file_path.name in ['CURRENT_STATUS.md', 'NEXT_SESSION_INSTRUCTIONS.md',
                             'PERSISTENT_BUGS.md', 'PERSISTENT_TODOS.md',
                             'MEMORY_FILE_NAMING_CONVENTION.md']:
            continue  # Skip special files

        parsed = parse_memory_filename(file_path.name)
        files.append((file_path.name, parsed))

    return sorted(files, key=lambda x: x[1]['datetime'], reverse=True)

def group_by_phase(files: List[Tuple[str, Dict[str, str]]]) -> Dict[str, List[Tuple[str, Dict[str, str]]]]:
    """Group files by phase."""
    phases = {}
    for filename, parsed in files:
        phase = parsed['phase']
        if phase not in phases:
            phases[phase] = []
        phases[phase].append((filename, parsed))
    return phases

def find_latest_by_type(files: List[Tuple[str, Dict[str, str]]], file_type: str) -> Optional[Tuple[str, Dict[str, str]]]:
    """Find the latest file of a specific type."""
    matching = [(f, p) for f, p in files if file_type.upper() in p['type'].upper()]
    return matching[0] if matching else None

def format_datetime(date_str: str, time_str: str) -> str:
    """Format datetime for display."""
    try:
        dt = datetime.strptime(f"{date_str}_{time_str}", "%Y%m%d_%H%M%S")
        return dt.strftime("%Y-%m-%d %H:%M:%S")
    except:
        return f"{date_str} {time_str}"

def main():
    """Main tracker function."""
    memory_dir = Path("/home/ssmoogen/conquer-project/conquer-5beta/_modernization/memory")

    if not memory_dir.exists():
        print("❌ Memory directory not found!")
        return

    print("🧠 CONQUER PROJECT MEMORY FILE TRACKER")
    print("=" * 50)

    # Get all memory files
    files = get_memory_files(memory_dir)

    if not files:
        print("No memory files found.")
        return

    print(f"\n📁 Total Memory Files: {len(files)}")

    # Show most recent files
    print("\n🕒 MOST RECENT SESSIONS (Last 10)")
    print("-" * 50)
    for i, (filename, parsed) in enumerate(files[:10]):
        dt_str = format_datetime(parsed['date'], parsed['time'])
        status = "✅" if parsed['parsed'] else "⚠️"
        print(f"{i+1:2d}. {status} {dt_str} | {parsed['phase']:8s} | {parsed['type']:12s} | {parsed['component']}")

    # Group by phase
    phases = group_by_phase(files)

    print(f"\n📊 BY PHASE SUMMARY")
    print("-" * 50)
    for phase in sorted(phases.keys()):
        count = len(phases[phase])
        latest = phases[phase][0][1]  # Most recent in this phase
        latest_date = format_datetime(latest['date'], latest['time'])
        print(f"{phase:12s}: {count:3d} files | Latest: {latest_date}")

    # Find key status files
    print(f"\n🎯 KEY STATUS FILES")
    print("-" * 50)

    # Latest session completion
    latest_complete = find_latest_by_type(files, 'COMPLETE')
    if latest_complete:
        filename, parsed = latest_complete
        dt_str = format_datetime(parsed['date'], parsed['time'])
        print(f"Latest Complete: {dt_str} | {filename}")

    # Latest session progress
    latest_session = find_latest_by_type(files, 'SESSION')
    if latest_session:
        filename, parsed = latest_session
        dt_str = format_datetime(parsed['date'], parsed['time'])
        print(f"Latest Session:  {dt_str} | {filename}")

    # Check special files
    special_files = ['CURRENT_STATUS.md', 'NEXT_SESSION_INSTRUCTIONS.md']
    print(f"\n📋 SPECIAL STATUS FILES")
    print("-" * 50)
    for special in special_files:
        special_path = memory_dir / special
        if special_path.exists():
            mtime = datetime.fromtimestamp(special_path.stat().st_mtime)
            print(f"✅ {special:25s} | Modified: {mtime.strftime('%Y-%m-%d %H:%M:%S')}")
        else:
            print(f"❌ {special:25s} | Missing")

    # Provide recommendations
    print(f"\n💡 RECOMMENDATIONS")
    print("-" * 50)

    # Find the actual latest work
    if files:
        latest_file, latest_parsed = files[0]
        latest_dt = format_datetime(latest_parsed['date'], latest_parsed['time'])
        print(f"📅 Most Recent Work: {latest_dt}")
        print(f"📄 File: {latest_file}")
        print(f"🎯 Phase: {latest_parsed['phase']}")
        print(f"📝 Type: {latest_parsed['type']}")
        print(f"🔧 Component: {latest_parsed['component']}")

        print(f"\n🚀 To continue from latest session:")
        print(f"   claude load {latest_file}")

    # Check for status inconsistencies
    current_status_path = memory_dir / "CURRENT_STATUS.md"
    if current_status_path.exists():
        current_mtime = datetime.fromtimestamp(current_status_path.stat().st_mtime)
        if files:
            latest_session_date = datetime.strptime(f"{files[0][1]['date']}_{files[0][1]['time']}", "%Y%m%d_%H%M%S")
            if current_mtime < latest_session_date:
                print(f"\n⚠️  WARNING: CURRENT_STATUS.md appears outdated!")
                print(f"   Status file: {current_mtime.strftime('%Y-%m-%d %H:%M:%S')}")
                print(f"   Latest work: {latest_session_date.strftime('%Y-%m-%d %H:%M:%S')}")

if __name__ == "__main__":
    main()
import argparse
import os
import subprocess
import sys
from pathlib import Path

def main():
    parser = argparse.ArgumentParser(description="Convert PMX models to FBX")
    parser.add_argument("path", help="Path to a PMX file or a directory containing PMX files")
    args = parser.parse_args()

    target_path = Path(args.path)

    if not target_path.exists():
        print(f"Error: Path does not exist: {target_path}", file=sys.stderr)
        sys.exit(1)

    script_dir = Path(__file__).resolve().parent
    exe_path = (script_dir.parent / "bin" / "Pmx2Fbx.exe").resolve()

    if not exe_path.exists():
        print(f"Error: Cannot find converter at: {exe_path}. Please ensure you copied Pmx2Fbx.exe to the skill's bin folder.", file=sys.stderr)
        sys.exit(1)

    pmx_files = []

    if target_path.is_dir():
        print(f"Finding all PMX models in directory: {target_path}")
        pmx_files = list(target_path.rglob("*.pmx"))
    else:
        if target_path.suffix.lower() == ".pmx":
            pmx_files = [target_path]
        else:
            print(f"Error: Target file is not a PMX file: {target_path}", file=sys.stderr)
            sys.exit(1)

    if not pmx_files:
        print("No PMX files found in target path.")
        sys.exit(0)

    print("Starting in-place conversion from PMX to FBX...")

    for pmx in pmx_files:
        print(f"Converting: {pmx}")
        # Run the converter
        try:
            subprocess.run([str(exe_path), str(pmx)], check=True)
        except subprocess.CalledProcessError as e:
            print(f"Error converting {pmx}: {e}", file=sys.stderr)
            sys.exit(1)

    print("Conversion completed successfully!")

if __name__ == "__main__":
    main()

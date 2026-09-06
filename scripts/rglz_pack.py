#!/usr/bin/env python3

import argparse
import struct
from pathlib import Path


# Format:
#
#   uint64_t              number_of_files
#
#   repeated N times:
#       uint64_t          filename_length
#       uint8_t[]         filename bytes
#       uint64_t          file_offset
#
#   repeated N times:
#       raw file data
#
#
# All integers are little-endian and tightly packed.
UINT64 = struct.Struct("<Q")


def pack_folder(folder: Path, output: Path) -> None:
    if not folder.is_dir():
        raise ValueError(f"Not a directory: {folder}")

    # Only regular files directly inside the folder.
    files = sorted(
        (p for p in folder.iterdir() if p.is_file()),
        key=lambda p: p.name
    )

    # Encode filenames once so we know their exact byte lengths.
    entries = []
    for path in files:
        name_bytes = path.name.encode("utf-8")
        entries.append((path, name_bytes))

    number_of_files = len(entries)

    # Calculate the size of the header.
    #
    # uint64_t number_of_files
    # +
    # for every file:
    #     uint64_t string length
    #     string bytes
    #     uint64_t offset
    header_size = UINT64.size

    for _, name_bytes in entries:
        header_size += UINT64.size       # string length
        header_size += len(name_bytes)   # string
        header_size += UINT64.size       # offset

    # File data starts immediately after the header.
    current_offset = header_size

    offsets = []

    for path, _ in entries:
        offsets.append(current_offset)
        current_offset += path.stat().st_size

    with output.open("wb") as archive:
        # Number of files.
        archive.write(UINT64.pack(number_of_files))

        # File table.
        for (path, name_bytes), offset in zip(entries, offsets):
            # String length.
            archive.write(UINT64.pack(len(name_bytes)))

            # String itself -- NO null terminator.
            archive.write(name_bytes)

            # Offset from the beginning of the archive.
            archive.write(UINT64.pack(offset))

        # File contents.
        for path, _ in entries:
            with path.open("rb") as f:
                while True:
                    chunk = f.read(1024 * 1024)
                    if not chunk:
                        break
                    archive.write(chunk)

    print(f"Created: {output}")
    print(f"Files:   {number_of_files}")
    print(f"Size:    {output.stat().st_size} bytes")


def main():
    parser = argparse.ArgumentParser(
        description="Pack all files in a folder into an .rglz archive."
    )

    parser.add_argument(
        "folder",
        type=Path,
        help="Folder containing the files to pack"
    )

    parser.add_argument(
        "-o",
        "--output",
        type=Path,
        help="Output .rglz file (default: <folder_name>.rglz)"
    )

    args = parser.parse_args()

    folder = args.folder.resolve()

    if args.output is None:
        output = folder.parent / f"{folder.name}.rglz"
    else:
        output = args.output

    # Ensure the requested extension.
    if output.suffix.lower() != ".rglz":
        output = output.with_suffix(".rglz")

    pack_folder(folder, output)


if __name__ == "__main__":
    main()

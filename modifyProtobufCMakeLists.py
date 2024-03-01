from pathlib import Path
import re, os, subprocess

def update_line_in_file(file_path, pattern, replacement):
    with open(file_path, 'r') as file:
        content = file.read()

    updated_content = re.sub(pattern, replacement, content)

    with open(file_path, 'w') as file:
        file.write(updated_content)

def main():
    cmake_lists_path = Path(__file__).parent / 'third_party/grpc/third_party/protobuf/CMakeLists.txt'

    search_pattern = r'set\(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded\$<\$<CONFIG:Debug>:Debug>DLL\)'
    replacement_pattern = r'set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>)'

    update_line_in_file(cmake_lists_path, search_pattern, replacement_pattern)

if __name__ == "__main__":
    main()
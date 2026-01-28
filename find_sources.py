import os

def find_cpp_files(start_dir):
    cpp_files = []
    for root, dirs, files in os.walk(start_dir):
        for file in files:
            if file.endswith(".cpp"):
                cpp_files.append(os.path.join(root, file))
    return cpp_files

dirs_to_search = [
    "Core/common",
    "Core/core",
    "Core/audio_core",
    "Core/video_core",
    "Core/input_common",
    "Core/network",
    "Core/web_service"
]

all_sources = []
for d in dirs_to_search:
    all_sources.extend(find_cpp_files(d))

# Filter out some known problematic ones or platform specific if needed
# For now, let's just print them all
for s in sorted(all_sources):
    print(s)

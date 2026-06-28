import os
import glob

def get_files(pattern):
    return [f.replace('\\', '/') for f in glob.glob(pattern, recursive=True)]

src_files = get_files('f:/PROJECT/RETAIL/src/**/*.cpp')
src_files += get_files('f:/PROJECT/RETAIL/src/**/*.h')

# relative paths
src_files = [os.path.relpath(f, 'f:/PROJECT/RETAIL').replace('\\', '/') for f in src_files]

with open('f:/PROJECT/RETAIL/CMakeLists.txt', 'r') as f:
    content = f.read()

import re

# find where add_executable is
src_list_str = "\n    ".join(src_files)
content = re.sub(r'add_executable\(RetailMS\n[^\)]+\)', f'add_executable(RetailMS\n    {src_list_str}\n)', content)

# But earlier we had add_executable(RetailMS WIN32 src/main.cpp).
# Let's just create a target_sources
target_sources_str = f"target_sources(RetailMS PRIVATE \n    {src_list_str}\n)"

if "target_sources(RetailMS" in content:
    content = re.sub(r'target_sources\(RetailMS PRIVATE.*?\)', target_sources_str, content, flags=re.DOTALL)
else:
    content += "\n" + target_sources_str + "\n"
    
with open('f:/PROJECT/RETAIL/CMakeLists.txt', 'w') as f:
    f.write(content)

print("Updated CMakeLists.txt with all source files.")

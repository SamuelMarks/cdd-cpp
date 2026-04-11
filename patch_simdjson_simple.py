import os
import re

# Since Makefile does `cd build_wasm && python3 ../patch_simdjson_simple.py`
# The current working directory is `build_wasm`
base_dir = "_deps/simdjson-src/include/simdjson"

if not os.path.exists(base_dir):
    print(f"ERROR: {base_dir} does not exist!")
    exit(1)

for root, _, files in os.walk(base_dir):
    for file in files:
        if file.endswith(".h") or file.endswith(".hpp") or file.endswith("-inl.h"):
            path = os.path.join(root, file)
            with open(path, "r") as f:
                content = f.read()
            original = content
            
            # Simple replacements
            content = content.replace("throw simdjson_error(error());", "abort();")
            content = content.replace("throw simdjson_error(x.error());", "abort();")
            content = content.replace("throw simdjson::simdjson_error(error);", "abort();")
            content = content.replace("throw simdjson::simdjson_error(x.error());", "abort();")
            content = content.replace("throw  simdjson::simdjson_error(x.error());", "abort();")
            content = content.replace("throw std::out_of_range(\"nonstd::string_view::at()\");", "abort();")
            content = content.replace("throw std::out_of_range(\"nonstd::string_view::copy()\");", "abort();")
            content = content.replace("throw std::out_of_range(\"nonstd::string_view::substr()\");", "abort();")
            
            # Regex fallback just in case
            content = re.sub(r'throw\s+simdjson_error\([^)]*\);', 'abort();', content)
            content = re.sub(r'throw\s+simdjson::simdjson_error\([^)]*\);', 'abort();', content)
            content = re.sub(r'throw\s+std::out_of_range\([^)]*\);', 'abort();', content)
            
            if content != original:
                if "#include <stdlib.h>" not in content:
                    content = "#include <stdlib.h>\n" + content
                with open(path, "w") as f:
                    f.write(content)

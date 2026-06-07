#!/usr/bin/env python3
"""
Phase 0.4: Protocol Generator (v2)
Parses CommonStruct.h and generates clean .fbs files in a separate output dir.
Does NOT modify existing protocol files.
"""

import re, os, sys

SRC = "/Users/macbookair/PRIBADI/luna-plus-master/src/common/header/CommonStruct.h"
DST = "/Users/macbookair/PRIBADI/luna-plus-master/LUNA-Plus-Reborn/tools/generated_fbs"

TYPE_MAP = {
    'BYTE': 'uint8', 'char': 'uint8', 'BOOL': 'bool',
    'WORD': 'uint16', 'short': 'int16',
    'DWORD': 'uint32', 'int': 'int32', 'INT': 'int32', 'long': 'int32',
    'float': 'float', 'double': 'double',
    'DWORDEX': 'uint64', '__int64': 'int64',
    'VECTOR3': 'Vec3', 'VECTOR2': 'Vec2',
    'LEVELTYPE': 'uint16', 'POSTYPE': 'uint16', 'MONEYTYPE': 'uint32',
}

def clean_comment(line):
    idx = line.find('//')
    return line[:idx] if idx >= 0 else line

def parse_structs(source):
    structs = []
    # Match "struct NAME {" or "typedef struct { ... } NAME;" or "struct NAME : public BASE {"
    struct_pattern = re.compile(r'(?:typedef\s+)?struct\s*(\w+)?\s*(?::\s*public\s+\w+)?\s*\{', re.MULTILINE)
    
    pos = 0
    while True:
        m = struct_pattern.search(source, pos)
        if not m: break
        
        start_pos = m.start()
        name = m.group(1)
        
        # Find the matching closing brace
        brace_depth = 0
        end_pos = -1
        for j in range(start_pos, len(source)):
            if source[j] == '{': brace_depth += 1
            elif source[j] == '}':
                brace_depth -= 1
                if brace_depth == 0:
                    end_pos = j
                    break
        
        if end_pos == -1: break
        
        # If typedef and name was empty, it might be at the end
        if not name:
            after_brace = source[end_pos+1:end_pos+100]
            tm = re.match(r'\s*(\w+)\s*;', after_brace)
            if tm: name = tm.group(1)
        
        if not name: name = f"anon_{len(structs)}"
        
        body = source[m.end():end_pos]
        fields = []
        for line in body.split('\n'):
            line = clean_comment(line).strip().rstrip(';')
            if not line or line.startswith('#'): continue
            
            # Match "Type Name;" or "Type Name[Size];"
            fm = re.match(r'(\w+(?:\s+\w+)?)\s+(\w+)(?:\[.*\])?', line)
            if fm:
                cpp_type, field_name = fm.group(1).strip(), fm.group(2).strip()
                fb_type = TYPE_MAP.get(cpp_type, 'uint8')
                fields.append((fb_type, field_name))
        
        if fields:
            structs.append({'name': name, 'fields': fields, 'line': start_pos})
        
        pos = end_pos + 1
    return structs

def main():
    if not os.path.exists(SRC):
        print(f"Source not found: {SRC}")
        return
    with open(SRC, 'r', errors='replace') as f:
        source = f.read()
    
    structs = parse_structs(source)
    print(f"Parsed {len(structs)} structs from CommonStruct.h")
    os.makedirs(DST, exist_ok=True)
    
    count = 0
    for s in structs:
        # Filter for important packets (MSG_ prefix or specific keywords)
        name = s['name']
        if not (name.startswith('MSG_') or name.startswith('SEND_') or "INFO" in name):
            continue
            
        out_path = os.path.join(DST, f"{name}.fbs")
        with open(out_path, 'w') as f:
            f.write(f"// Auto-generated from CommonStruct.h:{s['line']}\n")
            f.write(f"namespace luna.protocol;\n\n")
            f.write("struct Vec3 { x:float; y:float; z:float; }\n\n")
            f.write(f"table {name} {{\n")
            for fb_type, field_name in s['fields']:
                f.write(f"    {field_name}: {fb_type};\n")
            f.write("}\n")
        count += 1
        if count >= 200: break # Task 3.3A limit
    
    print(f"Generated {count} fbs files in {DST}")


if __name__ == "__main__":
    main()

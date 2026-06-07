#!/usr/bin/env python3
"""
MOD to OBJ Converter for LUNA Plus (v4 - Working!)
Based on reverse-engineered 4DyuchiGX .MOD format.
"""

import struct, sys, os

OT_MATERIAL = 0x00F00000
OT_MESH     = 0xF4000000
OT_BONE     = 0xF5000000
BASE_SZ     = 324  # FILE_BASE_OBJECT_HEADER
FG_HDR_SZ   = 24   # 6 DWORDs: mtl, idx, faces, max, vi, luv


def convert_mod(input_path, output_dir):
    name = os.path.splitext(os.path.basename(input_path))[0]
    
    with open(input_path, 'rb') as f:
        data = f.read()
    
    if len(data) < 28:
        return False
    
    ver, obj_num, mtl_num = struct.unpack_from('<III', data, 0)[:3]
    
    offset = 28
    # Skip materials
    for i in range(mtl_num):
        mtype, msize = struct.unpack_from('<II', data, offset)
        offset += 8 + msize
    
    all_verts = []
    all_uvs = []
    all_faces = []
    
    for obj_idx in range(obj_num):
        if offset + 8 > len(data):
            break
        otype, osize = struct.unpack_from('<II', data, offset)
        offset += 8
        obj_end = offset + osize
        
        if otype == OT_MESH:
            # Skip CBaseObject
            child_num = struct.unpack_from('<I', data, offset + 188)[0]
            offset += BASE_SZ + child_num * 4
            
            if offset + 7*4 + 24 > len(data):
                offset = obj_end
                continue
            
            # FILE_MESH_HEADER: 7 DWORDs
            mhdr = struct.unpack_from('<7I', data, offset)
            max_vn, vn, orig_vn, ext_vn, tex_vn, mtl_idx, fg_num = mhdr
            
            if vn == 0 or vn > 50000 or fg_num == 0:
                offset = obj_end
                continue
            
            offset += 7*4      # skip header
            offset += 4 + 4 + 12 + 4  # meshFlag + gridIdx + v3Dir + flag2
            
            base_v = len(all_verts)
            
            # Read vertices
            for i in range(vn):
                x, y, z = struct.unpack_from('<fff', data, offset)
                all_verts.append((x, y, z))
                offset += 12
            
            # Read UVs
            base_uv = len(all_uvs)
            for i in range(tex_vn):
                u, v = struct.unpack_from('<ff', data, offset)
                all_uvs.append((u, 1.0 - v))
                offset += 8
            while len(all_uvs) < len(all_verts):
                all_uvs.append((0, 0))
            
            # Skip extended vertex indices
            offset += ext_vn * 4
            
            # Read face groups (triangle strips with degenerates)
            for fg in range(fg_num):
                if offset + FG_HDR_SZ > len(data):
                    break
                h = struct.unpack_from('<6I', data, offset)
                fg_mtl, fg_idx, fg_faces, fg_max, fg_vi, fg_luv = h
                
                if fg_faces == 0 or fg_faces > 50000:
                    break
                
                idx_off = offset + FG_HDR_SZ
                # FACE INDICES: fg_faces * 3 WORDs = stored as triangle strip
                # Each group of 3 WORDs is one triangle
                for f in range(fg_faces):
                    if idx_off + f*6 + 6 > len(data):
                        break
                    a, b, c = struct.unpack_from('<HHH', data, idx_off + f*6)
                    # Skip degenerate triangles (used for strip restart)
                    if a == b or b == c or a == c:
                        continue
                    if a < vn and b < vn and c < vn:
                        all_faces.append((base_v + a, base_v + b, base_v + c, fg_mtl))
                
                offs = FG_HDR_SZ + fg_faces * 6
                if fg_luv > 0:
                    offs += fg_luv * 8
                offset += offs
        
        offset = max(offset, obj_end)
    
    if not all_verts or not all_faces:
        return False
    
    dst = os.path.join(output_dir, f"{name}.obj")
    with open(dst, 'w') as f:
        f.write(f"# MOD converted: {os.path.basename(input_path)}\n")
        f.write(f"# {len(all_verts)} verts, {len(all_faces)} faces\n\n")
        for v in all_verts:
            f.write(f"v {v[0]:.6f} {v[1]:.6f} {v[2]:.6f}\n")
        f.write("\n")
        for uv in all_uvs[:len(all_verts)]:
            f.write(f"vt {uv[0]:.6f} {uv[1]:.6f}\n")
        f.write("\n")
        for fa in all_faces:
            f.write(f"f {fa[0]+1}/{fa[0]+1} {fa[1]+1}/{fa[1]+1} {fa[2]+1}/{fa[2]+1}\n")
    
    print(f"  {name}: {len(all_verts)}v {len(all_faces)}t -> OK")
    return True


def main():
    dst = "/Users/macbookair/PRIBADI/luna-plus-master/LUNA-Plus-Reborn/assets_converted/mod_objs"
    os.makedirs(dst, exist_ok=True)
    
    src = "/Users/macbookair/PRIBADI/luna-plus-master/assets/unpacked/character"
    mods = sorted([f for f in os.listdir(src) if f.endswith('.mod')])
    
    print(f"Converting {len(mods)} character .MOD files...\n")
    ok = 0
    for m in mods[:30]:
        if convert_mod(os.path.join(src, m), dst):
            ok += 1
    print(f"\nResult: {ok}/{min(30, len(mods))} OK")


if __name__ == "__main__":
    main()

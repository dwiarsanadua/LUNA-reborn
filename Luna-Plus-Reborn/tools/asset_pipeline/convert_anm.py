import struct
import sys
import os
import json

def read_str(f, length):
    try:
        data = f.read(length)
        return data.decode('euc-kr', errors='replace').split('\x00')[0].strip()
    except:
        return ""

class AnmConverter:
    def __init__(self, file_path):
        self.file_path = file_path
        self.header = {}
        self.motion_objects = []

    def parse(self):
        if not os.path.exists(self.file_path):
            return False
            
        try:
            with open(self.file_path, 'rb') as f:
                # FILE_MOTION_HEADER
                header_data = f.read(160)
                if len(header_data) < 160: return False
                
                h = struct.unpack('<IIIIIII 4x 128s', header_data)
                self.header = {
                    'version': h[0],
                    'ticks_per_frame': h[1],
                    'first_frame': h[2],
                    'last_frame': h[3],
                    'frame_speed': h[4],
                    'obj_num': h[5],
                    'step': h[6],
                    'name': h[7].decode('euc-kr', errors='replace').split('\x00')[0].strip()
                }
                
                for i in range(self.header['obj_num']):
                    obj_type_data = f.read(4)
                    if not obj_type_data: break
                    obj_type = struct.unpack('<I', obj_type_data)[0]
                    obj_size = struct.unpack('<I', f.read(4))[0]
                    obj_start = f.tell()

                    mo_hdr_data = f.read(152)
                    if len(mo_hdr_data) < 152: break
                    mo_h = struct.unpack('<IIIII 128s I', mo_hdr_data)
                    
                    mo = {
                        'index': mo_h[0],
                        'rot_keys_num': mo_h[1],
                        'pos_keys_num': mo_h[2],
                        'scale_keys_num': mo_h[3],
                        'morph_keys_num': mo_h[4],
                        'name': mo_h[5].decode('euc-kr', errors='replace').split('\x00')[0].strip(),
                        'flag': mo_h[6],
                        'pos_keys': [],
                        'rot_keys': [],
                        'scale_keys': []
                    }

                    for _ in range(mo['pos_keys_num']):
                        data = f.read(20)
                        if len(data) < 20: break
                        k = struct.unpack('<II fff', data)
                        mo['pos_keys'].append({'ticks': k[0], 'frame': k[1], 'pos': [k[2], k[3], k[4]]})

                    for _ in range(mo['rot_keys_num']):
                        data = f.read(24)
                        if len(data) < 24: break
                        k = struct.unpack('<II ffff', data)
                        mo['rot_keys'].append({'ticks': k[0], 'frame': k[1], 'quat': [k[2], k[3], k[4], k[5]]})

                    for _ in range(mo['scale_keys_num']):
                        data = f.read(36)
                        if len(data) < 36: break
                        k = struct.unpack('<II fff fff f', data)
                        mo['scale_keys'].append({'ticks': k[0], 'frame': k[1], 'scale': [k[2], k[3], k[4]]})

                    f.seek(obj_start + obj_size)
                    self.motion_objects.append(mo)
            return True
        except Exception as e:
            print(f"Error parsing {self.file_path}: {e}")
            return False

    def save_json(self, output_path):
        try:
            data = {
                'header': self.header,
                'objects': self.motion_objects
            }
            with open(output_path, 'w') as f:
                json.dump(data, f, indent=2)
            return True
        except Exception as e:
            print(f"Error saving JSON {output_path}: {e}")
            return False


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 anm_to_json.py <input.anm> [output.json]")
        sys.exit(1)
    
    in_file = sys.argv[1]
    out_file = sys.argv[2] if len(sys.argv) > 2 else in_file + ".json"
    
    conv = AnmConverter(in_file)
    if conv.parse():
        conv.save_json(out_file)
        print(f"Successfully converted to {out_file}")

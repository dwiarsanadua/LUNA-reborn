# Complete Reference: Luna-Plus-Old Architecture & Asset Pipeline

> **Document Type:** Technical Reference
> **Purpose:** Menjadi acuan lengkap untuk proses reverse-engineering dan rebuild ke Luna-Plus-Reborn
> **Coverage:** Asset Pipeline, Engine, Client, Server, Tools, Libraries, Scripts
> **Total Old Source:** ~7.798 file .cpp/.h, ~2.922.486 baris kode
> **Total Old Asset:** ~2,16 GB packed (9 .pak), ~20.276 texture, ~10.573 model, ~7.105 animasi

---

## Daftar Isi

1. [Asset Pipeline & Formats](#1-asset-pipeline--formats)
2. [Engine Architecture](#2-engine-architecture)
3. [Client Architecture](#3-client-architecture)
4. [Server Architecture](#4-server-architecture)
5. [Tools & Utilities](#5-tools--utilities)
6. [Libraries](#6-libraries)
7. [Scripts & Converters](#7-scripts--converters)
8. [Project Top-Level Structure](#8-project-top-level-structure)

---

## 1. Asset Pipeline & Formats

### 1.1 Packed Archives (PAK)

**Lokasi:** `LEGACY_ASSETS/packed_3ddata/`
**Total: 9 file .pak, ~2,16 GB**

| File | Ukuran | Isi |
|------|--------|-----|
| `character.pak` | 701 MB | Karakter (body, hair, costume, weapon) |
| `map.pak` | 542 MB | Data map (terrain, tile, object placement) |
| `image.pak` | 260 MB | UI images, icons, 2D assets |
| `monster.pak` | 218 MB | Model + data monster |
| `npcImage.pak` | 206 MB | NPC portrait images |
| `effect.pak` | 199 MB | Efek visual (partikel, cahaya) |
| `npc.pak` | 41 MB | Model NPC |
| `housing.pak` | 32 MB | Model housing/furniture |
| `farm.pak` | 11 MB | Model farming |

**Format PAK (binary, dari `unpack_pak.py`):**
```
[Global Header: 32 bytes]
  dwVersion(4), dwFileCount(4), dwFlag(4), dwCRC[4](16), reserved(4)

[Entry Header: 32 bytes each]
  dwTotalSize(4), dwRealSize(4), dwNameLen(4), dwAbsOffset(4),
  dwF1(4), dwF2(4), dwF3(4), dwF4(4)
  + nama file (EUC-KR, null-terminated)
```

### 1.2 Enkripsi (.bin files)

**Dual encryption scheme** dari `decrypt_luna.py`:

**Metode 1 — Legacy XOR:**
```
Header: dwVersion(4), dwType(4), dwDataSize(4), dwCRC1(1)
if version == (20040308 + dwType + dwDataSize):
    for i in range(len(data)):
        val = (val - (i & 0xFF)) & 0xFF
        if i % dwType == 0: val = (val - (dwType & 0xFF)) & 0xFF
```

**Metode 2 — AES-256-ECB:**
```
Key: E76B2413958B00E193A1 + padding 0x00 to 32 bytes
Decrypt via OpenSSL: openssl enc -aes-256-ecb -d -K <key> -nopad
Output: truncate to data_size, remove last byte (CRC2)
```

**Output:** `.bin.txt` (decrypted text/scripts)

### 1.3 Model Format (.mod)

**Parser:** `mod_to_obj.py` (233 lines)
**Total di Old:** 8.815 file .mod
**Lokasi:** `LEGACY_ASSETS/legacy_unpacked/raw_originals/assets/unpacked/`

**Binary Structure:**
```
FILE_SCENE_HEADER (28 bytes):
  dwVersion, dwObjectNum, dwMaterialNum, dwMaxMesh/Light/Camera/BoneNum

Materials (per material):
  OBJECT_TYPE(4) + SIZE(4) + FILE_MATERIAL_HEADER
  { dwTextureNum, diffuse/ambient/specular, transparansi,
    szDiffuseTexmapFileName[256], ... }

Objects (per object):
  OBJECT_TYPE(4) + SIZE(4) + [data spesifik tipe]

  OBJECT_TYPE_MESH = 0xF4000000 — vertices, UVs, normals, face groups, physique
  OBJECT_TYPE_BONE = 0xF5000000 — bone hierarchy
  OBJECT_TYPE_LIGHT = 0xF1000000 — light data
  OBJECT_TYPE_CAMERA = 0xF2000000 — camera + target
```

**Mesh Object:**
```
CMeshObject::ReadFile:
  max_v, v_num, orig_v, ext_v, tex_v, mtl_idx, fg_num, flag, grid
  vertices[v_num] : float[3] (XYZ)
  uvs[tex_v]      : float[2] (UV)
  indices[ext_v]  : uint32 (extra vertex indices)
  FaceGroups[fg_num]:
    { mtl_idx, fg_idx, f_num, max_f, v_idx_num, luv1, luv2 }
    indices[f_num] : uint16[3] per triangle
  Physique:
    header: pv_num, pb_num, pa_num
    physique_vertices[pv_num] : { bone_idx[4], weight[4] }
    bones[pb_num] : { name[64], parent, mat[12] }
```

**LOD System:** Naming convention `basename_N.mod` di mana N = 0,1,2 untuk 3 level detail.

### 1.4 Animation Format (.anm)

**Parser:** `anm_to_json.py` (115 lines)
**Total di Old:** 7.105 file .anm
**Ticks per frame:** 160 | **Frame speed:** 30 FPS

**Binary Structure:**
```
FILE_MOTION_HEADER (160 bytes):
  dwVersion(4), dwTicksPerFrame(4)=160, dwFirstFrame(4)=0,
  dwLastFrame(4), dwFrameSpeed(4)=30, dwMotionObjectNum(4),
  dwKeyFrameStep(4), szMotionName[128] (EUC-KR)

Per Motion Object:
  WORD type + WORD size
  FILE_MOTION_OBJECT_HEADER (152 bytes):
    dwIndex(4), dwRotKeyNum(4), dwPosKeyNum(4),
    dwScaleKeyNum(4), dwMorphKeyNum(4),
    szObjectName[128], dwFlag(4)

  Key Data:
    ROT_KEY[n]   = { dwTicks(4), dwFrame(4), QUATERNION(16) }     — 24 bytes
    POS_KEY[n]   = { dwTicks(4), dwFrame(4), VECTOR3(12) }        — 20 bytes
    SCALE_KEY[n] = { dwTicks(4), dwFrame(4), VECTOR3(12), ... }   — 36 bytes
```

### 1.5 Character Definition (.chx / .chr)

**Total di Old:** 1.542 file .chx
**Format:** Text-based dengan PID tokens:
```
*MOD_FILE_NAME <path.mod>
*MOTION_NUM <n> <motion1.anm> <motion2.anm> ...
*MATERIAL_NUM <n> <material1.mtl> ...
```

### 1.6 Texture Formats

| Format | Jumlah | Keterangan |
|--------|--------|------------|
| .dds | 10.169 | DirectDraw Surface — format utama |
| .png | 8.279 | Portable Network Graphics |
| .tif | 1.151 | Tagged Image Format |
| .tga | 677 | Truevision Targa |

**Engine loading:** Prefer DDS via `CreateTextureWithDDS`, fallback TGA via `CImage::LoadTga`, TIF via `CImage::LoadTIF`.

### 1.7 Terrain Format (.hfl + .map)

**Parser:** `map_converter.py` (93 lines), `hfl_to_raw.py` (missing but logic available in map_converter)

**Heightfield (.hfl — binary):**
- Header variable (64/128/256 bytes)
- Grid size: offset 0x64 = uint32 grid_x, uint32 grid_z
- Height data: offset 0x70 = float32 array row-major
- Grid cell = 10x10 world units

**Map Script (.map — text):**
```
GX_MAP
  STATIC_MODEL <file.stm>
  HEIGHT_FIELD <file.hfl>
GX_OBJECT <count>
  <model> <flags> <sx> <sy> <sz> <px> <py> <pz> <qx> <qy> <qz> <qw> <extra>
BOX_MAX <x y z>
BOX_MIN <x y z>
```

### 1.8 Audio Format

| Format | Jumlah | Lokasi |
|--------|--------|--------|
| .wav | 930 | SFX, monster, character, UI, weapon, vehicle |
| .mp3 | 48 | BGM (46 map tracks + login + event) |

**Audio Library:** Miles Sound System (MSS) — via `SoundLib.dll` / `mss32.dll`
- BGM via `HSTREAM` streaming
- SFX via `HSAMPLE` dengan 3D positioning (EAX 1/2/3, DS3D HW/SW)

### 1.9 Shader Format (D3D9)

**Lokasi:** `4DYUCHIGX_RENDER/`
**Shader Model:** ps.1.1 / vs.1.1

| File | Type | Fungsi |
|------|------|--------|
| `diff.psh` | Pixel | Diffuse + alpha test |
| `diffOpasity.psh` | Pixel | Sama dengan diff (opacity variant) |
| `diffDot3.psh` | Pixel | Per-pixel dot3 bump + specular |
| `diffDot3Reflect.psh` | Pixel | Dot3 bump + reflection map |
| `diffDot3Pix.vsh` | Vertex | Tangent space transform untuk dot3 |

### 1.10 Asset Pipeline Tools Summary

| Script | Fungsi | Input | Output |
|--------|--------|-------|--------|
| `unpack_pak.py` | Extract .pak archives | .pak | Raw files |
| `unpack_pak_v2.py` | Extract v2 .pak | .pak | Raw files |
| `decrypt_luna.py` | Decrypt .bin files | .bin | `.bin.txt` |
| `mod_to_obj.py` | Convert .mod to Wavefront | .mod | .obj + .mtl |
| `anm_to_json.py` | Convert .anm to JSON | .anm | .anm.json |
| `map_converter.py` | Convert .map + .hfl to scene | .map, .hfl | scene.json + terrain.obj |
| `batch_convert.py` | Orchestrate all conversions | — | — |
| `organize_modern_assets.py` | Organize assets into NEW_LUNA | Converted files | Organized dirs |
| `deep_organize.py` | Deep directory organization | Raw | Organized |
| `debug_mod.py` | Debug .mod header viewer | .mod | Console output |

---

## 2. Engine Architecture

### 2.1 Overview — 4Dyuchi Modular Engine

Engine terdiri dari 7 direktori terpisah yang membentuk **sistem plugin COM-like**:

| Modul | Nama DLL | Fungsi | File .cpp | LOC |
|-------|----------|--------|-----------|-----|
| **Render** | `Renderer.dll` | Direct3D 9 rendering | 48 | ~113.000 |
| **Geometry** | `Geometry.dll` | Model/motion/material loading | 32 | ~22.000 |
| **Executive** | `Executive.dll` | Scene graph, object lifecycle | 13 | ~11.000 |
| **FileStorage** | `FileStorage.dll` | Virtual file system (PAK) | 9 | ~2.700 |
| **GFunc** | `SS3DGFunc.dll` | Utility (memory pools, hash) | 27 | ~10.500 |
| **Network** | `Network.dll` | IOCP-based TCP networking | 60 | ~6.600 |
| **GXUtil** | — | Mouse/tool utilities | 2 | ~850 |

**Total Engine:** ~191 file, ~166.000 LOC

### 2.2 Rendering System

**Core File:** `4DYUCHIGX_RENDER/`
**Main Classes:**
- `CoD3DDevice` (2.981 lines) — implements `I4DyuchiGXRenderer`
- Wraps `IDirect3D9` + `IDirect3DDevice9`

**Device Creation Flow:**
1. Fullscreen via `ChangeDisplaySettings`
2. D3D9 device with `D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE`
3. Fallback: `D3DCREATE_SOFTWARE_VERTEXPROCESSING`
4. Resource manager (`CD3DResourceManager`) — 8192*4 bytes pool
5. Texture manager (`CTextureManager`) — MAX_TEXTURE_NUM, MAX_TEXBUCKET_NUM
6. VB/IB heap pools untuk 7 vertex formats
7. Font cache (1024x1024 atlas, 256 chars, 4 fonts)
8. Render queues: RenderQ, DPCQ, TextBuffer, SpriteBuffer, RenderTriBuffer

**Vertex Buffer Pools:**
| Pool | FVF | Size | Usage |
|------|-----|------|-------|
| VLMeshHeap | D3DFVF_VLVERTEX | 32.768 | Skinned meshes |
| VLSMeshHeap | D3DFVF_VLSVERTEX | 32.768 | Static lightmapped |
| PLMeshHeap | D3DFVF_D3DDUALVERTEX | 32.768 | Projection light |
| LMMeshHeap | D3DFVF_D3DDUALVERTEX | 32.768 | Light mapped |
| SpriteHeap | D3DFVF_D3DTLVERTEX | 1.024 | 2D sprites |
| ImmMeshHeap | D3DFVF_IVERTEX | 4.096 | Immediate triangles |
| Vector3Heap | D3DFVF_XYZ | 16.384 | Generic vectors |

**Lighting:** Up to `MAX_REALTIME_LIGHT_NUM` dynamic lights + `MAX_SPOT_LIGHT_NUM_PER_SCENE` spot lights.
**Shadow:** Built-in shadow map support via `BeginShadowMap`/`EndShadowMap`.
**Fog:** Configurable start/end distance + density + color.

### 2.3 Texture Management

**Core File:** `4DYUCHIGX_RENDER/texture_manager.cpp` (590 lines)

**Key Classes:**
- `CTextureManager` — hash-based texture cache
- `CTextureContainer` — single texture with filename, `LPDIRECT3DTEXTURE9`
- `CImage` — TGA/TIF loader

**Loading Flow:**
1. `AllocTexture(filename)` — strip path, lowercase, hash lookup
2. If miss: `AddTexture()` → detect extension (.tga/.tif)
3. `image.LoadTga()` or `image.LoadTIF()` → raw RGBA
4. `Set2PowValueLess()` — round down to power of 2
5. `CreateTextureWithBitmap()` → `D3DXCreateTexture()` or device `CreateTexture()`
6. Insert into hash table

**Defaults:** 1x1 white diffuse texture, generated bump map from diffuse.

### 2.4 Model/Geometry System

**Core File:** `4DyuchiGXGeometry/`

**Key Classes:**
- `CoGeometry` (2.374 lines) — geometry manager, hash-based model/motion cache
- `CoModel` (2.823 lines) — model container (meshes, bones, materials, LOD)
- `CMotion` (197 lines) — animation container (keyframe data)
- `CMotionObject` — single bone track with keyframe arrays
- `CMeshObject` — mesh with vertex data, face groups, physique
- `CBoneObject` — bone hierarchy node
- `CPhysique` — skinning weight computation
- `CBinTree` — binary tree for O(log n) keyframe lookup
- `CMaterial` — material with texture references (diffuse, reflect, bump)

**Model Loading (CoModel::ReadFile):**
1. Open file via `g_pFileStorage->FSOpenFile`
2. Read `FILE_SCENE_HEADER`
3. Read materials → `CMaterialList`
4. Loop objects → dispatch by `OBJECT_TYPE` (MESH/BONE/LIGHT/CAMERA)
5. Call `pObject->ReadFile()` for each
6. Close file
7. Load LOD variants if present (`basename_N.mod`)

**LOD System:** Up to 3 levels, checked via `g_pFileStorage->IsExistInFileStorage`.

### 2.5 Animation System

**Core File:** `4DyuchiGXGeometry/MOTION.cpp` (197 lines)

**Key Classes:**
- `CMotion` — animation container (COM-like ref counting)
- `CMotionObject` — single bone track

**Loading (CMotion::ReadFile):**
1. Read `FILE_MOTION_HEADER` (160 bytes)
2. Loop motion objects: read type + size + `FILE_MOTION_OBJECT_HEADER`
3. Read key arrays: ROT_KEY[], POS_KEY[], SCALE_KEY[], MORPH_KEY[]
4. Build hash table `m_pHashObjName` (object name → `CMotionObject*`)

**Interpolation (dari motion_obj.cpp):**
- Rotasi: quaternion slerp antara 2 keyframe terdekat
- Posisi: linear interpolation
- Scale: linear interpolation
- Keyframe lookup: binary tree (CBinTree) untuk O(log n)

### 2.6 File Storage System

**Core File:** `4DyuchiFileStorage/CoStorage.cpp` (1.280 lines)

**Key Classes:**
- `CoStorage` — implements `I4DyuchiFileStorage`
- `CPackFile` — single .pak archive handler
- `FSFILE_DESC` — file descriptor (offset, size, name)
- `FSFILE_POINTER` — open file handle (position, read mode)

**Access Methods:**
- `FILE_ACCESS_METHOD_ONLY_FILE` — OS filesystem only
- `FILE_ACCESS_METHOD_FILE_OR_PACK` — check PAK first, fallback to OS

**Key Functions:**
- `FSOpenFile` — open file from PAK or OS (474-554)
- `FSRead` — read from file (positional)
- `SScanf` — text-mode parsing from memory buffer
- `MapPackFile` — mount a .pak archive

### 2.7 Executive (Scene Graph)

**Core File:** `4DYUCHIGXEXECUTIVE/executive.cpp` (5.010 lines)

**Key Classes:**
- `CoExecutive` — implements `I4DyuchiGXExecutive`
- `CoGXObject` — scene object (model + motion + transform)
- `GXOBJECT_HANDLE` — handle-based access to objects
- `GXSchedulePROC` — per-object callback (event processing)

**CreateGXObject Flow (lines 1259-1355):**
1. Strip path, lowercase → hash lookup
2. Cache hit → duplicate handles, inc ref count
3. Cache miss → `LoadGXObject()` → parse .chr/.chx/.mod
4. `AddGXObject()` → allocate from pool, set schedule proc
5. Linked list per file desc (sharing)
6. Insert into hash table

### 2.8 Networking (Legacy)

**Core File:** `4DYUCHINET_Latest/` (61 files, 6.590 LOC)
**Arsitektur:** IOCP (I/O Completion Port) — Windows-specific

**Key Classes:**
- `CConnection` — TCP socket wrapper
- `COverlappedRecv` / `COverlappedSend` — overlapped I/O
- `CNetwork` — connection management
- `CPIO` — packet serialization

**Features:**
- `TCP_NODELAY` (Nagle disabled) — real-time game traffic
- Configurable max transfer sizes
- Send/recv buffer pools
- Connection management with accept/connect queues

### 2.9 Audio System

**Core File:** `SoundLib/MilesSoundLib.cpp` (1.340 lines)

**Library:** Miles Sound System (RAD Game Tools) via `mss32.dll`

**Key Classes:**
- `CMilesSoundLib` — implements `IMilesSoundLib`
- `CSoundEffect` / `CSoundEffectFile` — effect file management
- `CSoundSample` — 3D sample instance

**Driver Types:**
- EAX 3/2/1
- DirectSound3D Hardware/Software
- Miles Fast 2D
- Dolby Surround

**Speaker Support:** Stereo, Headphone, 4CH, 5.1, 7.1

**Features:**
- BGM streaming (48kHz/16-bit/32-voice)
- 3D positional audio (EAX reverb, distance attenuation, doppler)
- Sound effect pooling (hash-based dedup)
- Listener position/orientation transform

---

## 3. Client Architecture

### 3.1 Overview

**Lokasi:** `[Client]LUNA/`
**Total:** 311 .cpp + 320 .h file di root, 78 interface, 78 Effect, 22 Engine, 7 Audio, 8 input, 4 Trigger, 1 FreeImage
**Total baris:** ~243.703 (cpp+h)

### 3.2 Core Engine Wrapper

**File:** `Engine/GraphicEngine.cpp` (549 lines) + `.h`
**Arsitektur:** Singleton via global `g_pExecutive` pointer

**Initialization (CreateExecutive):**
1. `LoadLibrary("Executive.dll")` → `DllCreateInstance`
2. `InitializeFileStorageWithoutRegistry` — mount PAK files
3. `InitializeWithoutRegistry` — load Geometry.dll + Renderer.dll
4. Display config, 10.000 objects, 300 bones
5. Pre-create lights from `light.cfg`
6. Ambient color `0xaaaaaaaa`
7. Disable debug, set alpha ref to 5

**Global State:**
```cpp
I4DyuchiGXExecutive* g_pExecutive;
cPtrList GarbageObjectList;     // deferred deletion
cPtrList EffectPool;            // effect object cache
BOOL bRenderSky, g_bDisplayFPS;
int g_SlowCount, g_FastCount;   // performance monitoring
```

**Key Systems:**
- `ProcessGarbageObject()` — deferred deletion per frame
- `GetSelectedObject()` — screen-to-world picking (3 overloads)
- `LoadPack()` / `UnLoadPack()` — runtime PAK mounting
- Effect pooling via `AddPool()` / `GetObjectHandle()`

### 3.3 Main Game Loop

**File:** `Engine/MainGame.cpp` (407 lines)

**State Machine:** `eGAMESTATE_*`
```
MainTitle → ServerConnect → CharSelect → CharMake → GameIn → MapChange
```

**Init Sequence:**
1. `srand(GetTickCount())`
2. `DIRECTORYMGR->Init()`
3. Load 6 `.befl` effect files (per gender/race)
4. `Create CEngine` with display config
5. Parse `launcher.sav` for image paths

### 3.4 UI System

**Lokasi:** `interface/` (78 file .cpp+.h)
**Total baris:** ~23.289

**Key Classes:**
| Class | File | LOC | Fungsi |
|-------|------|-----|--------|
| `cDialog` | cDialog.cpp | 511 | Base UI dialog |
| `cWindow` | cWindow.cpp | — | Window container |
| `cWindowManager` | cWindowManager.cpp | 3.225 | Window management |
| `cButton` | cButton.cpp | — | Button widget |
| `cEditBox` | cEditBox.cpp | — | Text input |
| `cListCtrl` / `cListCtrlEx` | — | — | List controls |
| `cGridDialog` | — | — | Grid (inventory) |
| `cTabDialog` | — | — | Tab panel |
| `cComboBox` | — | — | Dropdown |
| `cImage` | cImage.cpp | — | Image display |
| `cFont` | cFont.cpp | — | Font rendering |
| `cScriptManager` | cScriptManager.cpp | **4.589** | UI script parser |
| `cResourceManager` | cResourceManager.cpp | — | Resource loading |

**UI Script Format (.bin.txt):**
UI layout didefinisikan dalam file teks yang di-parse oleh `cScriptManager`:
```
<WindowDef>
  ID, class, x, y, w, h, text, style, font, color
  <Child controls...>
</WindowDef>
```

### 3.5 Effect System

**Lokasi:** `Effect/` (78 file .cpp+.h)
**Total baris:** ~7.147

**Key Classes:**
- `CEffectManager` — central manager (singleton)
- `CEffect` — single effect instance (pooled)
- `CEffectDesc` — effect definition
- `CEffectUnit` — effect unit (particle, animation, billboard, light, sound, camera shake, model)
- `CDamageNumber` — floating damage text (pooled)
- `CEffectTrigger` — trigger system for effects

**Memory Pools:**
- `CEffect`: initial 100, grow 50
- `CDamageNumber`: initial 60, grow 40

**Gender/Race Dispatch:**
6 effect descriptor lists:
```
list_h_m.befl, list_h_w.befl (Human)
list_e_m.befl, list_e_w.befl (Elf)
list_d_m.befl, list_d_w.befl (Dark — added 2009)
```

### 3.6 Audio Manager

**File:** `Audio/MHAudioManager.cpp` (535 lines)
**Library:** Miles Sound System (via `IMilesSoundLib`)

**Key Features:**
- `PlayBGM()` — stream background music
- `Play()` — spawn 3D sound effect
- `SmoothChangeBGM()` — crossfade between BGM tracks
- `SetMasterVolume()`, `SetBGMVolume()`, `SetSoundVolume()`
- `LoadList()` — load sound definitions from file
- Sound container: `stdext::hash_map<SNDIDX, CSoundItem>`

### 3.7 Input System

**Lokasi:** `input/` (8 file)
**Files:** `Keyboard.cpp/.h`, `Mouse.cpp/.h`, `UserInput.cpp/.h`, `cIMEWnd.cpp/.h`

### 3.8 Game Objects (Root Level)

Key game object files (311 .cpp files in root):

| File | LOC | Fungsi |
|------|-----|--------|
| `Hero.cpp` | ~2.000 | Player character logic |
| `HeroMove.cpp` | — | Movement + pathfinding |
| `Monster.cpp` | — | Monster AI + state |
| `Npc.cpp` | — | NPC interaction |
| `Pet.cpp` | — | Pet system |
| `Item.cpp` | — | Item representation |
| `Player.cpp` | — | Other player representation |
| `Party.cpp` | — | Party management |
| `Guild.cpp` | — | Guild system |
| `Quest.cpp` | — | Quest tracking |
| `ChatManager.cpp` | — | Chat system |
| `SkillDialog.cpp` | — | Skill UI |
| `Inventory.cpp` | — | Inventory |

### 3.9 Effect Unit Types (Legacy)

Dari `Effect/` directory, terdapat 39 pasang file .cpp/.h untuk setiap unit type:

| Unit Type | Fungsi |
|-----------|--------|
| `AnimationEffectUnit` | Play sprite animation sequence |
| `CameraEffectUnit` | Camera shake/zoom |
| `DamageEffectUnit` | Damage number display |
| `LightEffectUnit` | Dynamic light effect |
| `MoveEffectUnit` | Object movement effect |
| `ObjectEffectUnit` | Attach 3D model as effect |
| `SoundEffectUnit` | Trigger sound |
| `EffectTrigger*Unit` | Various trigger events (On, Off, Move, Gravity, FadeOut, Camera, Link, Illusion, SetBaseMotion, Animate, Attach, Detach) |

---

## 4. Server Architecture

### 4.1 Overview

Server terbagi menjadi 3 komponen + shared modules:

| Komponen | File .cpp | LOC | Fungsi |
|----------|-----------|-----|--------|
| **Map Server** | 116 | ~121.657 | Game world simulation |
| **Agent Server** | 21 | ~21.662 | Login gateway, session |
| **Distribute Server** | 12 | ~7.863 | Channel, chat, routing |
| **Shared (CC modules)** | 87 | ~64.190 | Skill, Quest, Battle, dll. |

### 4.2 Map Server

**Lokasi:** `[Server]Map/` (116 .cpp files)
**Main Files:**

| File | LOC | Fungsi |
|------|-----|--------|
| `Player.cpp` | 5.545 | Player state + actions (60+ includes) |
| `Monster.cpp` | 1.421 | Monster AI (FSM) + state |
| `ItemManager.cpp` | **11.047** | Item generation, inventory, drop, upgrade |
| `QuestManager.cpp` | 513 | Quest state tracking |
| `Party.cpp` | — | Party management |
| `Guild.cpp` | — | Guild system |
| `AISystem.cpp` | — | Monster AI system |
| `Trigger/` (91 files) | — | Event trigger system |
| `Finite State Machine/` (61 files) | — | Generic FSM engine |

**Key Systems (dari `Player.cpp` includes):**
- Combat, Skills, Siege, Housing, Dungeon, PCRoom, Farm, Family, Party, Guild
- Anti-cheat: `HackCheck`, speed hack, teleport detection

**Monster AI State Machine:**
```
IDLE → PATROL → AGGRO → CHASE → ATTACK → FLEE → RETURN → IDLE
```
Boss: multi-phase dengan HP thresholds, enrage timer, special mechanics.

### 4.3 Cross-Cutting (CC) Modules

**Lokasi:** `[CC]*/` directories

| Module | Files | Fungsi |
|--------|-------|--------|
| `[CC]Skill/` | 49 | Skill system (client + server) |
| `[CC]Quest/` | 45 | Quest system (conditions, events, NPCs) |
| `[CC]BattleSystem/` | 22 | Battle (GTournament, Showdown) |
| `[CC]RecallInfo/` | 22 | NPC recall / siege recall |
| `[CC]ServerModule/` | 30 | Server infrastructure (DB, Network, Console) |
| `[CC]SiegeDungeon/` | 2 | Siege dungeon management |
| `[CC]Header/` | 41 | Common headers (AES, TargetList, curl, types) |

### 4.4 Database Layer

**Teknologi:** SQLite + PostgreSQL (via `[Lib]DBThread/`)
**Thread safety:** Queue-based async queries via `CDBThread`

**Database Files:**
| File | Ukuran | Isi |
|------|--------|-----|
| `game_data_legacy.db` | 20 MB | Item templates, monster stats, skill data, quest data |
| `luna_char.db` | 16 KB | Player characters |
| `luna_member.db` | 28 KB | User accounts |

---

## 5. Tools & Utilities

### 5.1 Development Tools

| Tool | Files | Fungsi |
|------|-------|--------|
| `[Tool]MapEditor` (4DyuchiGXMapEditor) | 85 | Terrain + object placement editor |
| `[Tool]PackingTool` | ~9 | PAK archive creator |
| `[Tool]NewPackingTool` | — | PAK v2 creator |
| `[Tool]Recovery` (client) | 141 | GM recovery tool (item/player) |
| `[Tool]Recovery` (server) | 8 | Server-side recovery |
| `[Tool]Regen` | 110 | Monster spawn editor |
| `[Tool]Dummy` | 35 | Dummy test client |
| `[Tool]AutoPatchToolWin32` | 11 | Auto-patcher |
| `[Monitoring]Server` | 20 | Server monitoring |
| `[Monitoring]Tool` | 40 | Monitoring client |
| `4DyuchiGXFileExtractor` | 9 | PAK extractor |

### 5.2 Support Tools

| Tool | Fungsi |
|------|--------|
| `[Client]Launcher` | Game launcher + auto-updater |
| `[Client]LUNALauncher` | Alternative launcher |
| `[Client]LogReporter` | Crash log reporter |
| `FileConverter` | File format converter |
| `ModelView` | 3D model viewer |
| `MtlExp` | Material exporter |
| `anmexp` | Animation exporter |
| `max_common` / `maxexp` | 3ds Max export utilities |

### 5.3 HSEOS (Extended Services)

**Lokasi:** `hseos/`
**Fungsi:** Sistem tambahan untuk fitur komunitas:
- `Date` — Date system
- `Family` — Family system
- `Farm` — Farming system
- `Group` — Group system
- `Monstermeter` — Monster HP meter
- `ResidentRegist` — Resident registration

---

## 6. Libraries

### 6.1 [Lib]BaseNetwork

**File:** ~32 .cpp + .h
**Fungsi:** Windows IOCP-based networking
**Key Classes:**
- `CBaseNetwork` — main network manager
- `CConnection` — TCP connection wrapper
- `CSessionManager` — session management
- `CPacketBuffer` — packet buffer management
- `CTcpListener` / `CTcpConnection` — TCP server/client

### 6.2 [Lib]DBThread

**File:** ~14 .cpp + .h
**Fungsi:** Thread-safe database access
**Support:** SQLite + PostgreSQL via `IDatabase` interface
**Key Classes:**
- `CDBThread` — async database worker thread
- `CDatabaseFactory` — database instance factory
- `CSQLiteDatabase` / `CPostgresDatabase` — concrete implementations

### 6.3 [Lib]YHLibrary

**File:** ~34 .cpp + .h
**Fungsi:** Utility library
**Key Classes:**
- `CEncryptor` — encryption utilities
- `CFileio` — file I/O
- `CMemoryPool` / `CMemoryPoolTempl` — memory pools
- `CHashTable` — hash table
- `CStrTokenizer` — string tokenizer
- `CArray`, `CPtrList`, `CStaticString` — data structures
- `CConstLinkedList`, `CLinkedList`, `CLooseLinkedList` — linked lists

### 6.4 [Lib]HSEL

**File:** ~5 file
**Fungsi:** HSEL scripting language (internal game scripting)

### 6.5 [Lib]ZipArchive

**File:** 58 file (termasuk bundled zlib)
**Fungsi:** Zip archive handling (via bundled zlib)
**Components:**
- `CZipArchive`, `CZipFile`, `CZipFileHeader` — ZIP API
- `CZipAutoBuffer`, `CZipMemFile`, `CZipStorage` — storage
- `zlib/` — bundled ZLIB compression (15 .c files)

### 6.6 SoundLib

**File:** 14 .cpp + .h
**Fungsi:** Miles Sound System wrapper

---

## 7. Scripts & Converters

### 7.1 Python Scripts (scripts_legacy/)

Total: 14 file (plus 1 subdirectory)

| Script | Lines | Fungsi |
|--------|-------|--------|
| `unpack_pak.py` | 71 | Extract .pak archives |
| `unpack_pak_v2.py` | — | Extract v2 .pak archives |
| `decrypt_luna.py` | 61 | Decrypt AES-256 + XOR |
| `unpack_luna.py` | — | Unpack Luna files |
| `unpack_all.py` | — | Unpack all archives |
| `unpack_all_assets.py` | — | Unpack all assets |
| `mod_to_obj.py` | 233 | Convert .mod → .obj/.mtl |
| `anm_to_json.py` | 115 | Convert .anm → .json |
| `map_converter.py` | 93 | Convert .map/.hfl → scene.json |
| `batch_convert.py` | — | Orchestrate all conversions |
| `debug_mod.py` | — | Debug .mod header |
| `deep_organize.py` | — | Deep organization |
| `organize_modern_assets.py` | — | Organize to NEW_LUNA |
| `convert_chx/` (subdir) | — | Convert .chx → glTF |

### 7.2 Legacy Script Files (LunaPlus/Data/Script/)

Total: ~4.199 file `.bin` (encrypted)
Decrypted: `.bin.txt` format

**Subdirectories:**
| Dir | Jumlah | Isi |
|-----|--------|-----|
| Root | 4.140 | Main game scripts |
| Mapset | 53 | Map-specific scripts |
| NPC | 3 | NPC scripts |
| Tutorial | 3 | Tutorial scripts |

---

## 8. Project Top-Level Structure

### Luna-Plus-Old Directory Map

```
Luna-Plus-Old/
│
├── [CC]BattleSystem/       ← cross-cutting battle (GTournament, Showdown)
├── [CC]Header/             ← shared headers (AES, TargetList, curl)
├── [CC]Quest/              ← quest system (45 files)
├── [CC]RecallInfo/         ← NPC recall + siege recall
├── [CC]ServerModule/       ← server module abstractions
├── [CC]SiegeDungeon/       ← siege dungeon management
├── [CC]Skill/              ← skill system (49 files, client + server)
│
├── [Client]LUNA/           ← MAIN CLIENT (311 .cpp + 320 .h + 155 subdir files)
│   ├── Audio/              ← Miles Sound System wrapper
│   ├── Effect/             ← Effect system (78 files)
│   ├── Engine/             ← GraphicEngine wrapper (22 files)
│   ├── FreeImage/          ← FreeImage library header
│   ├── Trigger/            ← Trigger system
│   ├── input/              ← Keyboard, Mouse, UserInput
│   └── interface/          ← UI framework (78 files)
│
├── [Client]LUNA/           ← 311 .cpp + 320 .h (root level game logic)
├── [Client]LUNALauncher/   ← Alternative launcher
├── [Client]Launcher/       ← Game launcher + auto-updater
├── [Client]LogReporter/    ← Crash log reporter
│
├── [Engine]Sln/            ← Visual Studio solution files
│
├── [Lib]BaseNetwork/       ← IOCP networking library
├── [Lib]DBThread/          ← Database thread library
├── [Lib]HSEL/              ← HSEL scripting
├── [Lib]YHLibrary/         ← Utility library
├── [Lib]ZipArchive/        ← Zip + zlib
│
├── [Monitoring]Server/     ← Server monitoring agent
├── [Monitoring]Tool/       ← Monitoring client
│
├── [Server]Agent/          ← Agent server (21 files, 21.6K LOC)
├── [Server]Distribute/     ← Distribute server (12 file, 7.8K LOC)
├── [Server]Map/            ← Map server (116 files, 121.6K LOC)
│   ├── Dungeon/            ← Dungeon processors
│   ├── Finite State Machine/ ← 61 files — generic FSM engine
│   └── Trigger/            ← 91 files — event trigger system
│
├── [Tool]AutoPatchToolWin32/
├── [Tool]Dummy/            ← Dummy test client
├── [Tool]MapEditor/        ← 4DyuchiGXMapEditor (85 files)
├── [Tool]NewPackingTool/   ← PAK v2 creator
├── [Tool]PackingTool/      ← PAK creator
├── [Tool]Recovery/         ← GM recovery tool (141+8 files)
├── [Tool]Regen/            ← Monster spawn editor (110 files)
│
├── 4DyuchiFilePack/        ← PAK packer tool
├── 4DyuchiFileStorage/     ← VFS library (9 files, 2.7K LOC)
├── 4DyuchiGRX_myself97/    ← GRX common library
├── 4DYUCHIGX_RENDER/       ← RENDERER (48 .cpp + 55 .h, 113K LOC)
├── 4DyuchiGX_UTIL/         ← Mouse/tool utilities
├── 4DYUCHIGXEXECUTIVE/     ← EXECUTIVE (13 .cpp, 11K LOC)
├── 4DYUCHIGXFILEEXTRACTOR/ ← PAK extractor tool
├── 4DyuchiGXGeometry/      ← GEOMETRY (32 .cpp, 22K LOC)
├── 4DyuchiGXGFunc/         ← GFUNC (27 .cpp, 10.5K LOC)
├── 4DyuchiGXMapEditor/     ← Map editor (85 files)
├── 4DYUCHINET_COMMON/      ← Network common types
├── 4DyuchiNET_Latest/      ← NETWORK library (61 files, 6.6K LOC)
│
├── LEGACY_ASSETS/          ← ~2.16 GB packed + unpacked game assets
│   ├── packed_3ddata/      ← 9 .pak files + SoundList.bin
│   └── legacy_unpacked/    ← Unpacked files
│
├── LunaGameDataExtracter/  ← Data extraction tool
├── LunaPlus/               ← Game client runtime
│   ├── Data/               ← Sound, Script, Interface, 3DData
│   └── System/             ← Setting.cfg, Camera.cfg
│
├── ModelView/              ← 3D model viewer
├── MtlExp/                 ← Material exporter
├── NEW_LUNA/               ← Converted/modernized assets (35.760 files)
│
├── PackExtract/            ← PAK extractor
├── SoundLib/               ← Miles Sound System wrapper
│
├── anmexp/                 ← Animation exporter
├── hseos/                  ← Extended OS services (Date, Family, Farm, dll)
│
├── legacy_root/            ← CMake-modernized codebase (1.115 .cpp files)
│   ├── src/                ← Ported source
│   │   ├── client/         ← 866 files, 244.5K LOC
│   │   ├── server/         ← 514 files, 146.8K LOC
│   │   ├── engine/         ← 402 files, 167K LOC (gx_render, gx_geom, dll)
│   │   ├── common/         ← 223 files, 62.2K LOC
│   │   └── tools/          ← 432 files, 120.7K LOC
│   └── shaders/            ← bgfx shaders (.sc + .bin)
│
├── lib/                    ← Prebuilt library binaries
├── max_common/             ← 3ds Max exporter common
├── maxexp/                 ← 3ds Max exporter
├── misc/                   ← Screenshots
└── scripts_legacy/         ← 14 Python conversion scripts
```

---

## 9. Asset Completeness Verification (Old vs Reborn)

### 9.1 Status per Kategori

Tabel berikut membandingkan jumlah asset di Luna-Plus-Old (sumber) vs Luna-Plus-Reborn (hasil konversi).
Verifikasi dilakukan dengan scan langsung terhadap file system.

| Kategori | Old (LEGACY) | Old (NEW_LUNA) | Reborn (assets/) | Coverage | Status |
|----------|-------------|----------------|-------------------|----------|--------|
| **Audio** (.wav/.mp3) | 978 | — | 978 | **100%** | ✅ |
| **Animasi** (.anm → .anm.json) | 7.105 | 7.033 | 7.033 | **99%** | ✅ |
| **Character Defs** (.chx → .json) | 1.542 | — | 1.457 | **94%** | ✅ |
| **Scene Maps** (.map → .json) | 53 | 53 | 53 | **100%** | ✅ |
| **Shaders** (D3D9 → bgfx .sc) | 5 | — | 17 | **100%** *(++ melebihi)* | ✅ |
| **Font** (.ttf) | 1 | — | 1 | **100%** | ✅ |
| **Database** (SQLite) | 3 | — | 3 | **100%** | ✅ |
| **Heightmaps** (.hfl → .hgt) | 51 | — | 34 | **67%** | ⚠️ |
| **Models** (.mod → .glb) | 8.832 | 8.742 | 809 | **9%** | 🔴 |
| **Textures** (.dds/.tif/.tga → .png) | 20.228 | 10.498 | 353 | **2%** | 🔴 |

### 9.2 Detail Kategori

**PAK Archives — 9 file, ~2,16 GB**
```
character.pak (701 MB)  → unpacked ✅
map.pak (542 MB)        → unpacked ✅
image.pak (260 MB)      → unpacked ✅
monster.pak (218 MB)    → unpacked ✅
npcImage.pak (206 MB)   → unpacked ✅
effect.pak (199 MB)     → unpacked ✅
npc.pak (41 MB)         → unpacked ✅
housing.pak (32 MB)     → unpacked ✅
farm.pak (11 MB)        → unpacked ✅
```
Unpacked directories: `audio/`, `interface/`, `scripts/`, `textures/`, `unpacked/`

**Textures — Gap terbesar (19.875 file belum di-convert)**
| Format | Old (LEGACY) | Reborn (.png) | Pipeline siap? |
|--------|-------------|---------------|----------------|
| .dds | 10.169 | — | ✅ `convert_textures.py` |
| .png | 8.279 | 353 | — |
| .tif | 1.151 | — | ✅ |
| .tga | 677 | — | ✅ |

**Models — Gap besar (~8.000 file belum di-convert)**
| Format | Old | Reborn |
|--------|-----|--------|
| .mod (LEGACY) | 8.815 | — |
| .obj (LEGACY) | 17 | — |
| .obj (NEW_LUNA) | 8.742 | — |
| .glb (Reborn) | — | 809 |

**Animasi — Hampir sempurna (99%)**
- 7.105 .anm di Old → 7.033 .anm.json di Reborn
- 72 file hilang (kemungkinan duplikat atau corrupt)

**Character Defs — 94%**
- 1.542 .chx di Old → 1.457 .json di Reborn
- 85 file belum terkonversi

**Heightmaps — 67%**
- 51 .hfl di Old → 34 .hgt di Reborn
- 17 file belum di-convert

### 9.3 Cara Verifikasi Mandiri

Jalankan perintah berikut di terminal untuk mendapatkan status terkini:

```bash
cd /Users/macbookair/PRIBADI/luna-plus-master

echo "=== VERIFIKASI ASSET ==="
echo "Textures : Old $(find Luna-Plus-Old/LEGACY_ASSETS -name '*.dds' -o -name '*.tif' -o -name '*.tga' 2>/dev/null | wc -l) → Reborn $(find Luna-Plus-Reborn/assets/textures/ -type f 2>/dev/null | wc -l)"
echo "Models   : Old $(find Luna-Plus-Old/LEGACY_ASSETS -name '*.mod' 2>/dev/null | wc -l) → Reborn $(find Luna-Plus-Reborn/assets/models/ -type f 2>/dev/null | wc -l)"
echo "Anims    : Old $(find Luna-Plus-Old/LEGACY_ASSETS -name '*.anm' 2>/dev/null | wc -l) → Reborn $(find Luna-Plus-Reborn/assets/animations/ -type f 2>/dev/null | wc -l)"
echo "Audio    : Old $(find Luna-Plus-Old/LEGACY_ASSETS -name '*.wav' -o -name '*.mp3' 2>/dev/null | wc -l) → Reborn $(find Luna-Plus-Reborn/assets/audio/ -type f 2>/dev/null | wc -l)"
echo "Heightmap: Old $(find Luna-Plus-Old/LEGACY_ASSETS -name '*.hfl' 2>/dev/null | wc -l) → Reborn $(find Luna-Plus-Reborn/assets/maps/ -name '*.hgt' 2>/dev/null | wc -l)"
echo "CharDefs : Old $(find Luna-Plus-Old/LEGACY_ASSETS -name '*.chx' 2>/dev/null | wc -l) → Reborn $(find Luna-Plus-Reborn/assets/characters/ -type f 2>/dev/null | wc -l)"
```

Output langsung menunjukkan berapa file yang sudah dan belum di-convert.

### 9.4 Prioritas Konversi

| Prioritas | Asset | Jumlah Tertinggal | Estimasi Waktu | Tools |
|-----------|-------|-------------------|----------------|-------|
| 🔴 **Tertinggi** | Textures (.dds/.tif/.tga → .png) | ~19.875 | 3-4 jam | `tools/asset_pipeline/convert_textures.py` |
| 🔴 **Tinggi** | Models (.mod → .glb) | ~8.000 | 2-3 jam | `tools/asset_pipeline/convert_models.py` |
| 🟡 **Sedang** | Heightmaps (.hfl → .hgt) | 17 | 10 menit | `tools/asset_pipeline/convert_heightmaps.py` |

Semua pipeline tools sudah siap di `Luna-Plus-Reborn/tools/asset_pipeline/`. Cukup jalankan:
```bash
cd Luna-Plus-Reborn/tools/asset_pipeline
python3 run.py --textures    # batch convert 19.875 textures
python3 run.py --models      # batch convert 8.000 models
python3 run.py --heightmaps  # batch convert 17 heightmaps
```

---

*Document generated from deep codebase audit. Updated: June 2026.*
*Total Old source: ~7.798 file .cpp/.h, ~2.922.486 baris kode*
*Total Old asset: ~2,16 GB packed, ~20.276 tekstur, ~10.573 model, ~7.105 animasi, ~1.542 karakter*
*Asset verification data is real-time and can be re-generated via commands in Section 9.3*

# Assets Inventory
Generated: 2026-06-08

| Category | Count | Format |
|----------|-------|--------|
| Textures | 353 | .png |
| Models | 8,365 | .obj (7,556) + .glb (809) |
| Animations | 7,033 | .anm.json |
| Audio | 978 | .wav (930) + .mp3 (48) |
| Heightmaps | 34 | .hgt |
| Scene Maps | 53 | .json |
| Character Defs | 1,457 | .json |
| Shaders (compiled) | 9 | .bin |
| Shaders (source) | 17 | .sc |
| Fonts | 1 | .ttf |
| Data | 4 | .db |

**Total files: 27,431**
**Total size: ~4.8 GB**

## Directory Structure

```
assets/
├── textures/       — 353 .png (ui, environment, characters)
├── models/         — 8,365 files
│   ├── character/  — 6,003 .obj (legacy body parts/costumes)
│   ├── monster/    — 410 .obj + 318 .glb
│   ├── npc/        — 138 .obj + 96 .glb
│   ├── effect/     — .obj + 16 .glb
│   ├── farm/       — .obj (farm props)
│   ├── housing/    — .obj + .glb
│   ├── pet/        — 57 .glb
│   ├── vehicle/    — 68 .glb
│   └── prop/       — 254 .glb
├── animations/     — 7,033 .anm.json (converted from legacy .anm)
├── audio/          — 978 .wav/.mp3
├── maps/           — 34 .hgt + 53 .json scene files
├── characters/     — 1,457 .json character definitions
├── shaders/        — 9 .bin + 17 .sc
├── fonts/          — 1 .ttf
├── interface/      — (empty — UI textures in textures/)
├── scripts/        — (empty)
└── data/           — 4 .db files
```

## Pipeline Steps
1. **Textures** — Converted `.dds`/`.tif`/`.tga` → `.png`
2. **Models** — Converted `.mod` → `.obj`; organized `.glb` into categories
3. **Animations** — Parsed legacy `.anm` binary → `.anm.json` (JSON keyframe format)
4. **Heightmaps** — Converted `.hfl` → `.hgt` (ASCII grid)
5. **Maps** — Parsed `.map` scripts → `scene.json`
6. **Character Defs** — Parsed `.chx` → `.json`
7. **Validation** — Completeness check, no issues

# LAST-01 — Shader Rewrite: PBR → DX9 Fixed-Function Emulation

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Rewrite shader dari PBR (metalness/roughness/IBL) ke DX9 fixed-function emulation (ambient + directional + specular + fog). Ini adalah penyebab utama "rasa" rendering berbeda. bgfx support shader apapun — kita akan tulis shader yang mengemulasi DX9 fixed-function pipeline.

## Aturan Ketat

1. BACA shader existing — pahami uniforms dan input
2. ✅ Jika shader sudah fixed-function style — skip
3. 🔧 Jika masih PBR — rewrite ke Blinn-Phong fixed-function
4. Build verify — 0 error

## File yang harus dicek

```bash
# Cari shader files
ls shaders/*.sc shaders/vs_* shaders/fs_* 2>/dev/null
# Cari uniform definitions
rg "uniform\|sampler\|bgfx::setUniform\|bgfx::createUniform" engine/gx_render/ --type cpp
# Cari bagaimana shader di-load
rg "createProgram\|loadShader\|createShader" engine/gx_render/ --type cpp -l
```

## Yang harus diubah

### 1. Vertex Shader (vs_main.sc) — DX9 Fixed-Function Emulation

```c
$input a_position, a_normal, a_texcoord0, a_color0
$output v_color, v_texcoord0, v_fog

#include <bgfx_shader.sh>

// DX9 fixed-function uniforms
uniform vec4 u_ambient;      // (r,g,b,1.0) — scene ambient light
uniform vec4 u_light_dir;    // (x,y,z,0.0) — directional light direction
uniform vec4 u_light_diffuse; // (r,g,b,1.0) — light color
uniform vec4 u_light_specular; // (r,g,b,power) — specular color + power
uniform vec4 u_fog;          // (color.r,color.g,color.b,start) — fog params
uniform vec4 u_fog_end;      // (1/range, 0, 0, 0)

void main() {
    vec3 normal = normalize(mul(u_modelViewMat, vec4(a_normal, 0.0)).xyz);
    vec3 light_dir = normalize(u_light_dir.xyz);
    vec3 view_dir = normalize(-mul(u_modelViewMat, vec4(a_position, 1.0)).xyz);
    
    // Diffuse (Lambertian)
    float NdotL = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = u_light_diffuse.rgb * NdotL;
    
    // Specular (Blinn-Phong)
    vec3 half_vec = normalize(light_dir + view_dir);
    float NdotH = max(dot(normal, half_vec), 0.0);
    float spec_power = u_light_specular.a * 255.0; // DX9 power stored in alpha
    vec3 specular = u_light_specular.rgb * pow(NdotH, spec_power);
    
    // Ambient
    vec3 ambient = u_ambient.rgb;
    
    // Final color = ambient + diffuse + specular
    v_color = vec4(ambient + diffuse + specular, 1.0);
    v_texcoord0 = a_texcoord0;
    
    // Fog (linear)
    float z = gl_Position.w;
    float fog_factor = clamp((u_fog.w - z) * u_fog_end.x, 0.0, 1.0);
    v_fog = vec4(u_fog.rgb, fog_factor);
    
    gl_Position = mul(u_modelViewProjMat, vec4(a_position, 1.0));
}
```

### 2. Fragment Shader (fs_main.sc)

```c
$input v_color, v_texcoord0, v_fog

#include <bgfx_shader.sh>

SAMPLER2D(s_texColor, 0);

void main() {
    vec4 tex_color = texture2D(s_texColor, v_texcoord0);
    // DX9: modulate = texture × vertex color
    vec4 color = tex_color * v_color;
    // Alpha test (threshold 0.5f seperti DX9)
    if (color.a < 0.5) discard;
    // Fog lerp
    color.rgb = mix(u_fog.rgb, color.rgb, v_fog.a);
    gl_FragColor = color;
}
```

### 3. Renderer — Set DX9-style uniforms

Di C++ code (engine/gx_render/RenderDevice.cpp atau RenderDevice.h):
```cpp
// Set ambient light (match Old DX9 default)
float ambient[4] = { 0.2f, 0.2f, 0.3f, 1.0f };
bgfx::setUniform(u_ambient, ambient);

// Set directional light (match Old: 45° atas-kiri)
float light_dir[4]   = { 0.5f, -0.8f, 0.3f, 0.0f };
float light_diff[4]  = { 0.8f, 0.8f, 0.8f, 1.0f };
float light_spec[4]  = { 1.0f, 1.0f, 1.0f, 16.0f }; // spec power = 16
bgfx::setUniform(u_light_dir, light_dir);
bgfx::setUniform(u_light_diffuse, light_diff);
bgfx::setUniform(u_light_specular, light_spec);

// Set fog (match Old: linear, dark, range=200)
float fog[4]     = { 0.0f, 0.0f, 0.0f, 50.0f };  // color=hitam, start=50
float fog_end[4] = { 1.0f / 150.0f, 0, 0, 0 };     // 1/range
bgfx::setUniform(u_fog, fog);
bgfx::setUniform(u_fog_end, fog_end);
```

### 4. Cek dari Old DX9 setting

```bash
# Cari DX9 light/material settings dari Old codebase
rg "D3DLIGHT9\|D3DMATERIAL9\|SetLight\|LightEnable\|SetRenderState.*AMBIENT\|D3DRS_AMBIENT" \
  /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Old/4DYUCHIGX_RENDER/ --type cpp -l
```

## Verifikasi

Setelah selesai: `cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)` — 0 error.

## ✅ Kembalikan: "LAST-01 done: shader rewritten from PBR to DX9 fixed-function emulation"

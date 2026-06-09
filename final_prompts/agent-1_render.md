# AGENT-1 — Rendering: Shader + Lighting (merger LAST-01 + FINAL-01)

Lokasi: /Users/macbookair/PRIBADI/luna-plus-master/Luna-Plus-Reborn
Build: cmake --build build/macos-debug -j$(sysctl -n hw.ncpu)

## Tugas

Rewrite shader dari PBR ke DX9 fixed-function emulation + set light/fog uniforms. Ini penyebab utama "rasa" rendering berbeda.

## Aturan Ketat

1. BACA file shader existing sebelum ubah — pahami format `.sc`
2. ✅ Jika shader sudah fixed-function style → skip, jangan sentuh
3. 🔧 Jika masih PBR → rewrite dengan kode di bawah
4. Build verify — 0 error

## File target (tidak ada conflict dengan agent lain)

- `shaders/vs_main.sc` — vertex shader (BARU atau OVERWRITE)
- `shaders/fs_main.sc` — fragment shader (BARU atau OVERWRITE)
- `engine/gx_render/RenderDevice.cpp` atau `.h` — set uniforms

### 1. Vertex Shader: `shaders/vs_main.sc`

```c
$input a_position, a_normal, a_texcoord0, a_color0
$output v_color, v_texcoord0, v_fog

#include <bgfx_shader.sh>

uniform vec4 u_ambient;
uniform vec4 u_light_dir;
uniform vec4 u_light_diffuse;
uniform vec4 u_light_specular;
uniform vec4 u_fog;
uniform vec4 u_fog_end;

void main() {
    vec3 normal = normalize(mul(u_modelViewMat, vec4(a_normal, 0.0)).xyz);
    vec3 light_dir = normalize(u_light_dir.xyz);
    vec3 view_dir = normalize(-mul(u_modelViewMat, vec4(a_position, 1.0)).xyz);
    float NdotL = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = u_light_diffuse.rgb * NdotL;
    vec3 half_vec = normalize(light_dir + view_dir);
    float NdotH = max(dot(normal, half_vec), 0.0);
    float spec_power = u_light_specular.a * 255.0;
    vec3 specular = u_light_specular.rgb * pow(NdotH, spec_power);
    v_color = vec4(u_ambient.rgb + diffuse + specular, 1.0);
    v_texcoord0 = a_texcoord0;
    float z = gl_Position.w;
    float fog_factor = clamp((u_fog.w - z) * u_fog_end.x, 0.0, 1.0);
    v_fog = vec4(u_fog.rgb, fog_factor);
    gl_Position = mul(u_modelViewProjMat, vec4(a_position, 1.0));
}
```

### 2. Fragment Shader: `shaders/fs_main.sc`

```c
$input v_color, v_texcoord0, v_fog
#include <bgfx_shader.sh>
SAMPLER2D(s_texColor, 0);
void main() {
    vec4 tex_color = texture2D(s_texColor, v_texcoord0);
    vec4 color = tex_color * v_color;
    if (color.a < 0.5) discard;
    color.rgb = mix(vec3(0,0,0), color.rgb, v_fog.a);
    gl_FragColor = color;
}
```

### 3. RenderDevice — Set DX9-style uniforms

Di `engine/gx_render/RenderDevice.cpp` atau file yang relevan, cari fungsi yang set shader uniforms dan tambah:
```cpp
float ambient[4]   = {0.2f, 0.2f, 0.3f, 1.0f};
float light_dir[4] = {0.5f, -0.8f, 0.3f, 0.0f};
float light_diff[4]= {0.8f, 0.8f, 0.8f, 1.0f};
float light_spec[4]= {0.6f, 0.6f, 0.6f, 16.0f};
float fog[4]       = {0.0f, 0.0f, 0.0f, 50.0f};
float fog_end[4]   = {1.0f/150.0f, 0, 0, 0};
bgfx::setUniform(u_ambient, ambient);
bgfx::setUniform(u_light_dir, light_dir);
bgfx::setUniform(u_light_diffuse, light_diff);
bgfx::setUniform(u_light_specular, light_spec);
bgfx::setUniform(u_fog, fog);
bgfx::setUniform(u_fog_end, fog_end);
```

## ✅ Kembalikan: "AGENT-1 done: shader rewritten + lighting set, 0 build errors"

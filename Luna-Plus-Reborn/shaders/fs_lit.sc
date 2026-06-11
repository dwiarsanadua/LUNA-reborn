$input v_texcoord0, v_texcoordSplat, v_normal, v_color0, v_worldPos

#include <bgfx_shader.sh>

SAMPLER2D(s_texColor, 0);
uniform vec4 u_lightDir;
uniform vec4 u_color;
uniform vec4 u_ambient;

void main() {
    vec4 texel = texture2D(s_texColor, v_texcoord0);
    
    // Alpha Test (PENTING: Luna menggunakan alpha test untuk rambut/baju)
    if (texel.a < 0.5) {
        discard;
    }
    
    vec3 N = normalize(v_normal);
    vec3 L = normalize(-u_lightDir.xyz);
    
    // Ambient dari Uniform (atau default jika tidak diset)
    float ambient = 0.5; 
    
    // Diffuse - Menggunakan Half-Lambert agar karakter lebih cerah (style Luna)
    float diff = dot(N, L) * 0.5 + 0.5;
    
    vec3 finalLight = (ambient + diff) * u_color.rgb;
    
    // Output: Texture * Lighting * VertexColor
    gl_FragColor = vec4(texel.rgb * finalLight * v_color0.rgb, texel.a * v_color0.a);
}

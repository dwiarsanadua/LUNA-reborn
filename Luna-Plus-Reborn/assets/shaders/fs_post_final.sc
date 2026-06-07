$input v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(s_texColor, 0); // Scene
SAMPLER2D(s_texBloom, 1); // Bloom

void main() {
    vec4 scene = texture2D(s_texColor, v_texcoord0);
    vec4 bloom = texture2D(s_texBloom, v_texcoord0);
    
    // Additive blend for bloom
    vec3 color = scene.rgb + bloom.rgb * 0.8;
    
    // Tone mapping (Simple Reinhard)
    color = color / (color + vec3_splat(1.0));
    
    // Gamma correction
    color = pow(color, vec3_splat(1.0/2.2));
    
    gl_FragColor = vec4(color, 1.0);
}

$input v_texcoord0, v_normal, v_color0

#include <bgfx_shader.sh>

SAMPLER2D(s_texColor, 0);
uniform vec4 u_time; // x = time

void main() {
    float t = u_time.x;
    vec2 uv = v_texcoord0;
    
    // Wave distortion
    uv.x += sin(uv.y * 10.0 + t) * 0.02;
    uv.y += cos(uv.x * 10.0 + t) * 0.02;
    
    vec4 texel = texture2D(s_texColor, uv);
    
    // Add blue tint and foam
    vec3 waterCol = mix(vec3(0.1, 0.4, 0.8), texel.rgb, 0.5);
    float foam = max(0.0, sin(uv.x * 50.0 + t * 2.0) * cos(uv.y * 50.0 + t * 2.0));
    waterCol += foam * 0.1;
    
    gl_FragColor = vec4(waterCol, 0.7); // Semi-transparent
}

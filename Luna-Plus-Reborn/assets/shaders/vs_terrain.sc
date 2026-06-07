$input a_position, a_texcoord0, a_color0
$output v_texcoord0, v_texcoordSplat, v_normal, v_color0, v_worldPos

#include <bgfx_shader.sh>

void main() {
    vec4 worldPos = mul(u_model[0], vec4(a_position, 1.0));
    gl_Position = mul(u_viewProj, worldPos);
    
    v_texcoord0 = a_texcoord0;
    v_texcoordSplat = a_texcoord0;
    v_normal = vec3(0.0, 1.0, 0.0); // Simplified for terrain for now
    v_color0 = a_color0;
    v_worldPos = worldPos.xyz;
}

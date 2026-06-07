$input a_position, a_texcoord0, a_normal, a_color0, a_indices, a_weight
$output v_texcoord0, v_texcoordSplat, v_normal, v_color0, v_worldPos

#include <bgfx_shader.sh>

uniform mat4 u_bones[64];

void main() {
    ivec4 idx = ivec4(a_indices);
    vec4 pos = vec4(a_position, 1.0);
    vec3 normal = a_normal;
    
    vec4 skinnedPos =
        (mul(u_bones[idx.x], pos) * a_weight.x) +
        (mul(u_bones[idx.y], pos) * a_weight.y) +
        (mul(u_bones[idx.z], pos) * a_weight.z) +
        (mul(u_bones[idx.w], pos) * a_weight.w);
        
    vec3 skinnedNormal =
        (mul(u_bones[idx.x], vec4(normal, 0.0)).xyz * a_weight.x) +
        (mul(u_bones[idx.y], vec4(normal, 0.0)).xyz * a_weight.y) +
        (mul(u_bones[idx.z], vec4(normal, 0.0)).xyz * a_weight.z) +
        (mul(u_bones[idx.w], vec4(normal, 0.0)).xyz * a_weight.w);
        
    gl_Position = mul(u_viewProj, skinnedPos);
    v_texcoord0 = a_texcoord0;
    v_texcoordSplat = vec2(0.0, 0.0);
    v_normal = mul(u_model[0], vec4(skinnedNormal, 0.0)).xyz;
    v_color0 = a_color0;
    v_worldPos = skinnedPos.xyz;
}

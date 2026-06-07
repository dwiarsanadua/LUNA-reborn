$input a_position, a_texcoord0, a_color0
$output v_texcoord0, v_texcoordSplat, v_normal, v_color0, v_worldPos

#include <bgfx_shader.sh>

void main() {
    gl_Position = vec4(a_position, 1.0);
    v_texcoord0 = a_texcoord0;
    v_texcoordSplat = vec2(0.0, 0.0);
    v_normal = vec3(0.0, 0.0, 1.0);
    v_color0 = a_color0;
    v_worldPos = a_position;
}

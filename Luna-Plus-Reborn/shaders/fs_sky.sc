$input v_texcoord0, v_texcoordSplat, v_normal, v_color0, v_worldPos

#include <bgfx_shader.sh>

SAMPLER2D(s_texSky, 0);

void main() {
    vec4 texel = texture2D(s_texSky, v_texcoord0);
    gl_FragColor = texel * v_color0;
}

$input v_color, v_texcoord0, v_fog

#include <bgfx_shader.sh>

uniform vec4 u_fog;
SAMPLER2D(s_texColor, 0);

void main() {
    vec4 tex_color = texture2D(s_texColor, v_texcoord0);
    vec4 color = tex_color * v_color;
    if (color.a < 0.5) discard;
    color.rgb = lerp(u_fog.rgb, color.rgb, v_fog.a);
    gl_FragColor = color;
}

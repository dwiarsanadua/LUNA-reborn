$input v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(s_texColor, 0);
uniform vec4 u_blurData; // x=dx, y=dy

void main() {
    vec2 uv = v_texcoord0;
    vec2 offset = u_blurData.xy;
    
    vec4 color = texture2D(s_texColor, uv) * 0.227027;
    color += texture2D(s_texColor, uv + offset * 1.384615) * 0.316216;
    color += texture2D(s_texColor, uv - offset * 1.384615) * 0.316216;
    color += texture2D(s_texColor, uv + offset * 3.230769) * 0.070270;
    color += texture2D(s_texColor, uv - offset * 3.230769) * 0.070270;
    
    gl_FragColor = color;
}

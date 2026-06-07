$input v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(s_texColor, 0);

void main() {
    vec4 texel = texture2D(s_texColor, v_texcoord0);
    // Luminance threshold for bloom
    float lum = dot(texel.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (lum > 0.8) {
        gl_FragColor = texel;
    } else {
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}

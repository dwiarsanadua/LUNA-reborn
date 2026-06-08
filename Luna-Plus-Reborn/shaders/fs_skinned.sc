$input v_texcoord0, v_texcoordSplat, v_normal, v_color0, v_worldPos

#include <bgfx_shader.sh>

SAMPLER2D(s_texColor, 0);
uniform vec4 u_lightDir;
uniform vec4 u_color;

void main() {
    vec4 texel = texture2D(s_texColor, v_texcoord0);

    vec3 N = normalize(v_normal);
    vec3 L = normalize(-u_lightDir.xyz);
    vec3 V = normalize(-v_worldPos);

    float ambient = 0.45;
    float diff = max(0.0, dot(N, L));
    vec3 H = normalize(L + V);
    float spec = pow(max(0.0, dot(N, H)), 32.0) * 0.3;
    float rim = 1.0 - max(0.0, dot(N, V));
    rim = pow(rim, 3.0) * 0.4;

    vec3 finalLight = (ambient + diff * 0.55 + spec + rim) * u_color.rgb;
    gl_FragColor = vec4(texel.rgb * finalLight * v_color0.rgb, texel.a * v_color0.a);
}

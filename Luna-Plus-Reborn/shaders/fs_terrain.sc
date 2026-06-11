$input v_texcoord0, v_texcoordSplat, v_normal, v_color0, v_worldPos

#include <bgfx_shader.sh>

SAMPLER2D(s_texColor, 0);
SAMPLER2D(s_texGrass, 1);
SAMPLER2D(s_texRock, 2);
SAMPLER2D(s_texDirt, 3);

uniform vec4 u_lightDir;
uniform vec4 u_fogData; 
uniform vec4 u_fogColor;

void main() {
    // Large scale tiling for base, smaller for details
    vec4 texBase  = texture2D(s_texColor, v_texcoord0 * 0.1);
    vec4 texGrass = texture2D(s_texGrass, v_texcoord0 * 10.0);
    vec4 texRock  = texture2D(s_texRock,  v_texcoord0 * 8.0);
    vec4 texDirt  = texture2D(s_texDirt,  v_texcoord0 * 12.0);

    // Splatting based on vertex color
    float wGrass = v_color0.r;
    float wRock  = v_color0.g;
    float wDirt  = v_color0.b;
    float wBase  = max(0.0, 1.0 - (wGrass + wRock + wDirt));

    vec4 texel = (texGrass * wGrass) + (texRock * wRock) + (texDirt * wDirt) + (texBase * wBase);
    
    vec3 N = normalize(v_normal);
    vec3 L = normalize(-u_lightDir.xyz);
    
    // Use Half-Lambert for bright, soft terrain shadows (Luna Old style)
    float diff = dot(N, L) * 0.5 + 0.5;
    vec4 color = texel * diff;

    // Distance fog
    float dist = length(v_worldPos);
    float fogFactor = clamp((dist - u_fogData.x) / (u_fogData.y - u_fogData.x), 0.0, 1.0);
    gl_FragColor = mix(color, u_fogColor, fogFactor * u_fogData.z);
}

$input a_position, a_normal, a_texcoord0, a_color0
$output v_color, v_texcoord0, v_fog

#include <bgfx_shader.sh>

uniform vec4 u_ambient;
uniform vec4 u_light_dir;
uniform vec4 u_light_diffuse;
uniform vec4 u_light_specular;
uniform vec4 u_fog;
uniform vec4 u_fog_end;

void main() {
    vec4 wpos = mul(u_model[0], vec4(a_position, 1.0));
    vec3 normal = normalize(mul(u_model[0], vec4(a_normal, 0.0)).xyz);
    vec3 light_dir = normalize(u_light_dir.xyz);
    vec3 view_dir = normalize(-wpos.xyz);
    float NdotL = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = u_light_diffuse.rgb * NdotL;
    vec3 half_vec = normalize(light_dir + view_dir);
    float NdotH = max(dot(normal, half_vec), 0.0);
    float spec_power = u_light_specular.a * 255.0;
    vec3 specular = u_light_specular.rgb * pow(NdotH, spec_power);
    v_color = vec4(u_ambient.rgb + diffuse + specular, 1.0);
    v_texcoord0 = a_texcoord0;
    float z = length(wpos.xyz);
    float fog_factor = clamp((u_fog.w - z) * u_fog_end.x, 0.0, 1.0);
    v_fog = vec4(u_fog.rgb, fog_factor);
    gl_Position = mul(u_viewProj, wpos);
}

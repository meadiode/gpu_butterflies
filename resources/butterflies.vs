#version 300 es
precision highp float;
precision highp int;

#define PI 3.141592653589793
#define DEG2RAD (PI / 180.0)


in vec3 vertexPosition;
in vec3 vertexNormal;
in vec2 vertexTexCoord;

uniform mat4 mvp;

uniform float time;
uniform int nx;
uniform int ny;
uniform int nz;
uniform vec2 phi;
uniform vec3 scale;
uniform vec3 offset;
uniform float dispersion;
uniform vec4 color1;
uniform vec4 color2;
uniform float distribution;

out vec2 fragTexCoord;
out vec4 fragColor;
flat out int tex_id;

mat4 transform(vec3 pos, vec3 angles, vec3 scale)
{
    mat4 m = mat4(1.0);
    
    m[0][0] = scale.x;
    m[1][1] = scale.y;
    m[2][2] = scale.z;

    vec3 cs = cos(angles);
    vec3 sn = sin(angles);

    mat4 rx = mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0,  cs.x, -sn.x, 0.0,
        0.0,  sn.x,  cs.x, 0.0,
        0.0, 0.0, 0.0, 1.0
    );

    mat4 ry = mat4(
         cs.y, 0.0,  sn.y, 0.0,
        0.0, 1.0, 0.0, 0.0,
        -sn.y, 0.0,  cs.y, 0.0,
        0.0, 0.0, 0.0, 1.0
    );

    mat4 rz = mat4(
         cs.z, -sn.z, 0.0, 0.0,
         sn.z,  cs.z, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );

    m = rz * ry * rx * m;

    m[3][0] = pos.x;
    m[3][1] = pos.y;
    m[3][2] = pos.z;

    return m;
}


vec2 calc_yaw_pitch(vec3 target)
{
    float xz_len = sqrt(target.x * target.x + target.z * target.z);
    float yaw = atan(target.z, target.x);
    float pitch;

    if (xz_len < 0.0001) /* Near zero, straight up/down */
    {
        pitch = target.y > 0.0 ? PI / 2.0 : -PI / 2.0;
        yaw = 0.0;
    }
    else
    {
        pitch = atan(target.y, xz_len);
    }

    return vec2(yaw, pitch);
}


float rand(vec2 co)
{ 
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}


vec3 knot(float t, vec3 local_offset)
{
    vec3 res = vec3(float(nx), float(ny), float(nz)) * t;
    res += vec3(phi, 0.0);
    res = cos(res);
    res = res * scale + offset + local_offset;

    return res;
}

void main()
{
    float f_id = float(gl_InstanceID);
    float t_offset = -PI + rand(vec2(f_id * 0.0069, f_id * 0.00042069)) * PI * 2.0;
    
    /* local offset */
    float lo_theta = -PI + rand(vec2(f_id * 0.0001, 4.2)) * PI * 2.0;
    float lo_phi = -PI + rand(vec2(f_id * 0.0022, 42.0)) * PI * 2.0;
    vec3 loffset = vec3(sin(lo_theta) * cos(lo_phi),
                        sin(lo_theta) * sin(lo_phi),
                        cos(lo_theta));
    loffset *= dispersion;

    /* Butterfly speed variation */
    float speed = rand(vec2(f_id * 0.0004, 69.69));

    /* Butterfly size variation - depends on the speed - the bigger the slower */
    float butt_size = mix(1.0, 0.0, speed);

    /* Make large and small butterflies also differ in color */
    tex_id = butt_size > distribution ? 0 : 1;

    /* Some shade variation */
    float shade_var = rand(vec2(f_id * 0.0001337, 13.37));
    shade_var = mix(0.6, 1.3, shade_var);
    fragColor = vec4(vec3(shade_var), 1.0);

    speed += 0.5; /* speed is in range 0.5 - 1.5 */
    butt_size = 0.5 + butt_size * 2.0; /* size is in range 0.5 - 2.5 */

    /* Flapping animation */
    vec4 vpos = vec4(vertexPosition, 1.0);
    float px = vpos.x;
    float wing_a = sin((time + t_offset) * 600.0 * speed) * sign(px);
    vpos.x = cos(wing_a) * px;
    vpos.z = sin(wing_a) * px;

    /* Align to heading direction */
    vec3 pos = knot(time * speed + t_offset, loffset);
    vec3 ppos = knot(time * speed + t_offset - 0.0001, loffset);
    vec3 dir = normalize(pos - ppos);
    vec3 euler = vec3(calc_yaw_pitch(dir), 0.0);

    /* Apply final transform */
    vpos = mvp * transform(pos, euler, vec3(butt_size)) * vpos;    

    fragTexCoord = vertexTexCoord;

    gl_Position = vpos;
}

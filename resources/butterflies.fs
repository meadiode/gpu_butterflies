#version 300 es
precision highp float;

in vec2 fragTexCoord;
in vec4 fragColor;
flat in int tex_id;

uniform sampler2D texture0;
uniform sampler2D texture1;

out vec4 finalColor;

void main()
{
    if (tex_id == 1)
    {
        finalColor = texture(texture1, fragTexCoord).rgba;
    }
    else
    {
        finalColor = texture(texture0, fragTexCoord).rgba;
    }
    
    if (finalColor.a == 0.0)
    {
        discard; /* To skip z-test */
    }

    finalColor *= fragColor;
}

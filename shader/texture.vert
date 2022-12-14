#version 150

in vec3 position;
in vec3 normal;
in vec2 uv;

uniform mat4 projection;
uniform mat4 view;

out vec4 fPosition;
out vec2 fUV;
out vec4 fLightPosition;
out vec3 fNormal;

void main(void)
{
    fPosition = view * vec4(position,1.0);
    fLightPosition = view * vec4(0.0,0.0,1.0,1.0);

    fUV = uv;
    fNormal = vec3(view * vec4(normal,0.0));

    gl_Position = projection * fPosition;
    /*gl_Position.x *= 1000.0f;*/
    /*gl_Position.y = 0.0;*/
}

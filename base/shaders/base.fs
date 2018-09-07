#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// custom inputs
uniform vec3 iResolution;
uniform float iTime;
uniform float iTimeDelta;
uniform vec2 iMouse;

// Output fragment color
out vec4 finalColor;

void main()
{
    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, fragTexCoord);
    
    // NOTE: Implement here your fragment shader code
    
    finalColor = texelColor*colDiffuse;
}


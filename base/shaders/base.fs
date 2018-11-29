// Input vertex attributes (from vertex shader)
varying vec2 fragTexCoord;
varying vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// custom inputs
uniform vec3 iResolution;
uniform float iTime;
uniform float iTimeDelta;
uniform vec2 iMouse;

void main()
{
    // Texel color fetching from texture sampler
    vec4 texelColor = texture(texture0, fragTexCoord);
    
    // NOTE: Implement here your fragment shader code
    
    gl_FragColor = texelColor*colDiffuse;
}

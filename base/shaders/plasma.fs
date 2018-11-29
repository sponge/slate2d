precision mediump float;

const float PI = 3.14159265;

const float pixelWidth = 8.0;
const float pixelHeight = 8.0;

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
   float dx = pixelWidth*(1.0/iResolution.x);
   float dy = pixelHeight*(1.0/iResolution.y);

   vec2 p = vec2(dx*floor(fragTexCoord.x/dx), dy*floor(fragTexCoord.y/dy));
   
   // main code, *original shader by: 'Plasma' by Viktor Korsun (2011)
   float x = p.x;
   float y = p.y;
   float mov0 = x+y+cos(sin(iTime)*2.0)*100.+sin(x/100.)*1000.;
   float mov1 = y / 0.9 +  iTime;
   float mov2 = x / 0.2;
   float c1 = abs(sin(mov1+iTime)/2.+mov2/2.-mov1-mov2+iTime);
   float c2 = abs(sin(c1+sin(mov0/1000.+iTime)+sin(y/40.+iTime)+sin((x+y)/100.)*3.));
   float c3 = abs(sin(c2+cos(mov1+mov2+c2)+cos(mov2)+sin(x/1000.)));
   gl_FragColor = vec4(c1,c2,c3,1);
}

#version 410
in vec3 colour;
out vec4 frag_colour;
in INTERFACE {
    vec3 position;
    vec3 lightSpacePosition;
    vec3 modelData;
} In ;
uniform sampler2D textureStrip;

uniform bool isPolar;
uniform float headOffset;
out vec4 fragColor;
void main(){
    if(isPolar){
        float radius= (1.0f-(In.modelData.y* 180.0f / 3.14159265358979f+15.0f)/110.0f)/2.0f;
        vec2 uv= vec2(radius*cos(In.modelData.x+headOffset),radius*sin(In.modelData.x+headOffset))+vec2(0.5,0.5);
        fragColor = vec4(texture(textureStrip,uv).rgb,1.0);
    }
    else{
        fragColor = vec4(texture(textureStrip,vec2(In.modelData.x,In.modelData.y)).rgb,1.0);
    }
}

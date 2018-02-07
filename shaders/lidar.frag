#version 410
in INTERFACE {
vec3 modelData;
} In ;
uniform sampler2D textureColor;
out vec4 fragColor;
void main(){
    float radius= (1.0f-(In.modelData.y* 180.0f / 3.14159265358979f+15.0f)/110.0f)/2.0f;
    vec2 uv= vec2(radius*cos(In.modelData.x),radius*sin(In.modelData.x))+vec2(0.5,0.5);
//    fragColor =vec4(In.modelData,1.0);// vec4(texture(textureColor,uv).rgb,1.0);
    int r=int(1024*In.modelData.z)%255;
    int g=int(2047*In.modelData.z)%255;
    int b=int(4095*In.modelData.z)%255;
    fragColor =vec4(normalize(vec3(r,g,b)),1.0);
    fragColor.rgb = pow(fragColor.rgb, vec3(1.0/1.8));
}

#version 330

// Attributes
layout(location = 0) in vec3 v;

// Uniform: the light structure (position in view space)
layout (std140) uniform Light {
	vec4 position;
	vec4 Ia;
	vec4 Id;
	vec4 Is;
	float shininess;
	vec3 polar;
} light;

uniform mat4 mvp;
uniform mat4 mv;
uniform mat3 normalMatrix;
uniform mat4 lightMVP;
uniform bool isPolar;
uniform float headOffset;

out vec3 colour;

out INTERFACE {
	vec3 position;
	vec3 lightSpacePosition;
	vec3 modelData;
} Out ;

void main(){
    vec3 vr;
    if(isPolar){
        vr = vec3(cos(v.x) * cos(v.y) * v.z ,sin(v.y) * v.z,sin(v.x) * cos(v.y) * v.z );
    }
    else{
        vr = v;
    }
    gl_Position = mvp * vec4(vr, 1.0);
    Out.position = (mv * vec4(vr,1.0)).xyz;
    Out.lightSpacePosition = 0.5*(lightMVP * vec4(vr,1.0)).xyz + 0.5;
    Out.modelData = v;
}

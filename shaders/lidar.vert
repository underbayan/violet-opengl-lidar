#version 330

// Attributes
layout(location = 0) in vec3 v;
layout(location = 1) in vec2 uv;
layout(location = 2) in int flag;


// Uniform: the light structure (position in view space)
layout (std140) uniform Light {
	vec4 position;
	vec4 Ia;
	vec4 Id;
	vec4 Is;
	float shininess;
} light;

uniform mat4 mvp;
uniform mat4 mv;
uniform mat3 normalMatrix;
uniform mat4 lightMVP;
out INTERFACE {
	vec3 position;
	vec3 lightSpacePosition;
	vec3 modelData;
} Out ;

void main(){
    vec3 vr=vec3(cos(v.x) * cos(v.y) * v.z ,sin(v.y) * v.z,sin(v.x) * cos(v.y) * v.z);
    gl_Position = mvp * vec4(vr, 1.0);
    Out.position = (mv * vec4(vr,1.0)).xyz;
    Out.lightSpacePosition = 0.5*(lightMVP * vec4(vr,1.0)).xyz + 0.5;
    Out.modelData = v;
}

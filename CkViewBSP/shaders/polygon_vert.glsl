#version 440 core

#pragma debug(on)

// uniforms
layout(location=0)uniform mat4 uni_v;
layout(location=1)uniform mat4 uni_p;
layout(location=2)uniform mat4 uni_vp;
layout(location=3)uniform vec3 uni_cameraposition;
layout(location=4)uniform sampler2D uni_lmtexture;

// in
layout(location=0)in vec3 in_position;
layout(location=1)in vec2 in_texcoord;
layout(location=2)in vec3 in_normal;
layout(location=3)in vec2 in_lightmap;
layout(location=4)in vec3 in_color;

// out
out VS_FS_VERTEX
{
	vec3 position;
	vec2 texcoord;
	vec3 normal;
	vec2 lightmap;
	vec3 color;
} vertex_out;

void main()
{
	vertex_out.position = in_position;
	vertex_out.texcoord = in_texcoord;
	vertex_out.normal = in_normal;
	vertex_out.lightmap = in_lightmap;
	vertex_out.color = in_color;
	//
	gl_Position = uni_vp * vec4(in_position, 1.0);
}
#version 440 core

#pragma debug(on)

// uniforms
layout(location=0)uniform mat4 uni_v;
layout(location=1)uniform mat4 uni_p;
layout(location=2)uniform mat4 uni_vp;
layout(location=3)uniform vec3 uni_cameraposition;
layout(location=4)uniform sampler2D uni_lmtexture;

// in
layout(location=0)in float in_id;

// out
out float vertex_id;

void main()
{
	vertex_id = in_id;
	//vertex_out.position_ms = in_position;
	//vertex_out.texcoord = in_texcoord;
	//vertex_out.normal = in_normal;
	//vertex_out.lightmap = in_lightmap;
	//
	//gl_Position = uni_vp * vec4(in_position, 1.0);
	gl_Position = vec4(0);
}
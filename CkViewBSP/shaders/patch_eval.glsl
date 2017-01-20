#version 440 core

#pragma debug(on)

// layout
layout (quads, equal_spacing, ccw) in;

// uniforms
layout(location=0)uniform mat4 uni_v;
layout(location=1)uniform mat4 uni_p;
layout(location=2)uniform mat4 uni_vp;
layout(location=3)uniform vec3 uni_cameraposition;
layout(location=4)uniform sampler2D uni_lmtexture;

// Patch
struct patch_data
{
	vec4 position[3][3];
	vec4 texcoord[3][3];
	vec4 normal[3][3];
	vec4 lightmap[3][3];
	vec4 color[3][3];
};

// shader storage
layout(std140, binding = 0) buffer patch_buffer
{
	patch_data patches[];
};

// in
patch in TC_TE_PATCH
{
	float id;
} patch_in;

// out
out TE_FS_VERTEX
{
	vec3 position;
	vec2 texcoord;
	vec3 normal;
	vec2 lightmap;
	vec3 color;
} vertex_out;

vec4 quadratic(vec2 p, vec4 q[3][3])
{
	float u = p.x;
	float uinv = 1 - u;
	float v = p.y;
	float vinv = 1 - v;

	vec4 p_u[3];
	for(int i = 0; i < 3; i++)
	{
		p_u[i] = (uinv*uinv)*q[0][i] + 2*(uinv)*u*q[1][i] + (u*u)*q[2][i];
	}

	vec4 p_uv = (vinv*vinv)*p_u[0] + 2*vinv*v*p_u[1] + (v*v)*p_u[2];

	return(p_uv);
}

void main()
{
	int id = int(patch_in.id);
	vertex_out.position = quadratic(gl_TessCoord.xy, patches[id].position).xyz;
	vertex_out.texcoord = quadratic(gl_TessCoord.xy, patches[id].texcoord).xy;
	vertex_out.normal = quadratic(gl_TessCoord.xy, patches[id].normal).xyz;
	vertex_out.lightmap = quadratic(gl_TessCoord.xy, patches[id].lightmap).xy;
	vertex_out.color = quadratic(gl_TessCoord.xy, patches[id].color).xyz;

	gl_Position = (uni_p * uni_v) * vec4(vertex_out.position, 1.0);
}
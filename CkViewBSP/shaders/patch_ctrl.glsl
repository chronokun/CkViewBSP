#version 440 core

#pragma debug(on)

// layout
layout (vertices = 1) out;

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
in float vertex_id[];

// out
patch out TC_TE_PATCH
{
	float id;
} patch_out;

float calc_tessellation(vec3 a, vec3 b, vec3 c)
{
	precise float cp = 0.;
	if(length(b-a) > 1. && length(c-b) > 1.)
	{
		cp = length(cross(normalize(b-a), normalize(c-b)));
	}
	precise float dp = -clamp(dot(normalize(b-a), normalize(c-b)), -1.0, 0.0);
	precise float sum = (cp+dp);

	precise float len = length(b-a) + length(c-b);
	precise float dist = length(((a+b+c)/3.0)-uni_cameraposition) / 32.f;

	precise float screensize = len*(1./(dist*dist));

	precise float tessfact = clamp(screensize*cp*64.0, 1.0, 64.0);

	return(tessfact);
}

void main()
{
	patch_out.id = vertex_id[0];

	vec3 points[3][3];
	for(int u = 0; u < 3; u++)
	{
		for(int v = 0; v < 3; v++)
		{
			points[u][v] = patches[int(patch_out.id)].position[u][v].xyz;
		}
	}

	gl_TessLevelOuter[1] = calc_tessellation(points[0][0], points[1][0], points[2][0]);
	gl_TessLevelOuter[2] = calc_tessellation(points[2][0], points[2][1], points[2][2]);
	gl_TessLevelOuter[3] = calc_tessellation(points[2][2], points[1][2], points[0][2]);
	gl_TessLevelOuter[0] = calc_tessellation(points[0][2], points[0][1], points[0][0]);

	precise float tess_mid_u = calc_tessellation(points[0][1], points[1][1], points[2][1]);
	precise float tess_mid_v = calc_tessellation(points[1][0], points[1][1], points[1][2]);
	
	precise float tess_inner_a = (gl_TessLevelOuter[0] + gl_TessLevelOuter[2] + tess_mid_v) / 3.;
	precise float tess_inner_b = (gl_TessLevelOuter[1] + gl_TessLevelOuter[3] + tess_mid_u) / 3.;

	gl_TessLevelInner[1] = tess_inner_a;
	gl_TessLevelInner[0] = tess_inner_b;
}
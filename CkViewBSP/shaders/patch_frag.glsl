#version 440 core

#pragma debug(on)

// uniforms
layout(location=0)uniform mat4 uni_v;
layout(location=1)uniform mat4 uni_p;
layout(location=2)uniform mat4 uni_vp;
layout(location=3)uniform vec3 uni_cameraposition;
layout(location=4)uniform sampler2D uni_lmtexture;

// in
in TE_FS_VERTEX
{
	vec3 position;
	vec2 texcoord;
	vec3 normal;
	vec2 lightmap;
	vec3 color;
} frag_in;

// out
out vec4 out_color;

float dist_grid(vec2 texcoord, vec2 spacing, vec2 linewidth)
{
	vec2 d = vec2(1.);
	d.x = min(d.x, abs(0. - mod(texcoord, spacing.x).x));
	d.x = min(d.x, abs(spacing.x - mod(texcoord, spacing.x).x));
	d.y = min(d.y, abs(0. - mod(texcoord, spacing.y).y));
	d.y = min(d.y, abs(spacing.y - mod(texcoord, spacing.y).y));
	d -= linewidth;
	d *= 1./fwidth(texcoord);
	d = clamp(d, vec2(0.), vec2(1.));

	float g = 1.-min(d.x,d.y);

	return g;
}

vec3 draw_grid(vec2 texcoord, vec3 c)
{
	float g = dist_grid(texcoord, vec2(1./1.), vec2(1./4096.))
			+ (dist_grid(texcoord, vec2(1./16.), vec2(1./4096.))*(1./4.))
			+ (dist_grid(texcoord, vec2(1./256.), vec2(1./4096.))*(1./16.));

	//vec3 color = mix(vec3((normal + vec3(1.)) / 4.0), vec3(1.), clamp(g, 0., 1.));

	vec3 o = mix(c, vec3(1.), clamp(g, 0., 1.));

	//vec3 p_cs = (uni_vp * vec4(frag_in.position, 1.0)).xyz;
	//
	//o = mix(vec3(0), o, exp(-p_cs.z*.001));

	return o;
}

float dist_dots(vec2 texcoord, vec2 spacing, vec2 linewidth)
{
	vec2 d = vec2(1.);
	d = min(d, length((spacing/2.)-mod(texcoord, spacing)).xx-linewidth);
	d = min(d, length((spacing/2.)-mod(texcoord+(spacing/2.), spacing)).xx-linewidth);
	//d -= linewidth;
	d *= 1./fwidth(texcoord);
	d = clamp(d, vec2(0.), vec2(1.));

	float g = 1.-min(d.x,d.y);

	return g;
}

vec3 draw_dots(vec2 texcoord, vec3 c)
{
	float d = dist_dots(texcoord, vec2(1./8.), vec2(1./64.));

	vec3 o = mix(c, vec3(0.), clamp(d, 0., 1.));

	vec2 f = clamp(fwidth(texcoord)/(vec2(1./64.)*4.), vec2(0.), vec2(1.));

	o = mix(o, (c+vec3(0.))/2., max(f.x, f.y));

	return(o);
}

float dist_stripes(vec2 texcoord, vec2 spacing, float linewidth)
{
	float d = 1.;
	d = min(d, abs(dot(mod(texcoord, spacing)-(spacing/2.), normalize(vec2(1.)))));
	d = min(d, (dot(mod(texcoord, spacing)-spacing, normalize(vec2(-1.)))));
	d = min(d, (dot(mod(texcoord, spacing), normalize(vec2(1.)))));
	d -= linewidth;
	//d *= 1./fwidth(texcoord);
	//d = clamp(d, vec2(0.), vec2(1.));

	//float g = 1.-min(d.x,d.y);

	return d;
}

vec3 draw_stripes(vec2 texcoord, vec3 c)
{
	vec2 r = fwidth(texcoord);

	float d = 0.;
	for(int i = 0; i < 16; i++)
	{
		float a = (i / 16.) * 3.14159265 * 2.;
		vec2 p = texcoord + (vec2(cos(a), sin(a))*(r/8.));
		if(dist_stripes(p, vec2(1./8.), (1./64.)) > 0.)
		{
			d += 1.;
		}
	}
	d /= 16.;

	vec3 o = mix(c, vec3(0.), clamp(d, 0., 1.));

	vec2 f = clamp(fwidth(texcoord)/(vec2(1./64.)*4.), vec2(0.), vec2(1.));

	o = mix(o, (c+vec3(0.))/4., max(f.x, f.y));

	return(o);
}

void main()
{
	vec3 normal = normalize((vec4(frag_in.normal, 0.0) * inverse(uni_v)).xyz);
	//vec3 color = draw_grid(frag_in.texcoord.xy, normal);
	//vec3 color = frag_in.color;
	//vec3 color = (normal+vec3(1))/2.;
	//vec3 color = frag_in.color;
	//vec3 color = draw_grid(frag_in.texcoord, vec3((normal + vec3(1.)) / 4.0));
	//vec3 color = draw_grid(frag_in.texcoord, vec3(0.5));
	//vec3 color = vec3((normal + vec3(1.)) / 2.0);
	//
	//float l = (dot(frag_in.normal, normalize(vec3(1., 4., 2.)))+1.)/2.;
	//vec3 color = draw_grid(frag_in.texcoord, vec3(.125, .125, .5));
	//color *= l;
	vec3 color = draw_grid(frag_in.texcoord, vec3(0.));
	//vec3 color = vec3(1.);

	//if(length(color) < 1.)
	//{
	//	discard;
	//}

	color *= texture(uni_lmtexture, frag_in.lightmap).rgb;

	out_color = vec4(color, 1.0);
}
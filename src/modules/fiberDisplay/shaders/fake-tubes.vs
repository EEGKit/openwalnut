varying float tangent_dot_view;
varying vec3 tangentR3;
varying float s_param;
varying vec4 tubeColor;
uniform float u_thickness;
varying vec2 imageTangent;
varying float usePointSprite;
varying vec3 view;
varying vec3 lightDir;
varying vec3 halfVector;
varying float NdotL;
varying vec3 normal;
varying float endPoint;

void main()
{
	vec4 pos;
	float thickness = u_thickness*0.01/100;
	vec3 viewPoint = vec3(0., 0., -1.);
	view = (gl_ModelViewMatrix * gl_Vertex).xyz;
	//lightDir = normalize(gl_LightSource[0].position.xyz - view);
	//halfVector = normalize(gl_LightSource[0].halfVector.xyz);
	lightDir = normalize( viewPoint - view);
	halfVector = lightDir;
	normal = normalize(gl_NormalMatrix * gl_Normal);
	usePointSprite = 1.0;
	tangentR3 = gl_Normal;
	endPoint = 1.0;

	gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex; //< make clipping planes work

	// defines northpoint (1.0), southpoint (-1.0) or point sprite (0.0)
	s_param = gl_MultiTexCoord0.s; //< store texture coordinate for shader

	vec3 tangent = (gl_ModelViewProjectionMatrix * vec4(gl_Normal,0.)).xyz; //< transform our tangent vector
	//pos = ftransform(); //< transform position to eye space
	pos = gl_ModelViewProjectionMatrix * gl_Vertex;

	// color of tube
	tubeColor = gl_Color;

	vec3 offsetNN = cross( normalize(tangent.xyz), viewPoint);
	vec3 offset = normalize(offsetNN);
	tangent_dot_view = length(offsetNN);

	//scalar product between viewDir and tangent
	float tmp = dot(lightDir, tangent);

	offset.x *= thickness;
	offset.y *= thickness;

	//decide wether point sprite or triangle strip to render
	if(s_param == 0.0)
	{
		//calculation for pointsprites
		if(tmp > 0.93 || gl_MultiTexCoord0.t == 0.0)
		{
			vec4 pos1 = gl_Vertex;
			vec4 pos2 = gl_Vertex + vec4(gl_Normal,0);

			vec4 pos1p = gl_ModelViewProjectionMatrix * pos1.xyzw;
			vec4 pos2p = gl_ModelViewProjectionMatrix * pos2.xyzw;

			imageTangent.xy = (pos2p.x/pos2p.w - pos1p.x/pos1p.w, pos1p.y/pos1p.w - pos2p.y/pos2p.w);
			gl_PointSize = 5;
			usePointSprite = 0.0;
			endPoint = gl_MultiTexCoord0.t;
		}
		else
		{
			//clip point sprite
			pos.w = 0.0;
		}
	}
	else
	{
		// don't draw tringle strip	//decide wether point sprite or triangle strip to render
		if(tmp >= 0.98)
		{
			tubeColor.a = 0.0;
		}
		else
		{
			// transform triangle points to position

			pos.xyz += offset * (s_param); //< add offset in y-direction (eye-space)

			// TexCoord for 1D texture
			gl_TexCoord[1] = gl_MultiTexCoord0.t;
		}
	}
	gl_Position = pos; //< store final position
}

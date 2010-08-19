varying float tangent_dot_view;
varying vec3 tangentR3;
varying float s_param;
varying vec4 tubeColor;
uniform float u_thickness;
uniform int u_viewportHeight;
uniform int u_viewportWidth;
varying vec2 imageTangent;
varying float usePointSprite;
varying vec3 view;
varying vec3 lightDir;
varying float NdotL;
varying vec3 normal;
varying float endPoint;
uniform gl_DepthRangeParameters gl_DepthRange;
varying float z;
uniform vec3 nearPos;
uniform vec3 farPos;
varying float zNear;
varying float zFar;

void main()
{
	float thickness;
	vec4 pos;
	vec3 lightPosition = vec3(0., 0., -1.);
	usePointSprite = 1.0;
	tangentR3 = gl_Normal;
	endPoint = 1.0;

	pos = gl_ModelViewProjectionMatrix * gl_Vertex;
	z = pos.z;

	zNear = (gl_ModelViewProjectionMatrix * vec4(nearPos,0)).z;
	zFar = (gl_ModelViewProjectionMatrix * vec4(farPos,0)).z;

	view = - pos.xyz;

	//lightDir = normalize(gl_LightSource[0].position.xyz - view);
	//halfVector = normalize(gl_LightSource[0].halfVector.xyz);

	lightDir = normalize( lightPosition - pos.xyz);

	vec3 cameraPosition = vec3(gl_ModelViewMatrixInverse * vec4(0,0,0,1.0));
	vec3 cameraVector = cameraPosition - gl_Vertex.xyz;

	gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex; // make clipping planes work

	// defines northpoint (1.0), southpoint (-1.0) or point sprite (0.0)
	s_param = gl_MultiTexCoord0.s;

	vec3 tangent = (gl_ModelViewProjectionMatrix * vec4(gl_Normal,0.)).xyz; // transform our tangent vector

	//define thickness of tubes
	thickness = u_thickness * 0.0001 * (1 - (z+zNear)/(zNear+zFar));

	// color of tube
	tubeColor = gl_Color;

	vec3 offsetNN = cross( normalize(tangent.xyz), lightPosition);
	vec3 offset = normalize(offsetNN);
	tangent_dot_view = length(offsetNN);
	normal = normalize(cross( offsetNN, lightPosition));

	//scalar product between viewDir and tangent
	float tmp = dot(view, tangent);

	offset.x *= thickness;
	offset.y *= thickness;

	//decide wether point sprite or triangle strip to render
	#ifdef USE_POINTSPRITE
	//if(s_param == 0.0)
	//{

		//calculation for pointsprites
		if(tmp > 0.93 || gl_MultiTexCoord0.t == 0.0)
		{
			vec4 pos1 = gl_Vertex;
			vec4 pos2 = gl_Vertex + vec4(gl_Normal,0);

			vec4 pos1p = gl_ModelViewProjectionMatrix * pos1.xyzw;
			vec4 pos2p = gl_ModelViewProjectionMatrix * pos2.xyzw;

			imageTangent.xy = (pos2p.x/pos2p.w - pos1p.x/pos1p.w, pos1p.y/pos1p.w - pos2p.y/pos2p.w);

			//	compute size with u_viewportWidth and u_viewportHeight;
			//gl_PointSize = tangent_dot_view * u_viewportWidth;
			gl_PointSize = 20;

			usePointSprite = 0.0;
			endPoint = gl_MultiTexCoord0.t;
		}
		else
		{
			//clip point sprite
			pos.w = 0.0;
		}

	#else
	//}
	//else
	//{
		// don't draw quad strip	//decide wether point sprite or triangle strip to render
		if(tmp >= 0.98)
		{
			tubeColor.a = 0.0;
		}
		else
		{
			// transform quad points to position
			pos.xyz += offset * (s_param); //< add offset in y-direction (eye-space)

			// TexCoord for 1D texture
			gl_TexCoord[1] = gl_MultiTexCoord0.t;
		}
	//}
	#endif
	gl_Position = pos; //< store final position
}

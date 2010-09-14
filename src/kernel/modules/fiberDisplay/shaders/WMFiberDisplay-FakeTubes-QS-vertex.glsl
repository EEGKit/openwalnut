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
varying vec3 halfVector;
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
	vec3 lightPosition = vec3(0., 0., 1000.);
  vec3 ez = vec3(0., 0., 10.0);
	usePointSprite = 1.0;
	tangentR3 = gl_Normal;
	endPoint = 1.0;

	pos = gl_ModelViewProjectionMatrix * gl_Vertex;
	z = pos.z;

  vec3 q1 = nearPos;
  vec3 q2 = nearPos; q2.x = farPos.x;
  vec3 q4 = nearPos; q4.y = farPos.y;
  vec3 q5 = nearPos; q5.z = farPos.z;

  vec3 q7 = farPos;
  vec3 q8 = farPos; q8.x = nearPos.x;
  vec3 q6 = farPos; q6.y = nearPos.y;
  vec3 q3 = farPos; q3.z = nearPos.z;

	float z1 =0.5 + 0.5*( (gl_ModelViewProjectionMatrix * vec4(q1,1.0)).z );
	float z2 =0.5 + 0.5*( (gl_ModelViewProjectionMatrix * vec4(q2,1.0)).z );
	float z3 =0.5 + 0.5*( (gl_ModelViewProjectionMatrix * vec4(q3,1.0)).z );
	float z4  =0.5 + 0.5*( (gl_ModelViewProjectionMatrix * vec4(q4,1.0)).z );
	float z5  =0.5 + 0.5*( (gl_ModelViewProjectionMatrix * vec4(q5,1.0)).z );
	float z6  =0.5 + 0.5*( (gl_ModelViewProjectionMatrix * vec4(q6,1.0)).z );
	float z7  =0.5 + 0.5*( (gl_ModelViewProjectionMatrix * vec4(q7,1.0)).z );
	float z8  =0.5 + 0.5*( (gl_ModelViewProjectionMatrix * vec4(q8,1.0)).z );

  zNear = max( z1, max( z2, max( z3, max( z4, max( z5, max( z6, max( z7,z8)))))));
  zFar  = min( z1, min( z2, min( z3, min( z4, min( z5, min( z6, min( z7,z8)))))));

	view = - pos.xyz;

	lightDir = normalize( lightPosition - pos.xyz);

	vec3 cameraPosition = vec3(gl_ModelViewMatrixInverse * vec4(0.0, 0.0, 0.0, 1.0));
	vec3 referencePosition = vec3(gl_ModelViewMatrixInverse * vec4(1.0, 1.0, 1.0, 1.0));
	vec3 cameraVector = cameraPosition - gl_Vertex.xyz;

	gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex; // make clipping planes work

	// defines northpoint (1.0), southpoint (-1.0) or point sprite (0.0)
	s_param = gl_MultiTexCoord0.s;

	vec3 tangent = (gl_ModelViewProjectionMatrix * vec4(gl_Normal,0.)).xyz; // transform our tangent vector
	halfVector = normalize(cross(view,tangent));
	normal = cross( halfVector, tangent);

	//define thickness of tubes
	thickness = u_thickness * 0.001; //* length(cross(cameraPosition, referencePosition));//(1 + (zNear));

	// color of tube
	tubeColor = gl_Color;

	vec3 offsetNN = cross( (tangent.xyz), ez);
	vec3 offset = (offsetNN);
	tangent_dot_view = length(offsetNN);

	//scalar product between viewDir and tangent
	float tmp = dot(view, tangent);

	offset.x *= thickness;
	offset.y *= thickness;

	//decide wether point sprite or triangle strip to render
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
	gl_Position = pos; //< store final position
}

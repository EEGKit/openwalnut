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
	vec3 lightPosition = vec3(0., 0., 10.);
	usePointSprite = 1.0;
	tangentR3 = gl_Normal;
	endPoint = 1.0;

	pos = gl_ModelViewProjectionMatrix * gl_Vertex;
	z = pos.z;

	zNear =0.5 + 0.5*( (gl_ModelViewProjectionMatrix * vec4(nearPos,1.00)).z );
	zFar =0.5 + 0.5*( (gl_ModelViewProjectionMatrix * vec4(farPos,1.0)).z );
  float tmp2 = zNear;
  zNear = min( zNear, zFar );
  zFar = max( zFar, tmp2 );

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

	vec3 offsetNN = cross( normalize(tangent.xyz), lightPosition);
	vec3 offset = normalize(offsetNN);
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

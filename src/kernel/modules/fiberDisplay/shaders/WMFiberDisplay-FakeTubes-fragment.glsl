varying vec3 tangentR3; // Tangent vector in world space
varying float s_param; // s parameter of texture [-1..1]
varying float tangent_dot_view;
uniform bool globalColor;
uniform sampler1D texture;
uniform sampler2D texturePS;
varying float usePointSprite;
varying vec2 imageTangent;
varying vec4 tubeColor;
varying vec3 view;
varying vec3 lightDir;
varying float NdotL;
varying vec3 normal;
varying float endPoint;
uniform gl_DepthRangeParameters gl_DepthRange;
varying float z;
varying float zNear;
varying float zFar;

void main()
{

	vec3 L,halfVector,V, T, N, halfV;
	vec3 specular = 0.0;
	float NdotL,NdotHV;
	float alpha, beta, lt;
	halfVector = view - lightDir;
	T = normalize(tangentR3);
	V = normalize(view);
	L = normalize(lightDir);
	N = normalize(normal);

	NdotL = max(dot(N,L),0.0);
	vec4 color;

	// koordinaten für illuminated lines texturen
	alpha = dot(L,N) / sqrt(1 - pow( dot(L,T),2));
	beta = dot(halfVector,N) / sqrt( 1 - pow( dot(halfVector, T), 2));
	lt = dot(L, T);

	//compute rotation in imageplane for pointsprite
	#ifdef USE_POINTSPRITE
	//if(usePointSprite < 0.8)
	//{
		vec2 imageTangentNorm = normalize(imageTangent);
		vec2 newTexCoords;
		newTexCoords.x = (imageTangentNorm.y * (gl_PointCoord.y - 0.5) + imageTangentNorm.x * (gl_PointCoord.x - 0.5)) + 0.5;
		newTexCoords.y = (imageTangentNorm.y * (gl_PointCoord.x - 0.5) - imageTangentNorm.x * (gl_PointCoord.y - 0.5)) + 0.5;

		color = texture2D(texturePS, newTexCoords.xy);

		if(endPoint == 0.0)
		{
			color.x = 1.0;
		}
		gl_FragColor.a = color.z;

	#else
	//}
	//else
	//{
		color = texture1D(texture, gl_TexCoord[1].s);
		gl_FragColor.a = tubeColor.a;
	//}
	#endif

		/* compute the specular term if NdotL is  larger than zero */
		if (NdotL > 0.0) {

				// normalize the half-vector, and then compute the
				// cosine (dot product) with the normal
				halfV = normalize(halfVector);

				NdotHV = max(dot(normal, halfV),0.0);
				specular = pow(NdotHV,2) * color.y * tubeColor.rgb;
		}

	float depthCueingFactor = (1 - (z+zNear)/(zNear+zFar));
	gl_FragColor.rgb = tubeColor.rgb * (color.x) * depthCueingFactor + specular;
}


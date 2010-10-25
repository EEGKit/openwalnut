uniform bool globalColor;
uniform sampler1D texture;
uniform sampler2D texturePS;
varying vec2 imageTangent;
varying vec4 tubeColor;
varying vec3 view;
varying vec3 lightDir;
varying float NdotL;
varying vec3 normal;
varying vec3 halfVector;
varying float endPoint;
varying vec3 tangentR3;

varying float z;
varying float zNear;
varying float zFar;

uniform bool useDepthCueing;
uniform bool useLightModel;
uniform bool usePointSprites;
uniform bool useQuadStripes;
uniform bool useProjection;
uniform float lowerDepthCueingFactor;
uniform float upperDepthCueingFactor;
varying vec3 eyeDirection;
varying vec3  lightDirection;

void main()
{
    vec3 L,V, T, N, halfV;
    float specular = 0.0;
    float NdotL,NdotHV;
    float alpha, beta, lt;

    T = normalize(tangentR3);
    V = normalize(eyeDirection);
    L = normalize(lightDirection);
    N = normalize(normal);

    float depthCueingFactor = 1.0;
    NdotL = saturate(dot(N, L));
    vec3 eyeDir = normalize(eyeDirection);

    vec4 color;

    vec2 newTexCoords;
    if ( useLightModel )
    {
        // koordinaten für illuminated lines texturen
        alpha = dot(L,N) / sqrt(1 - pow( dot(L,T),2));
        beta = dot(halfVector,N) / sqrt( 1 - pow( dot(halfVector, T), 2));
        lt = dot(L, T);
    }

    //compute rotation in imageplane for pointsprite
    vec2 imageTangentNorm = normalize(imageTangent);
    newTexCoords.x = (imageTangentNorm.y * (gl_PointCoord.y - 0.5) + imageTangentNorm.x * (gl_PointCoord.x - 0.5)) + 0.5;
    newTexCoords.y = (imageTangentNorm.y * (gl_PointCoord.x - 0.5) - imageTangentNorm.x * (gl_PointCoord.y - 0.5)) + 0.5;
    color = texture2D(texturePS, newTexCoords.xy);

    gl_FragColor.a = color.z;

    if ( useDepthCueing )
    {
        depthCueingFactor = clamp(( 1 - z + zNear ) / ( zNear + zFar ), lowerDepthCueingFactor, upperDepthCueingFactor );
    }

    if(endPoint == 0.0)
    {
        gl_FragDepth = gl_FragCoord.z - 0.01;
        gl_FragColor.rgb = (vec3(0.2,0.2,0.2)+vec3(0.6,0.6,0.6)*NdotL) * tubeColor.rgb * depthCueingFactor  * depthCueingFactor;
    }
    else
    {
        /* compute the specular term if NdotL is  larger than zero */
        if (NdotL > 0.0)
        {
            // normalize the half-vector, and then compute the
            // cosine (dot product) with the normal
            halfV = normalize(halfVector);

            NdotHV = saturate(dot(N, halfV));
            specular = pow(NdotHV,16)*color.y;

       }
        gl_FragColor.rgb = (tubeColor.rgb * ((vec3(0.2,0.2,0.2)+vec3(0.6,0.6,0.6)*NdotL*color.r) + vec3(0.6,0.6,0.6)*specular )) * depthCueingFactor  * depthCueingFactor;
    }
}

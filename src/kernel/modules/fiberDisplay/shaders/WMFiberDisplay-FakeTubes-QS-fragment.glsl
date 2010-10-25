uniform bool globalColor;
uniform sampler1D texture;
uniform sampler2D texturePS;
uniform sampler2D textureDiff2D;
uniform sampler2D textureSpec2D;
varying vec4 tubeColor;
varying vec3 view;
varying vec3 lightDir;
varying float NdotL;
varying vec3 normal;
varying vec3 halfVector;
varying float endPoint;
varying float z;
varying float zNear;
varying float zFar;
varying vec3 tangentR3;

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

    if ( useDepthCueing )
    {
        depthCueingFactor = clamp( ( 1 - z + zNear ) / ( zNear + zFar ), lowerDepthCueingFactor, upperDepthCueingFactor );
    }

    if ( useLightModel )
    {
        // koordinaten für illuminated lines texturen
        halfV = normalize(halfVector);
        alpha = dot(L,N) / sqrt(1 - pow( dot(L,T),2));
        float ht = sqrt( 1 - pow( dot(halfV, T), 2));
        beta = dot(halfV,N) / ht;
        lt = dot(L, T);
        vec4 colorDiff = texture2D(textureDiff2D, vec2( alpha, lt ));
        vec4 colorSpec = texture2D(textureSpec2D, vec2( alpha, beta ));
       gl_FragColor.rgb = (tubeColor.rgb * ((vec3(0.2,0.2,0.2)+vec3(0.6,0.6,0.6)*pow(ht,64)*colorDiff.x) + vec3(0.6,0.6,0.6)*colorSpec.x )) * depthCueingFactor  * depthCueingFactor;

    }
    else
    {
        vec4 color = texture1D(texture, (gl_TexCoord[1].s ));
        gl_FragColor.a = tubeColor.a;
        /* compute the specular term if NdotL is  larger than zero */
        if (NdotL > 0.0)
        {
            // normalize the half-vector, and then compute the
            // cosine (dot product) with the normal
            halfV = normalize(halfVector);

            NdotHV = saturate(dot(N, halfV));
            specular = pow(NdotHV,16) * color.y;

       }
       gl_FragColor.rgb = (tubeColor.rgb * ((vec3(0.2,0.2,0.2)+vec3(0.6,0.6,0.6)*NdotL*color.r) + vec3(0.6,0.6,0.6)*specular )) * depthCueingFactor  * depthCueingFactor;
    }
}

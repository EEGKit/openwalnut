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
varying vec3 halfVector;
varying float endPoint;
varying float z;
varying float zNear;
varying float zFar;

uniform bool useDepthCueing;
uniform bool useLightModel;
uniform bool usePointSprites;
uniform bool useQuadStripes;
uniform bool useProjection;

void main()
{

    vec3 L,V, T, N, halfV;
    vec3 specular = 0.0;
    float NdotL,NdotHV;
    float alpha, beta, lt;
    T = normalize(tangentR3);
    V = normalize(view);
    L = normalize(lightDir);
    N = normalize(normal);
    float depthCueingFactor = 1.0;
    //NdotL = max(dot(N,L),0.0);
    vec4 color;

    if ( useLightModel )
    {
        // koordinaten für illuminated lines texturen
        alpha = dot(L,N) / sqrt(1 - pow( dot(L,T),2));
        beta = dot(halfVector,N) / sqrt( 1 - pow( dot(halfVector, T), 2));
        lt = dot(L, T);
    }
    else
    {
        color = texture1D(texture, gl_TexCoord[1].s);
        gl_FragColor.a = tubeColor.a;
    }

    /* compute the specular term if NdotL is  larger than zero */
    if (NdotL > 0.0)
    {
        // normalize the half-vector, and then compute the
        // cosine (dot product) with the normal
        halfV = normalize(halfVector);

        NdotHV = max(dot(N, halfV),0.0);
        specular = pow(NdotHV,16) * color.y;
    }

    if ( useDepthCueing )
    {
        depthCueingFactor = clamp((1 - (z+zNear)/(zNear+zFar)),0.5,0.8);
    }
    gl_FragColor.rgb = ( tubeColor.rgb * color.x + specular ) * depthCueingFactor  * depthCueingFactor;
}

varying float tangent_dot_view;
varying vec3 tangentR3;
varying vec4 tubeColor;

varying vec2 imageTangent;
varying vec3 view;
varying vec3 lightDir;
varying float NdotL;
varying vec3 normal;
varying vec3 halfVector;
varying float endPoint;

varying float z;
varying float zNear;
varying float zFar;

attribute vec2 psTexCoords;

uniform float u_thickness;
uniform int u_viewportHeight;
uniform int u_viewportWidth;

uniform vec3 nearPos;
uniform vec3 farPos;

uniform bool useDepthCueing;
uniform bool useLightModel;
uniform bool usePointSprites;
uniform bool useQuadStripes;
uniform bool useProjection;
uniform float lowerDepthCueingFactor;
uniform float upperDepthCueingFactor;

// Auch die Richtung zum Betrachter ist in der Regel
// für jedes Fragment unterschiedlich.
varying vec3 eyeDirection;
varying vec3  lightDirection;


void main()
{
    vec4 pos = gl_ModelViewProjectionMatrix * gl_Vertex;
    vec3 tangent = ( gl_ModelViewProjectionMatrix * vec4(gl_Normal, 1. ) ).xyz; //!< transform our tangent vector
    vec3 tangentR3 = normalize(gl_NormalMatrix * gl_Normal);
    vec3 eyeDir = normalize(-(gl_ModelViewMatrix * gl_Vertex).xyz);
    gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex;//!< make clipping planes work

    //scalar product between viewDir and tangent
    float tmp = dot( tangent, eyeDir);

    //calculation for pointsprites
    if(tmp > 0.93 ||psTexCoords.t == 0.0 || !useQuadStripes)
    {
        float thickness;
        vec3 lightPos = vec3(0., 0., 0.);
        vec3 lightDir = normalize(lightPos+eyeDir);
        vec3 ez = vec3(0., 0., -1.0);
        endPoint = 1.0;

        if ( useDepthCueing || useProjection )
        {
            z = pos.z;
            float z1 =0.5 + 0.5 * ( ( gl_ModelViewProjectionMatrix * vec4( nearPos, 1.0 ) ).z );
            float z2 =0.5 + 0.5 * ( ( gl_ModelViewProjectionMatrix * vec4( farPos.x, nearPos.yz, 1.0 ) ).z );
            float z3 =0.5 + 0.5 * ( ( gl_ModelViewProjectionMatrix * vec4( farPos.xy, nearPos.z, 1.0 ) ).z );
            float z4 =0.5 + 0.5 * ( ( gl_ModelViewProjectionMatrix * vec4( nearPos.x, farPos.y, nearPos.z, 1.0 ) ).z );
            float z5 =0.5 + 0.5 * ( ( gl_ModelViewProjectionMatrix * vec4( nearPos.xy, farPos.z, 1.0 ) ).z );
            float z6 =0.5 + 0.5 * ( ( gl_ModelViewProjectionMatrix * vec4( farPos.x, nearPos.y, farPos.z, 1.0 ) ).z );
            float z7 =0.5 + 0.5 * ( ( gl_ModelViewProjectionMatrix * vec4( farPos, 1.0 ) ).z );
            float z8 =0.5 + 0.5 * ( ( gl_ModelViewProjectionMatrix * vec4( nearPos.x, farPos.yz, 1.0 ) ).z );

            zFar = max( z1, max( z2, max( z3, max( z4, max( z5, max( z6, max( z7,z8)))))));
            zNear  = min( z1, min( z2, min( z3, min( z4, min( z5, min( z6, min( z7,z8)))))));
        }

        vec3 cameraPosition = vec3(gl_ModelViewProjectionMatrix * vec4(0.0, 0.0, 1.0, 0.0));
        vec3 referencePosition = vec3(gl_ModelViewProjectionMatrix * vec4(0.0, 0.0, -1.0, 0.0));

        halfVector = normalize( eyeDir + lightDir );
        vec3 binormal = normalize( cross( tangentR3, eyeDir ) );
        normal = normalize( cross( binormal, tangentR3));

        lightDirection = lightDir;
        eyeDirection = eyeDir;

        // color of tube
        tubeColor = gl_Color;
        vec3 offset = normalize(cross( eyeDir, tangentR3));
        if ( useProjection )
        {
            thickness *= clamp(( 1 - z + zNear ) / ( zNear + zFar ), lowerDepthCueingFactor, upperDepthCueingFactor );
        }

        //scalar product between viewDir and tangent
        // compute thickness
        thickness = max(distance(referencePosition, cameraPosition), 0.1) * u_thickness * 0.001;
        offset.x *= thickness*u_viewportWidth;
        offset.y *= thickness*u_viewportHeight;

        vec4 pos1 = gl_Vertex;
        vec4 pos2 = gl_Vertex + vec4(gl_Normal,1);

        vec4 pos1p = gl_ModelViewProjectionMatrix * pos1;
        vec4 pos2p = gl_ModelViewProjectionMatrix * pos2;

        imageTangent.xy = (pos2p.x/pos2p.w - pos1p.x/pos1p.w, pos1p.y/pos1p.w - pos2p.y/pos2p.w);
        imageTangent = normalize(imageTangent);

        //  compute size with u_viewportWidth and u_viewportHeight;
        gl_PointSize = length(vec2(offset.x, offset.y));

        endPoint = psTexCoords.t;
    }
    else
    {
        //clip point sprite
        pos.w = 0.0;
    }

    gl_Position = pos; //< store final position
}

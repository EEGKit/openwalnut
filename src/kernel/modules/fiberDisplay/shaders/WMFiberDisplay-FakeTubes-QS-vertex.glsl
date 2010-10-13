varying float tangent_dot_view;
varying vec3 tangentR3;
varying float normalDirection;
varying vec4 tubeColor;

varying vec2 imageTangent;
varying vec3 view;
varying vec3 light;
varying float NdotL;
varying vec3 normal;
varying vec3 halfVector;

varying float endPoint;

varying float z;
varying float zNear;
varying float zFar;

attribute vec2 qsTexCoords;

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

void main()
{
    vec4 pos = gl_ModelViewProjectionMatrix * gl_Vertex;
    vec3 tangent = ( gl_ModelViewProjectionMatrix * vec4(gl_Normal, 0. ) ).xyz; //!< transform our tangent vector
    gl_ClipVertex = gl_ModelViewMatrix * gl_Vertex; //!< make clipping planes work

    //scalar product between viewDir and tangent
    float tmp = dot(-pos.xyz, tangent);

    //decide wether point sprite or triangle strip to render
    // don't draw quad strip	//decide wether point sprite or triangle strip to render
    if(tmp >= 0.98 && usePointSprites)
    {
        tubeColor.a = 0.0;
    }
    else
    {
        float thickness;

        vec3 lightPos = vec3(0., 0., -1000.);
        vec3 ez = vec3(0., 0., -1.0);
        tangentR3 = gl_NormalMatrix * gl_Normal;

        if ( useDepthCueing )
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
        light = lightPos - pos;

        vec3 cameraPosition = vec3(gl_ModelViewMatrixInverse * vec4(0.0, 0.0, 0.0, 1.0));
        vec3 referencePosition = vec3(gl_ModelViewProjectionMatrix * vec4(1.0, 1.0, 1.0, 0.0));

        halfVector = normalize( -pos + light );
        vec3 binormal = normalize( cross( tangent, -pos ) );
        normal = cross( binormal, tangentR3);
        NdotL = max(dot(normal, light), 0.0);

        //define thickness of tubes
        thickness = u_thickness * 0.001; //* cross( cameraPosition, referencePosition );

        // color of tube
        tubeColor = gl_Color;

        vec3 offsetNN;
        if ( useProjection )
        {
            offsetNN = cross( (tangent.xyz), vec3(.0, .0, -1.));
        }
        else
        {
            offsetNN = cross( normalize(tangent.xyz), vec3(.0, .0, -1.));
        }
        vec3 offset = normalize(offsetNN);
        tangent_dot_view = length(offsetNN);

        thickness *= length(referencePosition);
        offset.x *= thickness;
        offset.y *= thickness;
        // transform quad points to position
        pos.xyz += offset * qsTexCoords.s; //< add offset in y-direction (eye-space)

        // TexCoord for 1D texture
        gl_TexCoord[1].s = qsTexCoords.t;
    }
    gl_Position = pos; //< store final position
}

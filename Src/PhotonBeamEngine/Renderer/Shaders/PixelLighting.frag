static const char* PixelLightingFragmentShader = STRINGIFY(

varying mediump vec3 EyespaceNormal;
varying lowp vec3 Diffuse;

uniform highp vec3 LightPosition;  // Normalized camera space.
uniform highp vec3 AmbientMaterial;
uniform highp vec3 SpecularMaterial;
uniform highp float Shininess;
uniform mediump float Opacity;

void main(void) {
    highp vec3 N = normalize(EyespaceNormal);
    highp vec3 E = vec3(0, 0, 1);
    highp vec3 H = normalize(LightPosition + E);
    
    highp float df = max(0.0, dot(N, LightPosition));
    highp float sf = max(0.0, dot(N, H));
    sf = pow(sf, Shininess);

    lowp vec3 color = AmbientMaterial + df * Diffuse + sf * SpecularMaterial;

    gl_FragColor = vec4(color, Opacity);
}

);

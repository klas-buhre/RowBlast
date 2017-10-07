static const char* TexturedLightingFragmentShader = STRINGIFY(

varying lowp vec3 DestinationColor;
varying lowp vec3 SpecularColor;
varying mediump vec2 TextureCoordOut;

uniform mediump float Opacity;
uniform sampler2D Sampler;

void main(void) {
    gl_FragColor = vec4(texture2D(Sampler, TextureCoordOut).xyz * DestinationColor + SpecularColor, Opacity);
}

);

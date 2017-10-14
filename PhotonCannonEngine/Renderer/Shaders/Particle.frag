static const char* ParticleFragmentShader = STRINGIFY(

varying mediump vec2 TextureCoordOut;
varying highp vec4 ColorOut;

uniform mediump float Opacity;
uniform sampler2D Sampler;

void main(void) {
    gl_FragColor = vec4(ColorOut.xyz, ColorOut.w * texture2D(Sampler, TextureCoordOut).a);
    // gl_FragColor = ColorOut * texture2D(Sampler, TextureCoordOut);
}

);

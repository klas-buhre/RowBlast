static const char* TextTopGradientFragmentShader = STRINGIFY(

varying highp vec2 TextureCoordOut;

uniform sampler2D Sampler;
uniform mediump vec4 TextColor;
uniform mediump vec3 TextColorSubtraction;

void main(void) {
    gl_FragColor = vec4(vec3(1.0, 1.0, 1.0) - TextColorSubtraction * (1.0 - TextureCoordOut.y), texture2D(Sampler, TextureCoordOut).a) * TextColor;
}

);
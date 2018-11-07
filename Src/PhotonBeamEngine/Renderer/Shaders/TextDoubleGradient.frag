static const char* TextDoubleGradientFragmentShader = STRINGIFY(

varying highp vec2 TextureCoordOut;

uniform sampler2D Sampler;
uniform mediump vec4 TextColor;
uniform mediump vec3 TextTopColorSubtraction;
uniform mediump vec3 TextMidColorSubtraction;

void main(void) {
    if (TextureCoordOut.y > 0.5) {
        gl_FragColor = vec4(TextColor.xyz - 2.0 * TextMidColorSubtraction * (1.0 - TextureCoordOut.y), texture2D(Sampler, TextureCoordOut).a * TextColor.w);
    } else {
        gl_FragColor = vec4(vec3(1.0, 1.0, 1.0) - 2.0 * TextTopColorSubtraction * (0.5 - TextureCoordOut.y), texture2D(Sampler, TextureCoordOut).a * TextColor.w);
    }
}

);

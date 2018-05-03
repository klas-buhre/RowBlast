static const char* TextGradientVertexShader = STRINGIFY(

attribute vec4 TextCoords;

uniform mat4 Projection;

varying vec2 TextureCoordOut;

void main(void) {
    gl_Position = Projection * vec4(TextCoords.xy, 0.0, 1.0);
    TextureCoordOut = TextCoords.zw;
}
    
);

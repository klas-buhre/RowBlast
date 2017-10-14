static const char* ParticleVertexShader = STRINGIFY(

attribute vec4 Position;
attribute vec2 TextureCoord;
attribute vec4 Color;

uniform mat4 Projection;
uniform mat4 Modelview;

varying vec2 TextureCoordOut;
varying vec4 ColorOut;

void main(void) {
    gl_Position = Projection * Modelview * Position;
    TextureCoordOut = TextureCoord;
    ColorOut = Color;
}

);

static const char* PointParticleVertexShader = STRINGIFY(

attribute vec4 Position;
attribute vec4 Color;
attribute float PointSize;

uniform mat4 Projection;
uniform mat4 Modelview;

varying vec4 ColorOut;

void main(void) {
    gl_Position = Projection * Modelview * Position;
    gl_PointSize = PointSize;
    ColorOut = Color;
}

);

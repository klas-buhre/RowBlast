static const char* PixelLightingVertexShader = STRINGIFY(

attribute vec4 Position;
attribute vec3 Normal;

uniform mat4 ModelViewProjection;
uniform mat3 NormalMatrix;
uniform vec3 DiffuseMaterial;

varying vec3 EyespaceNormal;
varying vec3 Diffuse;

void main(void) {
    EyespaceNormal = normalize(NormalMatrix * Normal);
    Diffuse = DiffuseMaterial;
    gl_Position = ModelViewProjection * Position;
}

);

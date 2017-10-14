static const char* EnvMapVertexShader = STRINGIFY(

attribute vec4 Position;
attribute vec3 Normal;

uniform mat4 Projection;
uniform mat4 Modelview;
uniform mat4 Model;
uniform mat3 Model3x3;
uniform mat3 NormalMatrix;
uniform vec3 LightPosition;  // World space.
uniform vec3 CameraPosition; // World space.
uniform vec3 AmbientMaterial;
uniform vec3 DiffuseMaterial;
uniform vec3 SpecularMaterial;
uniform float Shininess;

varying vec3 DestinationColor;
varying vec3 SpecularColor;
varying vec3 ReflectDir;

void main(void) {
    gl_Position = Projection * Modelview * Position;

    // Vertex lighting calculations in camera space:
    vec3 N = normalize(NormalMatrix * Normal);
    vec3 L = LightPosition;
    vec3 E = vec3(0, 0, 1);
    vec3 H = normalize(L + E);

    float df = max(0.0, dot(N, L));
    float sf = max(0.0, dot(N, H));
    sf = pow(sf, Shininess);

    DestinationColor = AmbientMaterial + df * DiffuseMaterial;
    SpecularColor = sf * SpecularMaterial;

    // Cube map calculations in world space:
    vec3 normalWorldSpace = normalize(Model3x3 * Normal);
    vec3 positionWorldSpace = vec3(Model * Position);
    
    vec3 I = normalize(positionWorldSpace - CameraPosition);
    ReflectDir = reflect(I, normalWorldSpace);
}

);

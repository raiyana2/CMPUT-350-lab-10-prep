#version 120 // glsl version 1.2 for OpenGL 2.1

uniform sampler2D texture;
uniform vec2 texelSize;

void main() {
    vec2 uv = gl_TexCoord[0].xy;
    vec4 color = vec4(0.0);
    float totalWeight = 0.0;

    for (int x = -2; x <= 2; ++x) {
        for (int y = -2; y <= 2; ++y) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            float weight = 1.0 / (abs(float(x)) + abs(float(y)) + 1.0);
            color += texture2D(texture, uv + offset) * weight;
            totalWeight += weight;
        }
    }

    gl_FragColor = color / totalWeight;
}

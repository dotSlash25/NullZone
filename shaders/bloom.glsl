#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec2 resolution;   // pass window or rendertexture size
uniform float threshold;   // e.g. 0.5
uniform float intensity;   // e.g. 0.8

out vec4 finalColor;

float brightness(vec3 c) {
    return dot(c, vec3(0.299, 0.587, 0.114)); // luminance formula
}

void main() {
    vec2 texel = 1.0 / resolution;
    vec3 col = texture(texture0, fragTexCoord).rgb;

    // Simple 9-sample blur kernel
    vec3 bloom = vec3(0.0);
    float kernel[9] = float[](0.05, 0.09, 0.12, 0.15, 0.18, 0.15, 0.12, 0.09, 0.05);

    for (int i = -4; i <= 4; i++) {
        bloom += texture(texture0, fragTexCoord + vec2(texel.x * i, 0.0)).rgb * kernel[i+4];
        bloom += texture(texture0, fragTexCoord + vec2(0.0, texel.y * i)).rgb * kernel[i+4];
    }

    vec3 result = col + bloom * intensity;
    finalColor = vec4(result, 1.0) * colDiffuse * fragColor;
}

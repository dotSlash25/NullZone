#version 330

in vec2 fragTexCoord;
in vec4 fragColour;

uniform sampler2D inputTexture;

out vec4 finalColour;

float threshold = 0.4;

void main() {
    vec4 texel = texture(inputTexture, fragTexCoord);
    if (texel.x + texel.y + texel.z > 3 * threshold) {
        finalColour = texel;
    } else {
        finalColour = vec4(0.0, 0.0, 0.0, 1.0);
    }
}

#version 330

in vec2 fragTexCoord;
in vec4 fragColour;

uniform sampler2D inputTexture;

out vec4 finalColour;

void main() {
    vec4 texel = texture(inputTexture, fragTexCoord);
    if (texel.a > 0.9) {
        finalColour = vec4(1.0, 1.0, 1.0, 0.8);//temporary
    } else {
        discard;
    }
}
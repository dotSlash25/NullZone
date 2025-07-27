#version 330

in vec2 fragTexCoord;
out vec4 finalColour;

uniform vec2 resolution = vec2(1200.0, 900.0);
float strength = 0.9;

void main() {
    float dis = distance(gl_FragCoord.xy / resolution, vec2(0.5, 0.5));
    dis = pow(dis, 1.5);
    finalColour = vec4(0.0, 0.0, 0.0, dis);
}

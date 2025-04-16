#version 330

in vec2 fragTexCoord;
out vec4 finalColour;

vec2 resolution = vec2(800.0, 600.0);
float strength = 0.9;

void main() {
    float dis = distance(gl_FragCoord.xy / resolution, vec2(0.5, 0.5));
    dis = pow(dis, 1.5);
    finalColour = vec4(0.0, 0.0, 0.0, dis);
}

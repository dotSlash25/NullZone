#version 330 core

uniform vec2 mpos;
uniform vec2 res;
out vec4 fragColour;

void main() {
    vec2 mouse = vec2(mpos.x/res.x, 1-mpos.y/res.x);
    vec3 circle = vec3(mouse.xy, 0.1);
    float alpha = distance(circle.xy, vec2(gl_FragCoord.x/res.x, gl_FragCoord.y/res.x));
    alpha = smoothstep(circle.z, circle.z + 0.05, alpha);
    fragColour = vec4(0., 0., 0., alpha);
}
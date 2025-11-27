#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec2 resolution; // (renderWidth, renderHeight)

out vec4 finalColor;


uniform float strength = 1.0;

vec3 toLinear(vec3 c) {
    return pow(c, vec3(2.2));
}

vec3 toSRGB(vec3 c) {
    return pow(c, vec3(1.0 / 2.2));
}

void main() {
    // vec2 texel = 1.0 / resolution;
    // vec3 color = toLinear(texture(texture0, fragTexCoord).rgb) * weight[0] * strength;

    // for (int i = 1; i < 5; i++) {
    //     color += toLinear(texture(texture0, fragTexCoord + vec2(offset[i] * texel.x, 0)).rgb) * weight[i] * strength;
    //     color += toLinear(texture(texture0, fragTexCoord - vec2(offset[i] * texel.x, 0)).rgb) * weight[i] * strength;
    //     color += toLinear(texture(texture0, fragTexCoord + vec2(0, offset[i] * texel.y)).rgb) * weight[i] * strength;
    //     color += toLinear(texture(texture0, fragTexCoord - vec2(0, offset[i] * texel.y)).rgb) * weight[i] * strength;
    // }

    // // Convert back to sRGB before output
    // finalColor = vec4(toSRGB(color), 1.0) * colDiffuse * fragColor;
    if (strength < 0.8) {
        finalColor = texture(texture0, fragTexCoord);
        return;
    }
    float xs = resolution.x; float ys = resolution.y;
    vec2 pos = 2 * fragTexCoord - vec2(-1.0, -1.0);
    float x,y,xx,yy,rr=strength*strength,dx,dy,w,w0;
    float totalWeight = 0.0;
    w0=0.3780/pow(strength,1.975);
    vec2 p;
    vec4 col=vec4(0.0,0.0,0.0,0.0);
    for (dx=1.0/xs,x=-strength,p.x=0.5+(pos.x*0.5)+(x*dx);x<=strength;x++,p.x+=dx){ xx=x*x;
     for (dy=1.0/ys,y=-strength,p.y=0.5+(pos.y*0.5)+(y*dy);y<=strength;y++,p.y+=dy){ yy=y*y;
      if (xx+yy<=rr)
        {
        w=w0*exp((-xx-yy)/(2.0*rr));
        vec4 texCol = texture(texture0, p);
        if (texCol.a > 0.5) {
            col+=texture(texture0, p)*w;
            totalWeight += w;
        }
        }}}
    finalColor=col/totalWeight;
}

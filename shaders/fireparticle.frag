#version 120

// Interpolated values from the vertex shaders
varying vec2 UV;
varying float frame;

uniform sampler2D myTextureSampler;

void main() {
    vec2 xy;
    float i = floor(frame * 64.0);

    float row = floor(i / 8.0);
    float col = floor(mod(i, 8.0));

    xy.x = (UV.x / 8.0) +  row / 8.0;
    xy.y = (UV.y / 8.0) +  col / 8.0;
    vec3 color = texture2D( myTextureSampler, xy ).rgb;

    if (color.r < 0.1 && color.g < 0.1 && color.b < 0.1) {
        gl_FragColor = vec4(color, 0.0); 
    } else {
    	gl_FragColor = vec4(color, 0.8);
    }
}

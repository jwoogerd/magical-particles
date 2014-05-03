#version 120

// Interpolated values from the vertex shaders
varying vec2 UV;

uniform sampler2D myTextureSampler;

void main() {

    vec3 color = texture2D( myTextureSampler, vec2(1.0 - UV.x, 1 - UV.y)).rgb;

    if (color.r > 0.7 && color.b < 0.3 && color.g < 0.3) {
        gl_FragColor = vec4(color, 0.0); 
    } else {
    	gl_FragColor = vec4(color, 0.8);
    }
}

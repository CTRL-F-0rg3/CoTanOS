#version 330 core
in vec2 vUv;
out vec4 FragColor;

uniform sampler2D uAtlas;
uniform vec3  uColor;
uniform float uFade;
uniform float uTime;

void main() {
    float a = texture(uAtlas, vUv).r;
    float pulse = 0.9 + 0.1 * sin(uTime * 1.5);
    vec3 col = uColor * pulse;
    FragColor = vec4(col, a * uFade);
}
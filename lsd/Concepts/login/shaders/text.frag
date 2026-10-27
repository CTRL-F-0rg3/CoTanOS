#version 330 core
in vec2 vUv;
out vec4 FragColor;
uniform sampler2D uAtlas;
uniform vec3  uColor;
uniform float uAlpha;
void main() {
    float a = texture(uAtlas, vUv).r;
    FragColor = vec4(uColor, a * uAlpha);
}

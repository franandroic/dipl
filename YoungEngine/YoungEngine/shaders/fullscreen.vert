#version 450

layout(location = 0) out vec2 uv;

void main() {

    //https://wallisc.github.io/rendering/2021/04/18/Fullscreen-Pass.html
    uv = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    gl_Position = vec4(uv * 2.0 - 1.0, 0.0, 1.0);
}

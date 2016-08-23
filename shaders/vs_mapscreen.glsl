#version 430 core

out VDataBlock {
  vec2 texCoord;
} OUT;

void main() {
  OUT.texCoord.s = (gl_VertexID << 1) & 2;
  OUT.texCoord.t = gl_VertexID & 2;

  gl_Position = vec4(2.0f * OUT.texCoord - 1.0f, 0.0f, 1.0f);
}
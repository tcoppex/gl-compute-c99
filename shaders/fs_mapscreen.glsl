#version 430 core

uniform sampler2D uInput;

in VDataBlock {
  vec2 texCoord;
} IN;

layout(location = 0) out vec4 fragColor;

void main() {
  fragColor = vec4(1.0, 0.7, 0.2, 1.0);

  vec4 diffuse = texture(uInput, IN.texCoord);
  fragColor = diffuse;
}
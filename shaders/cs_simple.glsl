#version 440 core

uniform float uTime = 1.0f;

layout(rgba8) writeonly uniform image2D uOutputImg;

layout(local_size_x = 32, local_size_y = 32) in;

vec3 renderEffect(vec2 pos);

void main() {
  const ivec3 gridDim   = ivec3(gl_NumWorkGroups);
  const ivec3 blockDim  = ivec3(gl_WorkGroupSize);
  const ivec3 blockIdx  = ivec3(gl_WorkGroupID);
  const ivec3 threadPos = ivec3(gl_GlobalInvocationID);
  const ivec3 threadIdx = ivec3(gl_LocalInvocationID);
  const int   tid       = int(gl_LocalInvocationIndex);

  vec2 pos = (threadPos.xy / 256.0);
  vec3 color = renderEffect(2*pos);

  imageStore(uOutputImg, threadPos.xy, vec4(color, 1.0f));
}

// =========================================================

// Noise and fbm functions by Inigo Quilez

mat3 m = mat3(0.00,  0.80,  0.60,
              -0.80,  0.36, -0.48,
              -0.60, -0.48,  0.64);

float hash(float n) {
    return fract(sin(n)*43758.5453);
}


float noise(in vec3 x) {
    vec3 p = floor(x);
    vec3 f = fract(x);

    f = f*f*(3.0-2.0*f);

    float n = p.x + p.y*57.0 + 113.0*p.z;

    float res = mix(mix(mix(hash(n+  0.0), hash(n+  1.0),f.x),
                        mix(hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
                    mix(mix(hash(n+113.0), hash(n+114.0),f.x),
                        mix(hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
    return res;
}

float fbm(vec3 p) {
    float f = 0.0;

    f += 0.5000*noise(p); p = m*p*2.02;
    f += 0.2500*noise(p); p = m*p*2.03;
    f += 0.1250*noise(p); p = m*p*2.01;
    f += 0.0625*noise(p);

    return f/0.9375;
}


float fbm(vec2 p) {
  return fbm(vec3(p, uTime*3.f));
}


vec3 renderEffect(vec2 pos) {
  vec2 p = pos / 4.0f + uTime;


  float f1 = (1.0f + sin(uTime));

  vec2 q = vec2(fbm(p + vec2(-1.0f, 3.0f)*f1),
                fbm(p + vec2(4.0f, 1.0f)));


  vec3 color;
  color.r = 2.0f*q.x*q.x;
  color.g = 5.0f*q.y*q.y*q.x;
  color.b = q.y;

  float n = fbm(p);
  color *= 2.2f * n;

  return smoothstep(0.0f, 1.0f, color);
}

// =========================================================
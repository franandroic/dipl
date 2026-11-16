#version 450
#define MAT3_ROWS(a, b, c, d, e, f, g, h, i) mat3(a, d, g, b, e, h, c, f, i)

layout(set = 1, binding = 0) uniform sampler2D normalMask;
layout(set = 1, binding = 1) uniform sampler2D colorMask;

layout(location = 0) in vec2 uv;

layout(location = 0) out vec4 outColor;

const vec3 LINE_COLOR = vec3(0.34, 0.1, 0.04);
const vec3 PAPER_COLOR = vec3(1.0, 1.0, 1.0);

const float STEP = 1.0;
const float MIN_VAL = 0.01;
const float MAX_VAL = 0.03;
const float COLOR_W = 0.6;
const float NORMAL_W = 0.3;

const float GRAIN_GAIN = 10.0;
const float CLOUD_SCALE = 4.0;
const float COLOR_WOBBLE = 1.0 / 40.0;
const float NORMAL_WOBBLE = 1.0 / 50.0;

const mat3 Gx = MAT3_ROWS(
    -1.0, 0.0, 1.0,
    -2.0, 0.0, 2.0,
    -1.0, 0.0, 1.0
);

const mat3 Gy = MAT3_ROWS(
    -1.0, -2.0, -1.0,
     0.0,  0.0,  0.0,
     1.0,  2.0,  1.0
);

//https://en.wikipedia.org/wiki/Grayscale#Converting_color_to_grayscale
const vec3 rgbToGrayscale = vec3(0.299, 0.587, 0.114);

//https://www.shadertoy.com/view/XdXGW8
vec2 grad( ivec2 z ) {

    // 2D to 1D
    int n = z.x+z.y*11111;

    // Hugo Elias hash
    n = (n<<13)^n;
    n = (n*(n*n*15731+789221)+1376312589)>>16;

    // Perlin style vectors
    n &= 7;
    vec2 gr = vec2(n&1,n>>1)*2.0-1.0;
    return ( n>=6 ) ? vec2(0.0,gr.x) : 
           ( n>=4 ) ? vec2(gr.x,0.0) :
                              gr;                         
}

float noise( in vec2 p ) {
    ivec2 i = ivec2(floor( p ));
    vec2 f = fract( p );
	
	vec2 u = f*f*(3.0-2.0*f);

    return mix( mix( dot( grad( i+ivec2(0,0) ), f-vec2(0.0,0.0) ), 
                     dot( grad( i+ivec2(1,0) ), f-vec2(1.0,0.0) ), u.x),
                mix( dot( grad( i+ivec2(0,1) ), f-vec2(0.0,1.0) ), 
                     dot( grad( i+ivec2(1,1) ), f-vec2(1.0,1.0) ), u.x), u.y);
}

//https://tympanus.net/codrops/2022/11/29/sketchy-pencil-effect-with-three-js-post-processing/
float cloud(vec2 p) {

    float value = 0.5;
    float amplitude = 0.5;

    for (int i = 0; i < 4; i++) {
        value += amplitude * noise(p);
        p *= 2.0;
        amplitude *= 0.5;
    }

    return clamp(value, 0.0, 1.0);
}

//globals, to avoid having to pass to function
vec2 colorUV;
vec2 normalUV;
float grain;

float valueAtPoint(float dx, float dy) {

    vec4 lit = texture(colorMask, colorUV + vec2(dx, dy));
    vec3 normal = texture(normalMask, normalUV + vec2(dx, dy)).rgb;

    float colorValue = dot(lit.rgb + PAPER_COLOR * (1.0 - lit.a), rgbToGrayscale);
    float normalValue = dot(normal * 0.5 + 0.5, rgbToGrayscale);

    return colorValue * COLOR_W + normalValue * NORMAL_W * grain;
}

void main() {

    vec2 size = vec2(textureSize(colorMask, 0));
    vec2 texel = STEP / size;
    float dx = texel.x;
    float dy = texel.y;

    float wobble = cloud(vec2(uv.x * size.x / size.y, uv.y) * CLOUD_SCALE) - 0.5;
    colorUV = uv + wobble * COLOR_WOBBLE;
    normalUV = uv + wobble * NORMAL_WOBBLE;

    grain = (noise(gl_FragCoord.xy) * 2.0 - 1.0) * GRAIN_GAIN;

    mat3 window = MAT3_ROWS(
        valueAtPoint(-dx, -dy), valueAtPoint(0.0, -dy), valueAtPoint(dx, -dy),
        valueAtPoint(-dx, 0.0), valueAtPoint(0.0, 0.0), valueAtPoint(dx, 0.0),
        valueAtPoint(-dx,  dy), valueAtPoint(0.0,  dy), valueAtPoint(dx,  dy)
    );

    float gx = dot(Gx[0], window[0]) + dot(Gx[1], window[1]) + dot(Gx[2], window[2]);
    float gy = dot(Gy[0], window[0]) + dot(Gy[1], window[1]) + dot(Gy[2], window[2]);

    float mag = gx * gx + gy * gy;
    float edge = smoothstep(MIN_VAL, MAX_VAL, mag);

    float coverage = texture(colorMask, colorUV).a;
    float alpha = edge + coverage * (1.0 - edge);
    vec3 color = (LINE_COLOR * edge + PAPER_COLOR * coverage * (1.0 - edge)) / max(alpha, 1e-5);

    outColor = vec4(color, alpha);
}

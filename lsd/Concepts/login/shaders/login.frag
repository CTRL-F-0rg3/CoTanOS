#version 330 core
out vec4 FragColor;

uniform float uTime;
uniform vec2  uResolution;
uniform vec3  uAccent;
uniform float uDotCount;
uniform float uFocusPulse;
uniform float uFlash;

uniform float uSidebarW;

uniform vec2  uBrandStar;
uniform float uBrandStarR;

uniform vec2  uUser1CardC, uUser1CardH;
uniform vec2  uUser1Dot;
uniform vec2  uUser2CardC, uUser2CardH;
uniform vec2  uUser2Dot;

uniform vec2  uPowerCardC, uPowerCardH;
uniform vec2  uPowerIconBase;
uniform float uPowerSpacing;

uniform vec2  uCardC, uCardH;
uniform vec2  uAvatarC;
uniform float uAvatarR;
uniform vec2  uFieldC, uFieldH;

#define FBM_ITERS 5
#define PI 3.14159265359
#define TAU 6.28318530718

float hash21(vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}
float noise(vec2 p) {
    vec2 i = floor(p), f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    float a = hash21(i), b = hash21(i + vec2(1,0));
    float c = hash21(i + vec2(0,1)), d = hash21(i + vec2(1,1));
    return mix(mix(a,b,u.x), mix(c,d,u.x), u.y);
}
float fbm(vec2 p) {
    float v = 0.0, a = 0.5;
    for (int i = 0; i < FBM_ITERS; i++) { v += a * noise(p); p = p*2.03+17.0; a *= 0.5; }
    return v;
}
float sdRoundBox(vec2 p, vec2 b, float r) {
    vec2 q = abs(p) - b + r;
    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r;
}
float sdStarSmall(vec2 p, float r, float sharp) {
    float ang = atan(p.x, p.y);
    float m = mod(ang, TAU / 5.0) - PI / 5.0;
    float k = sharp + (1.0 - sharp) * (abs(m) / (PI / 5.0));
    return length(p) - r * k;
}

void main() {
    vec2 uv = gl_FragCoord.xy / uResolution;
    float aspect = uResolution.x / uResolution.y;
    vec2 p = (uv - 0.5) * vec2(aspect, 1.0);

    // ==== nebula (bez panelu sidebaru — całe tło jednakowe) ====
    vec3 col = vec3(0.008, 0.008, 0.022);
    vec2 q = p * 1.3;
    vec2 warp = vec2(fbm(q + vec2(0.0, uTime * 0.008)),
                     fbm(q + vec2(5.2, uTime * 0.012)));
    q += warp * 0.4;
    vec2 rr = vec2(fbm(q + 3.1), fbm(q + 8.7));
    float neb = smoothstep(0.35, 0.85, fbm(q + 2.0 * rr));
    vec3 nebP = vec3(0.28, 0.10, 0.50);
    vec3 nebK = vec3(0.75, 0.20, 0.55);
    vec3 nebC = vec3(0.15, 0.55, 0.85);
    float c1 = smoothstep(0.3, 0.7, fbm(q * 0.7 + 10.0));
    vec3 nebCol = mix(nebP, nebK, c1);
    nebCol = mix(nebCol, nebC, smoothstep(0.5, 0.9, fbm(q * 0.4 + 20.0)));
    float radial = smoothstep(1.2, 0.0, length(p));
    col += nebCol * neb * (0.4 + 0.6 * radial) * 0.65;

    // stars
    vec2 s1 = uv * vec2(aspect, 1.0) * 220.0;
    vec2 f1 = fract(s1) - 0.5;
    float h1 = hash21(floor(s1));
    float tw1 = 0.6 + 0.4 * sin(uTime * 1.5 + h1 * 100.0);
    col += vec3(exp(-dot(f1,f1) * 22.0) * step(0.988, h1) * tw1 * 0.8);
    vec2 s2 = uv * vec2(aspect, 1.0) * 600.0;
    vec2 f2 = fract(s2) - 0.5;
    float h2 = hash21(floor(s2));
    col += vec3(exp(-dot(f2,f2) * 18.0) * step(0.994, h2) * 0.5);

    // ==== brand star (bez boxa, bez panelu) ====
    {
        vec2 sp = (uv - uBrandStar) * vec2(aspect, 1.0);
        float d = sdStarSmall(sp, uBrandStarR, 0.30);
        col += uAccent * exp(-abs(d) * 70.0) * 0.85;
        float m = smoothstep(0.0015, 0.0, d);
        col = mix(col, mix(uAccent, vec3(1.0), 0.55), m);
    }

    // ==== user 1 card (ctrl) — tylko border ====
    {
        vec2 d = (uv - uUser1CardC) * vec2(aspect, 1.0);
        vec2 h = uUser1CardH * vec2(aspect, 1.0);
        float dd = sdRoundBox(d, h, 0.012);
        col = mix(col, uAccent * 0.85, smoothstep(0.0018, 0.0, abs(dd)));
    }
    {
        vec2 d = (uv - uUser1Dot) * vec2(aspect, 1.0);
        col = mix(col, uAccent, smoothstep(0.0012, 0.0, length(d) - 0.008));
    }

    // ==== user 2 card (guest) — tylko border ====
    {
        vec2 d = (uv - uUser2CardC) * vec2(aspect, 1.0);
        vec2 h = uUser2CardH * vec2(aspect, 1.0);
        float dd = sdRoundBox(d, h, 0.012);
        col = mix(col, uAccent * 0.35, smoothstep(0.0015, 0.0, abs(dd)));
    }
    {
        vec2 d = (uv - uUser2Dot) * vec2(aspect, 1.0);
        col = mix(col, vec3(0.35, 0.42, 0.52), smoothstep(0.0012, 0.0, length(d) - 0.008));
    }

    // ==== power row card — tylko border ====
    {
        vec2 d = (uv - uPowerCardC) * vec2(aspect, 1.0);
        vec2 h = uPowerCardH * vec2(aspect, 1.0);
        float dd = sdRoundBox(d, h, 0.012);
        col = mix(col, uAccent * 0.35, smoothstep(0.0015, 0.0, abs(dd)));
    }

    // power icons
    {
        for (int i = 0; i < 3; i++) {
            vec2 pc = uPowerIconBase + vec2((float(i) - 1.0) * uPowerSpacing, 0.0);
            vec2 pp = (uv - pc) * vec2(aspect, 1.0);
            float r = 0.017;

            col = mix(col, uAccent * 0.85, smoothstep(0.0018, 0.0, abs(length(pp) - r)) * 0.9);

            if (i == 0) {
                float dBar = sdRoundBox(pp - vec2(0.0, r * 0.50), vec2(0.0010, r * 0.55), 0.0005);
                col = mix(col, uAccent, smoothstep(0.0010, 0.0, dBar));
            } else if (i == 1) {
                float ang = atan(pp.y, pp.x);
                float gapMask = step(0.55, abs(ang - 1.57));
                col = mix(col, uAccent,
                          smoothstep(0.0018, 0.0, abs(length(pp) - r)) * gapMask);
                float dArrow = length(pp - vec2(0.62, 0.62) * r) - 0.005;
                col = mix(col, uAccent, smoothstep(0.0012, 0.0, dArrow));
            } else {
                float d1 = length(pp) - r;
                float d2 = length(pp - vec2(r * 0.55, 0.0)) - r * 0.82;
                col = mix(col, uAccent, smoothstep(0.0014, 0.0, max(d1, -d2)));
            }
        }
    }

    // ==== LOGIN CARD ====
    vec2 cc = (uv - uCardC) * vec2(aspect, 1.0);
    vec2 ch = uCardH * vec2(aspect, 1.0);
    float dCard = sdRoundBox(cc, ch, 0.022);
    col += uAccent * exp(-abs(dCard - 0.015) * 25.0) * 0.06;
    vec3 cardBg = mix(vec3(0.045, 0.055, 0.10), vec3(0.065, 0.085, 0.14), uv.y);
    col = mix(col, cardBg, smoothstep(0.0015, 0.0, dCard));
    col = mix(col, uAccent * 0.45, smoothstep(0.0020, 0.0, abs(dCard)) * 0.85);

    // ==== AVATAR ====
    vec2 avP = (uv - uAvatarC) * vec2(aspect, 1.0);
    float dAv = length(avP) - uAvatarR;
    col += uAccent * exp(-abs(dAv) * 10.0) * 0.35;
    col = mix(col, mix(vec3(0.07, 0.09, 0.17), vec3(0.11, 0.15, 0.26), 0.5),
              smoothstep(0.0015, 0.0, dAv));
    col = mix(col, uAccent, smoothstep(0.0022, 0.0, abs(dAv)));

    // ==== FIELD ====
    vec2 fd = (uv - uFieldC) * vec2(aspect, 1.0);
    vec2 fhd = uFieldH * vec2(aspect, 1.0);
    float dField = sdRoundBox(fd, fhd, 0.012);
    col = mix(col, vec3(0.05, 0.07, 0.13), smoothstep(0.0015, 0.0, dField));
    col = mix(col, mix(uAccent * 0.5, uAccent, uFocusPulse),
              smoothstep(0.0018, 0.0, abs(dField)));

    // ==== DOTS ====
    int ndots = int(uDotCount);
    float spacing = 0.0180;
    float dotR = 0.0055;
    for (int i = 0; i < 20; i++) {
        if (i >= ndots) break;
        float startX = -(float(ndots - 1)) * spacing * 0.5;
        float dx = startX + float(i) * spacing;
        float dd = length(fd - vec2(dx, 0.0)) - dotR;
        col = mix(col, vec3(0.92, 0.95, 1.0), smoothstep(0.0012, 0.0, dd));
    }
    if (ndots == 0) {
        float blink = step(0.5, fract(uTime * 1.2));
        float dCur = sdRoundBox(fd, vec2(0.0012, fhd.y * 0.45), 0.0005);
        col = mix(col, uAccent * blink, smoothstep(0.0008, 0.0, dCur));
    }

    // flash
    col += uFlash * uAccent * 0.4;

    // dither
    float dither = fract(sin(dot(gl_FragCoord.xy, vec2(12.9898, 78.233))) * 43758.5453);
    col += (dither - 0.5) / 255.0;

    FragColor = vec4(col, 1.0);
}
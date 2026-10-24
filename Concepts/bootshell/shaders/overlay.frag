#version 330 core
out vec4 FragColor;

uniform float uProgress;
uniform float uTime;
uniform float uFade;
uniform vec2  uResolution;
uniform vec3  uAccent;

#define FBM_ITERS 5
#define PI 3.14159265359
#define TAU 6.28318530718

float hash21(vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);
    float a = hash21(i);
    float b = hash21(i + vec2(1.0, 0.0));
    float c = hash21(i + vec2(0.0, 1.0));
    float d = hash21(i + vec2(1.0, 1.0));
    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

float fbm(vec2 p) {
    float v = 0.0;
    float a = 0.5;
    for (int i = 0; i < FBM_ITERS; i++) {
        v += a * noise(p);
        p = p * 2.03 + 17.0;
        a *= 0.5;
    }
    return v;
}

float sdSegment(vec2 p, vec2 a, vec2 b) {
    vec2 pa = p - a;
    vec2 ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return length(pa - ba * h);
}

float sdStarOutline(vec2 p, float R, float rIn) {
    float d = 1e9;
    for (int i = 0; i < 10; i++) {
        float a1 = float(i) * PI / 5.0;
        float r1 = (mod(float(i), 2.0) == 0.0) ? R : rIn;
        float a2 = float(i + 1) * PI / 5.0;
        float r2 = (mod(float(i + 1), 2.0) == 0.0) ? R : rIn;
        vec2 v1 = r1 * vec2(sin(a1), cos(a1));
        vec2 v2 = r2 * vec2(sin(a2), cos(a2));
        d = min(d, sdSegment(p, v1, v2));
    }
    return d;
}

// filled 5-point star SDF (Inigo Quilez)
float sdStar5(vec2 p, float r, float rf) {
    const vec2 k1 = vec2(0.809016994375, -0.587785252292);
    const vec2 k2 = vec2(-k1.x, k1.y);
    p.x = abs(p.x);
    p -= 2.0 * max(dot(k1, p), 0.0) * k1;
    p -= 2.0 * max(dot(k2, p), 0.0) * k2;
    p.x = abs(p.x);
    p.y -= r;
    vec2 ba = rf * vec2(-k1.y, k1.x) - vec2(0.0, 1.0);
    float h = clamp(dot(p, ba) / dot(ba, ba), 0.0, r);
    return length(p - ba * h) * sign(p.y * ba.x - p.x * ba.y);
}

void main() {
    vec2 uv = gl_FragCoord.xy / uResolution;
    float aspect = uResolution.x / uResolution.y;
    vec2 p = (uv - 0.5) * vec2(aspect, 1.0);

    vec3 col = vec3(0.008, 0.008, 0.022);

    // ===== NEBULA =====
    vec2 q = p * 1.6;
    vec2 warp = vec2(fbm(q + vec2(0.0, uTime * 0.02)),
                     fbm(q + vec2(5.2, uTime * 0.03)));
    q += warp * 0.35;
    vec2 rr = vec2(fbm(q + 3.1), fbm(q + 8.7));
    float neb = smoothstep(0.35, 0.85, fbm(q + 2.0 * rr));

    vec3 nebPurple = vec3(0.28, 0.10, 0.50);
    vec3 nebPink   = vec3(0.75, 0.20, 0.55);
    vec3 nebCyan   = vec3(0.15, 0.55, 0.85);

    float c1 = smoothstep(0.3, 0.7, fbm(q * 0.7 + 10.0));
    vec3 nebColor = mix(nebPurple, nebPink, c1);
    nebColor = mix(nebColor, nebCyan,
                   smoothstep(0.5, 0.9, fbm(q * 0.4 + 20.0)));

    float radial = smoothstep(1.2, 0.0, length(p));
    col += nebColor * neb * (0.5 + 0.5 * radial) * 0.9;

    // ===== STARS =====
    vec2 s1 = uv * vec2(aspect, 1.0) * 220.0;
    vec2 f1 = fract(s1) - 0.5;
    float h1 = hash21(floor(s1));
    float s1on = step(0.985, h1);
    float tw1 = 0.6 + 0.4 * sin(uTime * 2.5 + h1 * 100.0);
    col += vec3(exp(-dot(f1, f1) * 22.0) * s1on * tw1 * 0.9);

    vec2 s2 = uv * vec2(aspect, 1.0) * 600.0;
    vec2 f2 = fract(s2) - 0.5;
    float h2 = hash21(floor(s2));
    col += vec3(exp(-dot(f2, f2) * 18.0) * step(0.994, h2) * 0.5);

    // ===== LOGO: ostra gwiazda (jeż) + 2 orbity =====
    vec2 logoCenterP = vec2(0.0, 0.20);
    vec2 lp = p - logoCenterP;

    float bgGlow = exp(-dot(lp, lp) * 25.0);
    col += uAccent * bgGlow * 0.18 * (0.85 + 0.15 * sin(uTime * 1.2));

    // --- Orbita 1 ---
    float tiltA = -0.28;
    float ca = cos(tiltA), sa = sin(tiltA);
    vec2 op1 = vec2(lp.x * ca - lp.y * sa, lp.x * sa + lp.y * ca);
    float orb1A = 0.145, orb1B = 0.040;
    float dOrb1 = length(vec2(op1.x / orb1A, op1.y / orb1B)) - 1.0;
    dOrb1 *= orb1B;
    float orb1Mask = smoothstep(0.0015, 0.0, abs(dOrb1));
    float back1  = step(0.0, op1.y);
    float front1 = step(op1.y, 0.0);

    // --- Orbita 2 ---
    float tiltB = 1.05;
    float cb = cos(tiltB), sb = sin(tiltB);
    vec2 op2 = vec2(lp.x * cb - lp.y * sb, lp.x * sb + lp.y * cb);
    float orb2A = 0.090, orb2B = 0.030;
    float dOrb2 = length(vec2(op2.x / orb2A, op2.y / orb2B)) - 1.0;
    dOrb2 *= orb2B;
    float orb2Mask = smoothstep(0.0012, 0.0, abs(dOrb2));
    float back2  = step(0.0, op2.y);
    float front2 = step(op2.y, 0.0);

    // tylne połówki orbit
    col = mix(col, uAccent * 0.40, orb1Mask * back1);
    col = mix(col, uAccent * 0.30, orb2Mask * back2);

    // --- Gwiazda centralna: OSTRA (rf 0.45 → 0.26) ---
    float starR = 0.042;          // większa, bo ostrzejsze tipsy mają mniej masy
    float dStar = sdStar5(lp, starR, 0.26);
    float starMask = smoothstep(0.0015, 0.0, dStar);

    col += uAccent * exp(-abs(dStar) * 70.0) * 0.55;
    vec3 starColor = mix(uAccent, vec3(1.0), 0.55);
    col = mix(col, starColor, starMask);

    // jasny rdzeń – mały, kontrastuje z ostrymi ramionami
    float starCore = exp(-length(lp) * 110.0);
    col += vec3(1.0) * starCore * 0.30;

    // drobne poświaty na końcach ramion (5 tipów) – efekt "kolców jeża"
    for (int i = 0; i < 5; i++) {
        float a = float(i) * TAU / 5.0;
        vec2 tipDir = vec2(sin(a), cos(a));
        vec2 tipPos = tipDir * starR;
        float tipDist = length(lp - tipPos);
        col += uAccent * exp(-tipDist * tipDist * 9000.0) * 0.6;
    }

    // przednie połówki orbit
    col = mix(col, uAccent,         orb1Mask * front1);
    col = mix(col, uAccent * 0.75, orb2Mask * front2);

    // --- Księżyc 1 ---
    float m1Angle = uTime * 0.7;
    vec2 m1Local = vec2(cos(m1Angle) * orb1A, sin(m1Angle) * orb1B);
    vec2 m1 = vec2(m1Local.x * ca + m1Local.y * sa,
                  -m1Local.x * sa + m1Local.y * ca);
    float m1Dist = length(lp - m1);
    float m1Alpha = mix(1.0, 0.55, step(0.0, m1Local.y));
    col += vec3(0.9, 0.95, 1.0) * exp(-m1Dist * 120.0) * 0.8 * m1Alpha;
    col = mix(col, vec3(1.0), smoothstep(0.0045, 0.0025, m1Dist) * m1Alpha);

    // --- Księżyc 2 ---
    float m2Angle = uTime * 1.1 + 2.5;
    vec2 m2Local = vec2(cos(m2Angle) * orb2A, sin(m2Angle) * orb2B);
    vec2 m2 = vec2(m2Local.x * cb + m2Local.y * sb,
                  -m2Local.x * sb + m2Local.y * cb);
    float m2Dist = length(lp - m2);
    float m2Alpha = mix(1.0, 0.55, step(0.0, m2Local.y));
    col += vec3(0.9, 0.95, 1.0) * exp(-m2Dist * 150.0) * 0.7 * m2Alpha;
    col = mix(col, vec3(1.0), smoothstep(0.0035, 0.0018, m2Dist) * m2Alpha);

    // ===== GWIAZDA SPINNER (obrys) – też ostrzejsza =====
    vec2 starPos = (vec2(0.5, 0.15) - 0.5) * vec2(aspect, 1.0);
    vec2 sp = p - starPos;

    float R    = 0.038;
    float rIn  = R * 0.26;         // było 0.42 – teraz jeż

    float psi  = mod(PI * 0.5 - atan(sp.y, sp.x), TAU);
    float sParam = psi / TAU;

    float dOutline = sdStarOutline(sp, R, rIn);

    float strokeW = 0.0018;
    float strokeMask = smoothstep(strokeW, 0.0, dOutline);

    float headSoft = 0.008;
    float drawnMask = smoothstep(uProgress + headSoft,
                                 uProgress - headSoft, sParam);

    vec3 strokeBright = mix(uAccent, vec3(1.0), 0.15);
    vec3 strokeDim    = uAccent * 0.28;

    col = mix(col, strokeDim, strokeMask * 0.75);
    col = mix(col, strokeBright, strokeMask * drawnMask);

    float glowR = exp(-dOutline * 220.0) * drawnMask;
    col += uAccent * glowR * 0.35;

    float headAngle = uProgress * TAU;
    float headSector = mod(headAngle, TAU / 5.0) / (PI / 5.0);
    float headTri = 1.0 - abs(headSector - 1.0);
    float headR = R - (R - rIn) * headTri;
    vec2 headDir = vec2(sin(headAngle), cos(headAngle));
    vec2 headPos = headDir * headR;

    float headDist  = length(sp - headPos);
    float headDot   = exp(-headDist * headDist * 14000.0);
    float headFade  = smoothstep(0.0, 0.01, uProgress)
                    * smoothstep(1.0, 0.99, uProgress);
    float headPulse = 0.85 + 0.15 * sin(uTime * 8.0);

    col += strokeBright * headDot * headFade * headPulse * 2.2;

    float dither = fract(sin(dot(gl_FragCoord.xy, vec2(12.9898, 78.233))) * 43758.5453);
    col += (dither - 0.5) / 255.0;

    FragColor = vec4(col, uFade);
}
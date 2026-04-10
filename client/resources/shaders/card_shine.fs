#version 100

precision highp float;

varying vec2 fragTexCoord;
varying vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform float uTime;
uniform vec2 uCardSize;

float saturate(float value)
{
    return clamp(value, 0.0, 1.0);
}

float hash12(vec2 p)
{
    vec3 p3 = fract(vec3(p.xyx) * 0.1031);
    p3 += dot(p3, p3.yzx + 33.33);
    return fract((p3.x + p3.y) * p3.z);
}

vec3 iridescent(float t)
{
    return 0.5 + 0.5 * cos(6.28318 * (t + vec3(0.00, 0.33, 0.67)));
}

void main()
{
    vec4 texelColor = texture2D(texture0, fragTexCoord) * colDiffuse * fragColor;

    vec2 uv = fragTexCoord;
    float aspect = uCardSize.y / uCardSize.x;
    vec2 p = (uv - 0.5) * vec2(1.0, aspect);

    float edgeMask = smoothstep(0.0, 0.10, uv.x) * smoothstep(0.0, 0.08, uv.y) *
        smoothstep(0.0, 0.10, 1.0 - uv.x) * smoothstep(0.0, 0.08, 1.0 - uv.y);

    float radius = length(p * vec2(1.05, 1.18));
    float centerMask = 1.0 - smoothstep(0.10, 0.84, radius);
    float fresnel = pow(saturate(1.0 - centerMask), 2.4);
    float staticGloss = centerMask * 0.05 + fresnel * 0.08;

    // Diagonal sweep: a continuous sinusoidal wave along the card diagonal.
    // The wavelength is set to the card's own projection span, so the moment
    // one peak exits one side, the next peak is already entering the other —
    // there is no time when the card has no peak on it.
    vec2 sweepDir = normalize(vec2(1.0, 0.6));
    float proj = dot(p, sweepDir);
    float projHalfSpan = 0.5 * abs(sweepDir.x) + 0.5 * abs(sweepDir.y) * aspect;
    float wavelen = 2.0 * projHalfSpan;
    float sweepFreq = 6.28318 / wavelen;
    const float sweepRate = 0.40; // peaks crossing the card per second
    // +pi/2 phase offset makes the bright peak sit at the leading card edge at
    // uTime=0, so a fresh hover begins with the sweep just entering the card
    // (visible immediately, with the full travel still ahead).
    float phaseShift = uTime * sweepRate * 6.28318 + 1.5708;
    float s = max(sin(proj * sweepFreq - phaseShift), 0.0);
    float bandSoft = pow(s, 4.0);
    float bandCore = pow(s, 40.0);
    float sweep = bandSoft * 0.22 + bandCore * 0.42;

    // Star glint riding the nearest peak of the sinusoidal sweep.
    // Peak positions solve proj*sweepFreq - phaseShift = pi/2 + 2*pi*k;
    // we pick the k that keeps the peak nearest the card center.
    float peakCoord = (1.5708 + phaseShift) / sweepFreq;
    float peakProj = mod(peakCoord + projHalfSpan, wavelen) - projHalfSpan;

    vec2 glintCenter = sweepDir * peakProj + vec2(0.0, -0.08);
    vec2 glintDelta = p - glintCenter;
    vec2 starDelta = glintDelta * 2.8;
    float glintStarA = pow(saturate(1.0 - abs(starDelta.x + starDelta.y)), 26.0);
    float glintStarB = pow(saturate(1.0 - abs(starDelta.x - starDelta.y)), 26.0);
    float glintHalo  = pow(saturate(1.0 - dot(glintDelta * 3.8, glintDelta * 3.8)), 3.0);
    float glint = (glintStarA + glintStarB) * 0.20 + glintHalo * 0.12;
    // Hide the teleport when the tracked peak hands off at the card edge.
    glint *= smoothstep(projHalfSpan, 0.15, abs(peakProj));

    // Twinkling sparkles scattered across the card surface.
    vec2 cellRes = vec2(18.0, 26.0);
    vec2 cellId = floor(uv * cellRes);
    vec2 cellUv = fract(uv * cellRes);
    float rA = hash12(cellId);
    float rB = hash12(cellId + 17.3);
    float alive = step(0.92, rA);
    float blinkRate = 3.5 + rB * 3.0;
    float blinkPhase = rB * 6.28318;
    float blink = pow(0.5 + 0.5 * sin(uTime * blinkRate + blinkPhase), 22.0);
    vec2 sparkleCenter = vec2(0.5) + 0.28 * (vec2(rA, rB) - 0.5);
    float sparkleDist = length(cellUv - sparkleCenter);
    float sparkle = alive * blink * exp(-sparkleDist * sparkleDist * 520.0);

    // Iridescent tint that slowly drifts across the card.
    float hueBase = uv.x * 0.7 + uv.y * 0.35 + uTime * 0.06;
    vec3 iridColor = iridescent(hueBase);
    vec3 warmColor = vec3(1.0, 0.99, 0.95);
    vec3 foilTint = mix(warmColor, iridColor, 0.38);

    float movingShine = sweep + glint + sparkle * 0.9;
    float shine = (staticGloss + movingShine) * edgeMask * texelColor.a;

    vec3 shineColor = mix(warmColor, foilTint, saturate(movingShine * 2.5)) * shine;

    gl_FragColor = vec4(texelColor.rgb + shineColor, texelColor.a);
}

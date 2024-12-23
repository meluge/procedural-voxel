#version 150

in vec2 fs_UV;

uniform vec3 R;
uniform vec3 F;
uniform vec3 U;
uniform float aspect;
uniform vec3 eye;

uniform vec4 lightDir;

uniform float u_Time;

out vec4 out_Col;




const vec3 daysky[5] = vec3[](
    vec3(135, 206, 235) / 255.0,  // sky blue
    vec3(100, 180, 230) / 255.0,
    vec3(80, 160, 225) / 255.0,
    vec3(60, 140, 220) / 255.0,
    vec3(40, 120, 215) / 255.0
);


const vec3 sunset[5] = vec3[](vec3(255, 229, 119) / 255.0,
                               vec3(254, 192, 81) / 255.0,
                               vec3(255, 137, 103) / 255.0,
                               vec3(253, 96, 81) / 255.0,
                               vec3(57, 32, 51) / 255.0);
// Dusk palette
const vec3 dusk[5] = vec3[](vec3(144, 96, 144) / 255.0,
                            vec3(96, 72, 120) / 255.0,
                            vec3(72, 48, 120) / 255.0,
                            vec3(48, 24, 96) / 255.0,
                            vec3(0, 24, 72) / 255.0);


const vec3 sunDir = normalize(vec3(-1, 0.1, 0));

vec3 random3( vec3 p ) {
    return fract(sin(vec3(dot(p,vec3(127.1, 311.7, 191.999)),
                          dot(p,vec3(269.5, 183.3, 765.54)),
                          dot(p, vec3(420.69, 631.2,109.21))))
                 *43758.5453);
}


float WorleyNoise3D(vec3 p)
{
    // Tile the space
    vec3 pointInt = floor(p);
    vec3 pointFract = fract(p);

    float minDist = 1.0; // Minimum distance initialized to max.

    // Search all neighboring cells and this cell for their point
    for(int z = -1; z <= 1; z++)
    {
        for(int y = -1; y <= 1; y++)
        {
            for(int x = -1; x <= 1; x++)
            {
                vec3 neighbor = vec3(float(x), float(y), float(z));

                // Random point inside current neighboring cell
                vec3 point = random3(pointInt + neighbor);

                // Animate the point
                point = 0.5 + 0.5 * sin(u_Time * 0.01 + 6.2831 * point); // 0 to 1 range

                // Compute the distance b/t the point and the fragment
                // Store the min dist thus far
                vec3 diff = neighbor + point - pointFract;
                float dist = length(diff);
                minDist = min(minDist, dist);
            }
        }
    }
    return minDist;
}

float fractalWorley(vec3 rayDir) {
    float amp = 0.5;
    float freq = 1.0;
    float sum = 0;
    for (int i = 0; i < 4; i++) {
        sum += WorleyNoise3D(rayDir * freq) * amp;
        amp *= 0.5;
        freq += 2;
    }

    return sum;
}



vec3 calculateSunDir() {
    float angle = 0.05 * u_Time * (2.0 * 3.14159 / 60.0);

    return normalize(vec3(
        -1.0,
        0.3 * sin(angle),
        0.3 * cos(angle)
    ));
}

struct Ray {
    vec3 origin;
    vec3 direction;
};

Ray raycast() {
    vec3 ref = eye + F;
    vec3 V = U * 1 * tan(radians(22.5));
    vec3 H = R * 1 * tan(radians(22.5)) * aspect;
    float sx = fs_UV.x * 2 - 1;
    float sy = fs_UV.y * 2 - 1;

    vec3 p = ref + sx * H + sy * V;

    Ray result;
    result.origin = eye;
    result.direction = normalize(p - eye);

    return result;
}

vec3 sunsetLerp(float t) {
    t *= 4;
    float fract_t = fract(t);
    int low_t = int (floor(t));
    int high_t = low_t + 1;
    return mix(sunset[low_t], sunset[high_t], fract_t);
}

vec3 duskLerp(float t) {
    t *= 4;
    float fract_t = fract(t);
    int low_t = int (floor(t));
    int high_t = low_t + 1;
    return mix(dusk[low_t], dusk[high_t], fract_t);
}

vec3 skyColor(vec3 rayDir) {
    vec3 currentSun = lightDir.xyz;
    float t = clamp(dot(rayDir, vec3(0,1,0)), 0, 1);
    // float worley = fractalWorley(rayDir * 10);
    // worley = smoothstep(0.0, 1.0, worley) * 0.15;
    // t = clamp(t + worley, 0, 1);

    float t2 = clamp(dot(rayDir, currentSun), 0, 1);

    float sunHeight = currentSun.y;
    float dayInfluence = smoothstep(0.2, 0.4, sunHeight);

    vec3 horizonSky = mix(duskLerp(t), sunsetLerp(t), t2);

    vec3 blueSky = daysky[0];
    vec3 sky = mix(horizonSky, blueSky, dayInfluence);

    if (t2 > 0.999) {
        sky = sunset[0];
    } else if (t2 > 0.993) {
        sky = mix(sky, sunset[0], (t2 - 0.995) / 0.01);
    }

    if (sunHeight < 0.0) {
        sky = mix(sky, dusk[4], smoothstep(0.0, -0.2, sunHeight));
    }

    return sky;
}


// vec3 skyColor(vec3 rayDir) {
//     vec3 currentSun = lightDir.xyz;
//     float t = clamp(dot(rayDir, vec3(0,1,0)), 0, 1);

//     float worley = fractalWorley(rayDir * 8.0);
//     float cloudShape = smoothstep(0.4, 0.8, worley);
//     cloudShape *= 0.25;

//     cloudShape *= smoothstep(0.0, 0.3, rayDir.y);

//     float t2 = clamp(dot(rayDir, currentSun), 0, 1);
//     float sunHeight = currentSun.y;
//     float dayInfluence = smoothstep(0.2, 0.4, sunHeight);

//     vec3 horizonSky = mix(duskLerp(t), sunsetLerp(t), t2);
//     vec3 blueSky = daysky[0];
//     vec3 basesky = mix(horizonSky, blueSky, dayInfluence);

//     vec3 daytimeCloudColor = vec3(1.0, 0.98, 0.95);
//     vec3 sunsetCloudColor = sunset[1];
//     vec3 cloudColor = mix(sunsetCloudColor, daytimeCloudColor, dayInfluence);

//     vec3 sky = mix(basesky, cloudColor, cloudShape);

//     if (t2 > 0.999) {
//         sky = sunset[0];
//     } else if (t2 > 0.993) {
//         sky = mix(sky, sunset[0], (t2 - 0.995) / 0.01);
//     }

//     if (sunHeight < 0.0) {
//         sky = mix(sky, dusk[4], smoothstep(0.0, -0.2, sunHeight));
//     }

//     return sky;
// }

void main(void)
{
    Ray ray = raycast();
    out_Col = vec4(skyColor(ray.direction), 1);
}

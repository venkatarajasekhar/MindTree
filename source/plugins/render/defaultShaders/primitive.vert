#version 330
in vec3 P;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 mvp;
uniform mat4 staticTransformation;
uniform bool fixed_screensize = true;
uniform bool screen_oriented = false;
uniform int point_size = 5;

uniform ivec2 resolution;
out vec3 pos;

mat4 computeScreenOrientation() {
    mat4 camrotation = mat4(mat3(view));
    mat4 invcamrot = inverse(camrotation);
    return invcamrot;
}

mat4 extractTranslation(mat4 mat){
    return mat * inverse(mat4(mat3(mat)));
}

mat4 extractRotation(mat4 mat) {
    mat4 rot = mat4(1);
    rot[0] = mat[0];
    rot[1] = mat[1];
    rot[2] = mat[2];

    return rot;
}

vec2 project(vec4 pos) {
    vec4 posndc = projection * view * extractTranslation(model) * pos;
    posndc /= posndc.w;
    vec2 pospixel = posndc.xy + vec2(1);
    pospixel /= 2;
    pospixel *= resolution;
    return pospixel;
}

mat4 computeScaleMatrix() {
    mat4 invcamrot = computeScreenOrientation();
    vec2 x0 = project(invcamrot * vec4(0, 0, 0, 1));
    vec2 x1 = project(invcamrot * vec4(1, 0, 0, 1));

    float screenlength = length(x1 - x0);

    mat4 scalemat = mat4(1);
    scalemat[0].x = 50 / screenlength;
    scalemat[1].y = 50 / screenlength;
    scalemat[2].z = 50 / screenlength;

    return scalemat;
}

mat4 normalizeMat4(mat4 mat)
{
    mat[0] = normalize(mat[0]);
    mat[1] = normalize(mat[1]);
    mat[2] = normalize(mat[2]);

    return mat;
}

void main(){
    mat4 screenTransform;
    mat4 translation = extractTranslation(model);
    mat4 rot = extractRotation(model);

    vec4 staticTransformedP = staticTransformation * vec4(P, 1);
    if (screen_oriented) {
        screenTransform = projection * view * translation * computeScreenOrientation();
    }
    else
        screenTransform = projection * view * normalizeMat4(model);

    vec4 output;
    if (fixed_screensize)
        output = screenTransform * computeScaleMatrix() * staticTransformedP;
    else 
        output = screenTransform * staticTransformedP;
    gl_Position = output;
    gl_PointSize = point_size;

    pos = (translation * staticTransformedP).xyz;
};

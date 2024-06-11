#ifdef VERTEX_SHADER
precision highp float;

uniform float iTime;
uniform mat4 view;
uniform mat4 proj;

mat4 Ry(float a){
  float ra = radians(a);
  mat4 R = mat4(1);
  R[0][0] =  cos(ra);
  R[2][2] =  cos(ra);
  R[0][2] =  sin(ra);
  R[2][0] = -sin(ra);
  return R;
}

mat4 Rx(float a){
  float ra = radians(a);
  mat4 R = mat4(1);
  R[1][1] =  cos(ra);
  R[2][2] =  cos(ra);
  R[1][2] =  sin(ra);
  R[2][1] = -sin(ra);
  return R;
}

mat4 T(float x,float y,float z){
  mat4 R = mat4(1);
  R[3] = vec4(x,y,z,1);
  return R;
}

out vec2 vCoord;
flat out int vMaterial;

void quad(int offset,mat4 model,int m){
 
  vec3 verts[] = vec3[](
    vec3(0,0,0),
    vec3(1,0,0),
    vec3(0,1,0),
    vec3(0,1,0),
    vec3(1,0,0),
    vec3(1,1,0)
  );
  mat4 pvm = proj * view * model;
 
  int vID = gl_VertexID - offset;
 
  if(vID < 0 || vID >= verts.length())return;
 
  gl_Position = pvm*vec4(verts[vID],1);
  vCoord = verts[vID].xy;
  vMaterial = m;
}

void main(){
  gl_Position = vec4(0,0,0,1);
 
  vMaterial = 0;

  quad(0,T(0,0,0),1);
  quad(6,Ry(90),2);
  quad(12,T(1,0,1)*Ry(180),3);
  quad(18,T(1,0,1)*Ry(270),4);
  quad(24,T(0,0,0)*Rx(90),5);
  quad(30,T(0,1,0)*Rx(90),6);
}
              
#endif

#ifdef FRAGMENT_SHADER
precision highp float;

out vec4 fColor;

in vec2 vCoord;

uniform float iTime;
flat in int vMaterial;

const float pi = 3.14159;

void main(){

  vec2 uv = vCoord * 2 - 1;
  vec3 color = vec3(0,0,0);
  float time = iTime;

  if(vMaterial == 1) {
    uv *= 4;
    float r = fract(length(uv));
    float g = fract(length(uv * 2));
    float b = fract(length(uv * 3));
    color = vec3(r,g,b);
  }
 
  if(vMaterial == 2) {
    uv = abs(uv * 3);
    time = time * 3;

    float p = 4.;
    float a = sqrt(pow(uv.x, p) + pow(uv.y, p));

    float r = sin(a - time);
    float g = sin(a - time + pi / 6);
    float b = sin(a - time + pi / 3);

    color = vec3(r,g,b);
  }
 
  if(vMaterial == 3) {
    uv *= 1.3;
    uv.x -= 0.5;
    vec2 orig = uv;

    for (int i = 0; i < 50; i++) {
      float tmp = uv.x;
      uv.x = uv.x * uv.x - uv.y * uv.y + fract(time / 3);
      uv.y = 2 * tmp * uv.y + fract(time / 3);

      if (length(uv) > 4) {
        color = vec3(float(i) / 50);
        return;
      }
    }

    color = vec3(100);
  }
 
  if(vMaterial == 4) {
    uv *= 10;
    color = vec3(0);

    float sine = sin(uv.x + time) + uv.x / 2;
    bool value = fract(length(uv.y - sine)) < 0.1;
    if (value) 
      color = vec3(1);
  }
 
  if(vMaterial == 5) {
    float random = 252452 * sin(uv.x * 46848) + sin(uv.y * 4587646);

    for (int i = 0; i < 5; i++) {
      random = fract(random);
      random = 252 * sin(uv.x - cos(78*uv.y - 3543*sin(uv.x - 78* cos(uv.y))) * 468 + 45) + sin(uv.y * 426 + 78115);
    }

    color = vec3(random);
  }
 
  if(vMaterial == 6) {
    uv *= 1.3;
    uv.x -= 0.5;
    vec2 orig = uv;

    for (int i = 0; i < 50; i++) {
      float tmp = uv.x;
      uv.x = uv.x * uv.x - uv.y * uv.y + orig.x;
      uv.y = 2 * tmp * uv.y + orig.y;

      if (length(uv) > 4) {
        color = vec3(float(i) / 50);
        return;
      }
    }

    color = vec3(0);
  }
 
  fColor = vec4(color,1);
}
#endif

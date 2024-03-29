#pragma once
#include <math.h>

#define IDK_PI32 3.14159265359f
#define IDK_PI 3.14159265358979323846
#define IDK_F32_MAX 3.402823e+38
#define IDK_F32_MIN 1.175494e-38

union idk_vec2
{
  struct
  {
    float x;
    float y;
  };
};

union idk_vec3
{
  struct
  {
    float x;
    float y;
    float z;
  };
  float elements[3];
#ifdef __cplusplus
  inline float& operator[](const int& index)
  {
    return elements[index];
  }

#endif
};

union idk_vec4
{
  struct
  {
    float x;
    float y;
    float z;
    float w;
  };
  float elements[4];
#ifdef __cplusplus
  inline float& operator[](const int& index)
  {
    return elements[index];
  }

#endif
};

inline bool operator==(const idk_vec2& a, const idk_vec2& b)
{
  return (a.x == b.x && a.y == b.y);
}

inline bool operator==(const idk_vec3& a, const idk_vec3& b)
{
  return (a.x == b.x && a.y == b.y && a.z == b.z);
}

union idk_mat4
{
  float elements[4][4];
};

inline idk_vec3 idk_vec3f(float x, float y, float z)
{
  idk_vec3 result = {};
  result.x = x;
  result.y = y;
  result.z = z;
  return result;
}

inline idk_vec3 idk_vec3fv(float value)
{
  idk_vec3 result = {};
  result.x = value;
  result.y = value;
  result.z = value;
  return result;
}

inline idk_mat4 idk_mat4f(float value)
{
  idk_mat4 result = {};
  result.elements[0][0] = value;
  result.elements[1][1] = value;
  result.elements[2][2] = value;
  result.elements[3][3] = value;
  return result;
}

inline idk_vec3 idk_cross(idk_vec3 a, idk_vec3 b)
{
  idk_vec3 result = {};
  result.x = a.y*b.z - b.y*a.z;
  result.y = a.z*b.x - b.z*a.x;
  result.z = a.x*b.y - b.x*a.y;
  return result;
}

inline float idk_clamp(float value, float lower, float upper)
{
  float result = value;
  if(value < lower)
    result = lower;
  if(value > upper)
    result = upper;
  return result;
}

inline float idk_radians(float degrees)
{
  float result = degrees * (IDK_PI32/180.0f);
  return result;
}

inline float idk_min(float a, float b)
{
  return a < b ?a:b;
}

inline float idk_max(float a, float b)
{
  return a > b ?a:b;
}

inline idk_mat4 idk_perspective(float fovy, float aspect, float z_near, float z_far)
{
  // Reference: https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/gluPerspective.xml
  idk_mat4 result = {};
  float f = 1.0f/tan(fovy/2);
  result.elements[0][0] = f/aspect;
  result.elements[1][1] = f;
  result.elements[2][3] = -1.0f;
  result.elements[2][2] = (z_far+z_near)/(z_near-z_far);
  result.elements[3][2] = (2*z_far*z_near)/(z_near-z_far);
  result.elements[3][3] = 0.0f;
  return result;
}

inline idk_mat4 idk_scale(idk_mat4 m, idk_vec3 scale)
{
  // Reference: https://en.wikipedia.org/wiki/Scaling_(geometry)
  idk_mat4 result = {};
  result.elements[0][0] = m.elements[0][0]*scale[0];
  result.elements[1][1] = m.elements[1][1]*scale[1];
  result.elements[2][2] = m.elements[2][2]*scale[2];
  result.elements[3][3] = m.elements[3][3];
  return result;
}

inline idk_mat4 idk_translate(idk_mat4 m, idk_vec3 scale)
{
  // Reference: https://en.wikipedia.org/wiki/Scaling_(geometry)
  idk_mat4 result = idk_mat4f(1.0f);
  result.elements[0][3] = scale[0];
  result.elements[1][3] = scale[1];
  result.elements[2][3] = scale[2];
  return result;
}

#ifdef __cplusplus
inline idk_vec3 operator*(float a, idk_vec3 b)
{
  idk_vec3 result = {};
  result.x = b.x * a;
  result.y = b.y * a;
  result.z = b.z * a;
  return result;
}

inline idk_vec3 operator*(idk_vec3 a, float b)
{
  idk_vec3 result = {};
  result.x = a.x * b;
  result.y = a.y * b;
  result.z = a.z * b;
  return result;
}

inline idk_vec3 operator-(idk_vec3 a)
{
  idk_vec3 result = {};
  result.x = -a.x;
  result.y = -a.y;
  result.z = -a.z;
  return result;
}

inline idk_vec3 operator-(idk_vec3 a, idk_vec3 b)
{
  idk_vec3 result = {};
  result.x = a.x - b.x;
  result.y = a.y - b.y;
  result.z = a.z - b.z;
  return result;
}

inline idk_vec3& operator-=(idk_vec3& a, idk_vec3 b) {
  return (a = a - b);
}

inline idk_vec3 operator+(idk_vec3 a, idk_vec3 b)
{
  idk_vec3 result = {};
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  result.z = a.z + b.z;
  return result;
}

inline idk_vec3& operator+=(idk_vec3& a, idk_vec3 b) {
  return (a = a + b);
}
#endif

inline float idk_dot_vec3(idk_vec3 a, idk_vec3 b)
{
  float result = (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
  return result;
}

inline float idk_vec3_length(idk_vec3 a)
{
  float result = sqrtf(idk_dot_vec3(a,a));
  return result;
}

inline idk_vec3 idk_normalize_vec3(idk_vec3 a)
{
  idk_vec3 result = {};
  float vector_length = idk_vec3_length(a);
  result = (1.0f/vector_length) * a;
  return result;
}

inline idk_mat4 idk_lookat(idk_vec3 eye, idk_vec3 center, idk_vec3 up)
{
  idk_mat4 result;

  idk_vec3 f = idk_normalize_vec3(center-eye);
  idk_vec3 cross = idk_cross(f, up);
  idk_vec3 s = idk_normalize_vec3(cross);
  idk_vec3 u = idk_cross(s, f);

  result.elements[0][0] = s.x;
  result.elements[0][1] = u.x;
  result.elements[0][2] = -f.x;
  result.elements[0][3] = 0.0f;

  result.elements[1][0] = s.y;
  result.elements[1][1] = u.y;
  result.elements[1][2] = -f.y;
  result.elements[1][3] = 0.0f;

  result.elements[2][0] = s.z;
  result.elements[2][1] = u.z;
  result.elements[2][2] = -f.z;
  result.elements[2][3] = 0.0f;

  result.elements[3][0] = -idk_dot_vec3(s, eye);
  result.elements[3][1] = -idk_dot_vec3(u, eye);
  result.elements[3][2] = idk_dot_vec3(f, eye);
  result.elements[3][3] = 1.0f;

  return result;
}

#ifndef __GLM_H__
#define __GLM_H__

#pragma warning (push, 4)
#pragma warning (disable:4201)
#pragma warning (disable:4127)
#define GLM_FORCE_SWIZZLE
#include <glm.hpp> // vec3, vec4, ivec4, mat4
#include <gtc/matrix_transform.hpp> // translate, rotate, scale, perspective
#include <gtc/type_ptr.hpp>
#pragma warning (pop)

#endif // ! __GLM_H__
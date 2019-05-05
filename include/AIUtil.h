//----------------------------------------------------------------------------------------------------------------------
/// @file AIUtil.h
/// @brief Util functions that help converting between Assimp
/// and NGL
/// @author Jonathan Macey
/// @version 1.0
/// @date 10/9/13
//----------------------------------------------------------------------------------------------------------------------

#ifndef AIUTIL_H__
#define AIUTIL_H__

#include <assimp/scene.h>
#include <assimp/cimport.h>

namespace ngl
{
  class Mat4;
  class Vec2;
  class Vec3;
  class Vec4;
  class Quaternion;
}

namespace AIU
{

//-----------------------------------------------
/// @brief converts between Assimp native Matrix format
///and NGL::matrix format
///@param[in] _m Assimp Matrix
///@param[out] ngl::Mat4
//---------------------------------------------------
extern ngl::Mat4 aiMatrix4x4ToNGLMat4(const aiMatrix4x4 &_m);
//-----------------------------------------------
/// @brief converts between Assimp native Matrix format
///and NGL::matrix format after transposing
///@param[in] _m Assimp Matrix
///@param[out] ngl::Mat4
//---------------------------------------------------
extern ngl::Mat4 aiMatrix4x4ToNGLMat4Transpose(const aiMatrix4x4 &_m);
//-----------------------------------------------
/// @brief converts between Assimp native vector3 format
///and NGL::vec3 format
///@param[in] _m Assimp vector3d
///@param[out] ngl::Vec3
//---------------------------------------------------
extern ngl::Vec3 aiVector3DToNGLVec3(const aiVector3D &_v);
//-----------------------------------------------
/// @brief converts between Assimp native vector3 format
///and NGL::vec4 format after setting vec4.w=1.0
///@param[in] _m Assimp vector3d
///@param[out] ngl::Vec4
//---------------------------------------------------
extern ngl::Vec4 aiVector3DToNGLVec4(const aiVector3D &_v);
//-----------------------------------------------
/// @brief converts between Assimp native vector2 format
///and NGL::vec2 format
///@param[in] _m Assimp vector2d
///@param[out] ngl::Vec2
//---------------------------------------------------
extern ngl::Vec2 aiVector2DToNGLVec2(const aiVector2D &_v);
//-----------------------------------------------
/// @brief converts between Assimp native quaternion format
///and NGL::Quaternion format
///@param[in] _m Assimp Quaternion
///@param[out] ngl::Quaternion
//---------------------------------------------------
extern ngl::Quaternion aiQuatToNGLQuat(const aiQuaternion &_v);

//-----------------------------------------------
/// @brief calculate the bounding box of the scene after looping
/// through all the meshes
///@param[in] scene Assimp scene data
///@param[in] o_min lower bounding box Values
///@param[in] o_max upper bounding box Values
//---------------------------------------------------
extern void getSceneBoundingBox(const aiScene * scene,ngl::Vec3 &o_min, ngl::Vec3 &o_max);

}


#endif

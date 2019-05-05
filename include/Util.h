//----------------------------------------------------------------------------------------------------------------------
/// @file Util,h
/// @brief Util functions that help converting and displaying
/// vectors,matrices and quaternions
/// @author Prethish Bhasuran
/// @version 1.0
/// @date 12/9/14
//----------------------------------------------------------------------------------------------------------------------
#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include<string>

#include<ngl/Vec3.h>
#include<ngl/Vec4.h>
#include<ngl/Mat3.h>
#include<ngl/Mat4.h>
#include<ngl/Util.h>
#include<ngl/Quaternion.h>
#include <ngl/Transformation.h>

#include"Dualquaternion.h"

//-----------------------------------------------
/// @brief function that transforms a point using Quaternion
///quaternion tranformation using cross product instead q*v*q1
///v1 = v + 2r × (r × v + av) where v is original point,quaternion is <a,r>
///@param[in] _q Rotation Quaternion
///@param[in] _v ngl::vec3 point
///@param[out] ngl::Vec3 transformed point
//---------------------------------------------------
ngl::Vec3 Quat_transfomPoint(ngl::Quaternion _q,ngl::Vec3 _v)
{
    //it works properly if its a unit quaternion
    _q.normalise();
    ngl::Real a=_q.getS();
    ngl::Vec3 r(_q.getX(),_q.getY(),_q.getZ());
    ngl::Vec3 r2=2*r;
    return (_v +r2.cross(r.cross(_v)+a*_v)) ;
}

//-----------------------------------------------
/// @brief used for calculation simplicity
/// its just a multiplication of similar variables
/// dot=x*x+y*y+z*z+w*w
///@param[in] _p Quaternion 1
///@param[in] _q Qauternion 2
///@param[out] ngl::Real
//---------------------------------------------------
ngl::Real Quat_dot(ngl::Quaternion _p, ngl::Quaternion _q)
{
  return (_p.getX() * _q.getX() +
          _p.getY() * _q.getY() +
          _p.getZ() * _q.getZ() +
          _p.getS() * _q.getS());
}

//-----------------------------------------------
/// @brief display NGL::vec3
///@param[in] _v ngl::Vec3
///@param[in] _s string to print in front
//---------------------------------------------------
void displayVec( ngl::Vec3 _v,std::string _s="V3")
{
  std::cout <<_s<< "["
           << _v.m_x << ","
            << _v.m_y << ","
            << _v.m_z << "]" << std::endl;
}
//-----------------------------------------------
/// @brief display NGL::Quaternion
///@param[in] _q ngl::Quaternion
///@param[in] _s string to print in front
//---------------------------------------------------
void displayQuat(ngl::Quaternion _q,std::string _s="Q4")
{
  std::cout << _s<<"[s="
            << _q.getS() << ",x="
            << _q.getX()<< ",y="
            << _q.getY()<< ",z="
            << _q.getZ() << "]" << std::endl;

}

//-----------------------------------------------
/// @brief display NGL::mat3
///@param[in] _m ngl::mat3
///@param[in] _s string to print in front
//---------------------------------------------------
void displayMat(ngl::Mat3 _m,std::string _s="Mat3")
{
  std::cout << _s<< "[\n"
            << _m.m_00 << ","
            << _m.m_01 << ","
            << _m.m_02 << std::endl
            << _m.m_10 << ","
            << _m.m_11 << ","
            << _m.m_12 << std::endl
            << _m.m_20 << ","
            << _m.m_21 << ","
            << _m.m_22 << "]" << std::endl;
}
//-----------------------------------------------
/// @brief display NGL::mat4
///@param[in] _m ngl::mat4
///@param[in] _s string to print in front
//---------------------------------------------------
void displayMat(ngl::Mat4 _m,std::string _s="Mat4")
{
  std::cout << _s<<"[\n" << _m.m_00 << ","
            << _m.m_01 << ","
            << _m.m_02 << ","
            << _m.m_03 << std::endl
            << _m.m_10 << ","
            << _m.m_11 << ","
            << _m.m_12 << ","
            << _m.m_13 << std::endl
            << _m.m_20 << ","
            << _m.m_21 << ","
            << _m.m_22<< ","
            << _m.m_23 << std::endl
            << _m.m_30 << ","
            << _m.m_31 << ","
            << _m.m_32 << ","
            << _m.m_33 << "]" << std::endl;
}

//-----------------------------------------------
/// @brief display DualQuaternion
///@param[in] _v DualQuaternion
///@param[in] _s string to print in front
//---------------------------------------------------
void displayDualQuaternion(DualQuaternion _d,std::string _s="DQ--")
{
    std::cout<<_s;
    displayQuat(_d.getReal(),"Real");
    displayQuat(_d.getDual(),"Dual");
}

//-----------------------------------------------
/// @brief return the rotation matrix3x3 and translate
/// works only for rigid transforms
///@param[in] _m rigid affine matrix
///@param[in] _r rotation matrix
///@param[in] _t translate
//---------------------------------------------------
void getTranslateRotate(ngl::Mat4 _m,ngl::Mat3 &_r,ngl::Vec3 &_t)
{
    _r.m_00=_m.m_00;
    _r.m_01=_m.m_01;
    _r.m_02=_m.m_02;
    _r.m_10=_m.m_10;
    _r.m_11=_m.m_11;
    _r.m_12=_m.m_12;
    _r.m_20=_m.m_20;
    _r.m_21=_m.m_21;
    _r.m_22=_m.m_22;
    _t=ngl::Vec3(_m.m_30,_m.m_31,_m.m_32);
}

//-----------------------------------------------
/// @brief return the rotation matrix3x3 and translate
/// works only for rigid transforms
///@param[in] _m rigid affine matrix
///@param[in] _r rotation quaternion
///@param[in] _t translate
//---------------------------------------------------
void getTranslateRotate(const ngl::Mat4 &_m, ngl::Quaternion &_r, ngl::Vec3 &_t)
{
    _r=ngl::Quaternion (_m);
       _t=ngl::Vec3(_m.m_30,_m.m_31,_m.m_32);
}

//-----------------------------------------------
/// @brief multiply a vec3 point with a 4x4 matrix and return the transformed
/// point
///@param[in] _v input point
///@param[in] _m transform matrix
///@param[in] _w weight of transformatoin
///@param[out] ngl::Vec3 transformed point
//---------------------------------------------------
ngl::Vec3 multMatrix(ngl::Vec3 _v,ngl::Mat4 _m,ngl::Real _w=1.0f)
{
      ngl::Vec4 temp= ngl::Vec4(_v) * _m*_w;
      temp=temp.m_w>1?temp/temp.m_w:temp;
      return(ngl::Vec3(temp.m_x,temp.m_y,temp.m_z));
}

//-----------------------------------------------
/// @brief function to decompose rotation matrix is
/// yaw,pitch and roll euler angles in degrees
/// this function is not working well as the angle Y seems to be wrong
/// on occasion
///@param[in] _m mat4 rotation matrix
///@param[out] ngl::Vec3 Euler rotation in Degrees
//---------------------------------------------------
ngl::Vec3 decomposeRotation(ngl::Mat4 _m)
{
        ngl::Real rx=ngl::degrees( atan2(_m.m_12,_m.m_22) );
        ngl::Real ry=ngl::degrees( atan2(-_m.m_02, sqrt(_m.m_00*_m.m_00+_m.m_01*_m.m_01)) );
        ngl::Real rz=ngl::degrees(atan2(_m.m_01,_m.m_00));
       return ngl::Vec3(rx,ry,rz);
}

//-----------------------------------------------
/// @brief function to decompose rotation matrix is
/// yaw,pitch and roll euler angles in degrees
/// this function is not working well as the angle Y seems to be wrong
/// on occasion
///@param[in] _m mat3 rotation matrix
///@param[out] ngl::Vec3 Euler rotation in Degrees
//---------------------------------------------------
ngl::Vec3 decomposeRotation(ngl::Mat3 _m)
{
        ngl::Real rx=ngl::degrees( atan2(_m.m_12,_m.m_22) );
        ngl::Real ry=ngl::degrees( atan2(-_m.m_02, sqrt(_m.m_00*_m.m_00+_m.m_01*_m.m_01)) );
        ngl::Real rz=ngl::degrees(atan2(_m.m_01,_m.m_00));
       return ngl::Vec3(rx,ry,rz);
}

#endif // UTIL_H

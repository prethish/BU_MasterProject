//----------------------------------------------------------------------------------------------------------------------
/// @file DualQuaternion.cpp
/// @brief member fucntions of class DualQuaternion
/// @author Prethish Bhasuran
/// @version 1.0
/// @date 12/9/14
//----------------------------------------------------------------------------------------------------------------------

#include "Dualquaternion.h"
#include<math.h>
#include<iostream>


DualQuaternion::DualQuaternion(const ngl::Mat4 _m)
{
  m_real = ngl::Quaternion(_m);
  ngl::Quaternion v(0, _m.m_30, _m.m_31, _m.m_32);
  m_dual = v * m_real * 0.5f;
  normalize();
}

DualQuaternion DualQuaternion::operator+(const DualQuaternion _dq) const
{
  DualQuaternion t;
  t.m_real = m_real + _dq.m_real;
  t.m_dual = m_dual + _dq.m_dual;
  return t;
}
void DualQuaternion::operator+=(const DualQuaternion _dq)
{
  *this = *this + _dq;
}

DualQuaternion DualQuaternion::operator-(const DualQuaternion _dq) const
{
  DualQuaternion t;
  t.m_real = m_real - _dq.m_real;
  t.m_dual = m_dual - _dq.m_dual;
  return t;
}
void DualQuaternion::operator-=(const DualQuaternion _dq)
{
  *this = *this - _dq;
}

DualQuaternion DualQuaternion::operator*(const DualQuaternion _dq) const
{
  DualQuaternion t;
  t.m_real = m_real * _dq.m_real;
  t.m_dual = m_real * _dq.m_dual + m_dual * _dq.m_real;
  return t;
}


void DualQuaternion::operator*=(const DualQuaternion _dq)
{
  *this = *this * _dq;
}

DualQuaternion DualQuaternion::operator *(ngl::Real _r)
{
  DualQuaternion t(m_real * _r, m_dual * _r);
  return t;
}
void DualQuaternion::operator *=(ngl::Real _r)
{
  m_real *= _r;
  m_dual *= _r;
}
inline void DualQuaternion::operator -()
{
  -m_real;
  -m_dual;
}
inline DualQuaternion DualQuaternion::operator -()const
{
  return (-*this);
}
DualQuaternion DualQuaternion::conjugate()
{
  ngl::Quaternion real = m_real.conjugate();
  ngl::Quaternion dual = m_dual.conjugate() * -1;
  return DualQuaternion(real, dual);
}

ngl::Real DualQuaternion::quaternionDot(ngl::Quaternion _p, ngl::Quaternion _q)
{
  return (_p.getX() * _q.getX() +
          _p.getY() * _q.getY() +
          _p.getZ() * _q.getZ() +
          _p.getS() * _q.getS());
}

void DualQuaternion::normalize()
{
  ngl::Real mag = sqrt(quaternionDot(m_real, m_real));
  if (mag > 0) {
    ngl::Quaternion dual = m_dual * (1.0 / mag);
    m_real *= 1.0 / mag;
    m_dual = dual - m_real * quaternionDot(m_real, dual);
  }

}

bool DualQuaternion::isNormalized()
{
  ngl::Quaternion real = m_real;
  ngl::Quaternion dual = m_dual;
  if ((real.conjugate()*dual + dual.conjugate()*real) == 0) {
    return true;
  } else {
    return false;
  }
}

ngl::Real DualQuaternion::magnitude()
{
  return sqrt(quaternionDot(m_real, m_real));
}

ngl::Mat4 DualQuaternion::toMatrix() const
{
  ngl::Mat4 m;
  m.identity();
  m = m_real.toMat4();
  // Extract translation information
  ngl::Vec3 t = this->getTranslation();
  m.m_30 = t.m_x;
  m.m_31 = t.m_y;
  m.m_32 = t.m_z;
  return m;
}

void DualQuaternion::fromMatrix(ngl::Mat4 _m)
{
  m_real = ngl::Quaternion(_m);
  ngl::Quaternion v(0, _m.m_30, _m.m_31, _m.m_32);
  m_dual = v * m_real * 0.5f;
  normalize();
}

ngl::Vec3 DualQuaternion::transformPoint(ngl::Vec3 _p)
{
  DualQuaternion p;
  DualQuaternion n(m_real, m_dual);
  n.normalize();
  p.setPoint(_p);
  DualQuaternion r = n * p * n.conjugate();
  _p = r.getTranslation() * .5;
  return _p;
}



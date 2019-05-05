//----------------------------------------------------------------------------------------------------------------------
/// @file DualQuaternion.h
/// @brief the main DualQuaternion class the creates the Datatype DualQuaternion
/// @author Prethish Bhasuran
/// @version 1.0
/// @date 12/9/14
/// @class DualQuaternion
///@brief it uses NGL::Quaternion class to repersent the real and dual parts of a DualQuaternion
///a Dual quaternion is made of 2 quaternions,it uses Dual number arthematic
///a DualQuaternion q=qr+E.qd- where E is dual coeficiant with property E*E=0,
///-where(m_real) qr=real quaternion part which is usually used to represent rotation
///default value is [0,0,0,1] since the nql::Quaternion representation is [x,y,z,w]
///-where(m_dual) qd=dual quaternion part which is usually used to represent translation  or a point
///default value is [0,0,0,0] since the nql::Quaternion representation is [x,y,z.w]
/// since it uses quaternions many of its arthematic properties are similar to Quaternions
/// Note:1.DualQuaterions works only on rigid transforms,so using a non rigid transform affine matrix( with scalling)
///- will give the wrong result
/// 2.DualQuaternions are not Commutative ie,R*T is NOT= T*R
/// 3.A DualQuaternion represents Translation when qr=[1,0,0,0] and qd=[0,tx/2,ty/2,tz/2]
/// 4.A DualQuaternion represents Rotation when qr=rotation Quaternion and qd=[0,0,0,0]
/// 5.A DualQuaternion represents a rigid transform when
/// -case1. Rotation then Translation
/// --DualQuaternion=Rotation DualQuaternion*Translation DualQuaternion
/// --=RotationQuaternion+E*1/2*RotationQuaternion*TranslationQuaternion
/// -case2.Translation then Rotation-this is not used here
/// --Please Refer http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/other/dualQuaternion/MJBDualQuaternions.pdf
/// --for more detailed information
/// -this is the reason why setTranslation fucntion sets dual part(m_dual)=1/2*real*dual
/// -and getTranslation returns translation by translation=2*dual*realConjugate
//----------------------------------------------------------------------------------------------------------------------
#ifndef DUALQUATERNION_H
#define DUALQUATERNION_H

#include<ngl/Quaternion.h>
\
class DualQuaternion
{
public:

  //-----------------------------------------------
  /// @brief Default constructor which sets the default values
  //---------------------------------------------------
  inline DualQuaternion() {
    m_real = ngl::Quaternion(1, 0, 0, 0);
    m_dual = ngl::Quaternion();
  }

  //-----------------------------------------------
  /// @brief constructor to create a dual Quaternion from a NGL::Mat4 matrix
  /// it uses the NGL::Quaternion class to extract the rotation
  /// while the translation is just a copy of the matrix number at 30,31,32 of the matrix
  ///@param[in] _m ngl::mat4 rigid affine transform matrix
  //---------------------------------------------------
  DualQuaternion(const ngl::Mat4 _m);

  //-----------------------------------------------
  /// @brief constructor for DualQuaternion from real and dual qauternion parts
  ///@param[in] _real ngl::Quaternion Real part
  ///@param[in] _dual ngl::Quaternion Dual part
  //---------------------------------------------------
  inline DualQuaternion(ngl::Quaternion _real, ngl::Quaternion _dual)
  {
      //most papers recommend that the rotation be a unit Quaternion when setting it
      // but I have found that normalizing it over here will cause rounding off errors and will cause severe
      //artifacts
  m_real=_real;
  m_dual=_dual;
}

  //-----------------------------------------------
  /// @brief copy constructor for DualQuaternion from another DualQuaternion
  ///@param[in] _dq ngl::Quaternion to copy from
  //---------------------------------------------------
  inline DualQuaternion(const DualQuaternion &_dq):m_real(_dq.m_real), m_dual(_dq.m_dual) {;}


  //-----------------------------------------------
  /// @brief destructor
  ///there are no pointer variables used,so left empty
  //---------------------------------------------------
  ~DualQuaternion() {}

  //-----------------------------------------------
  /// @brief function to set a DualQuaternion to default value
  ///Note:using this for skinning operation as the default value of the Total transformation-
  /// -for TotalDualQuaternion=weight1*DualQuaternion1 + weight2*DualQuaternion2
  /// -will cause ERROR, the final transformations will be multiplied by half its value
  /// -using setNull is recomended instead.
  /// However it is used when DualQuaternion are concantanated
  /// for TotalDualQuaternion=DualQuaternion1 * DualQuaternion2
  //---------------------------------------------------
  void setIdentity()
  {
      m_real.set(1,0,0,0);
      m_dual.set(0,0,0,0);
  }

  //-----------------------------------------------
  /// @brief function to set a DualQuaternion to complete zeros,it is used when
  /// quaternions needs to added together for skinning equations
  /// -for TotalDualQuaternion=weight1*DualQuaternion1 + weight2*DualQuaternion2
  //---------------------------------------------------
  void setNull()
  {
      m_real.set(0,0,0,0);
      m_dual.set(0,0,0,0);
  }

  //-----------------------------------------------
  /// @brief mutator to set the real part
  ///@param[in] _r ngl::Quaternion
  //---------------------------------------------------
  inline void setReal(ngl::Quaternion _r) {
    m_real = _r;
  }

  //-----------------------------------------------
  /// @brief mutator to set the dual part
  ///@param[in] _d ngl::Quaternion
  //---------------------------------------------------
  inline void setDual(ngl::Quaternion _d) {
    m_dual = _d;
  }

  //-----------------------------------------------
  /// @brief mutator to set the Dual Quaternion to represent a point
  /// the real part will be [1,0,0,0]
  /// while the dual part will be [0,tx,ty,tz]
  /// it is used for calculations to transform a point
  ///@param[in] (_point ngl::vec3 point(x,y,z)
  //---------------------------------------------------
  void setPoint(ngl::Vec3 _point)
  {
      m_dual.set(0.0,_point.m_x,_point.m_y,_point.m_z);
  }

  //-----------------------------------------------
  /// @brief set the DualQuaternion to represent a rigid transformation
  /// here it is important that both of them are set at the same time
  /// because the translation part depends on the value of the rotation
  /// more is explained in class notes above
  ///@param[in] _r ngl::Quaternion that represents Rotations
  ///@param[in] _t ngl::Quaternion that represents displacements
  //---------------------------------------------------
  void setRotationTranslate(ngl::Quaternion _r,ngl::Quaternion _t )
  {
      setRotation(_r);
      setTranslation(_t);
  }

  //-----------------------------------------------
  /// @brief set the DualQuaternion to represent a rigid transformation
  ///@param[in] _r ngl::Quaternion that represents Rotations
  ///@param[in] _t ngl::vec3 that represents displacements
  //---------------------------------------------------
  void setRotationTranslate(ngl::Quaternion _r,ngl::Vec3 _t )
  {
      setRotation(_r);
      setTranslation(_t);
  }

  //-----------------------------------------------
  /// @brief set the DualQuaternion to represent a rigid transformation
  ///@param[in] _r ngl::vec3 that represents euler Rotation in Degrees
  ///@param[in] _t ngl::vec3 that represents displacements
  //---------------------------------------------------
  void setRotationTranslate(ngl::Vec3 _rEuler,ngl::Vec3 _t )
  {
      setRotationEuler(_rEuler);
      setTranslation(_t);
  }

  //-----------------------------------------------
  /// @brief accessor to get the dual part
  ///@param[out] ngl::Quaternion
  //---------------------------------------------------
  inline ngl::Quaternion getDual() const {
    return m_dual;
  }

  //-----------------------------------------------
  /// @brief accessor to get the dual part
  ///@param[out] ngl::Quaternion
  //---------------------------------------------------
  inline ngl::Quaternion getReal() const {
    return m_real;
  }

  //-----------------------------------------------
  /// @brief get quaternion rotation
  ///@param[out] ngl::Quaternion
  //---------------------------------------------------
  inline ngl::Quaternion getRotation() const {
    return m_real;
  }

  //-----------------------------------------------
  /// @brief get translation from a rigid transformation
  /// DualQuaternions are not commutative,ie R*T is not equal to T*R
  /// here the equation for getting the translation=2*dual*realConjugate
  /// more in class notes above
  ///@param[out] ngl::Vec3
  //---------------------------------------------------
  ngl::Vec3 getTranslation() const {
    ngl::Quaternion t = (m_dual * 2) * m_real.conjugate();
    return ngl::Vec3(t.getX(), t.getY(), t.getZ());
  }

  //-----------------------------------------------
  /// @brief overloaded operator + to get the result of addition
  /// if q1=qr1+Eqd1 and q2=qr1+Eqd1
  /// then q1+q2=(qr1+qr2)+E(qd1+qd2)
  ///@param[in] _dq DualQuaternion to add
  ///@param[out] DualQuaternion Addition result
  //---------------------------------------------------
  DualQuaternion operator+(const DualQuaternion _dq) const;

  //-----------------------------------------------
  /// @brief overloaded += operator to add a DualQuaternion to itself
  ///@param[in] _dq DualQuaternion to add
  //---------------------------------------------------
  void operator+=(const DualQuaternion _dq);

  //-----------------------------------------------
  /// @brief overloaded operator - to get the result of subtraction
  /// if q1=qr1+Eqd1 and q2=qr1+Eqd1
  /// then q1-q2=(qr1-qr2)+E(qd1-qd2)
  ///@param[in] _dq DualQuaternion to subtract
  ///@param[out] DualQuaternion Subtraction result
  //---------------------------------------------------
  DualQuaternion operator-(const DualQuaternion _dq) const;

  //-----------------------------------------------
  /// @brief overloaded -= operator to subtract a DualQuaternion to itself
  ///@param[in] _dq DualQuaternion to subtract
  //---------------------------------------------------
  void operator-=(const DualQuaternion _dq);

  //-----------------------------------------------
  /// @brief overloaded operator * to get the result of multiplication
  /// if q1=qr1+Eqd1 and q2=qr1+Eqd1
  /// then q1*q2=(qr1*qr2)+E(qr1 * qd2 + qd1 * qr2)
  ///@param[in] _dq DualQuaternion to add
  ///@param[out] DualQuaternion Addition result
  //---------------------------------------------------
  DualQuaternion operator*(const DualQuaternion _dq) const;

  //-----------------------------------------------
   /// @brief overloaded *= operator to multiply a DualQuaternion to itself
   ///@param[in] _dq DualQuaternion to multiply
   //---------------------------------------------------
  void operator*=(const DualQuaternion _dq);

  //-----------------------------------------------
  /// @brief overloaded operator * to get the result of multiplication with a scalar
  /// if q1=qr1+Eqd1 and S is the scalar number
  /// then q1*S=(qr1*S)+E(qd2*S)
  ///@param[in] _r Scalar Number to multiply
  ///@param[out] Scalar Multiplied result
  //---------------------------------------------------
  DualQuaternion operator*(ngl::Real _r) ;

  //-----------------------------------------------
   /// @brief overloaded *= operator to multiply a scalar to itself
   ///@param[in] _r Scalar Number to multiply
   //---------------------------------------------------
  void operator*=(ngl::Real _r);

  //-----------------------------------------------
  /// @brief overlaoded operator- to multiply by -1 to itslef
  //---------------------------------------------------
  inline void operator -();

  //-----------------------------------------------
  /// @briefoverlaoded operator- to multiply by -1
  ///@param[out] DualQuaternion
  //---------------------------------------------------
  inline DualQuaternion operator -()const;

  //-----------------------------------------------
  /// @brief get the conjugate of a dual quaternion
  /// there are multitple definitions of conjugate
  ///1.qrConjugate+E.qdConjugate
  /// 2.qr-E.qd
  /// 3.qrConjugate-E.qdConjugate
  /// here the 3rd definition is implemented since it is the one used for transforming a point
  /// Refer http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/other/dualQuaternion/functions/index.htm
  ///@param[out] DualQuaternion
  //---------------------------------------------------
  DualQuaternion conjugate();

  //-----------------------------------------------
  /// @brief converts to a UNIT DualQuaternion
  //---------------------------------------------------
  void normalize();

  //-----------------------------------------------
  /// @brief checks if a DualQuaternion is UNIT or not
  /// for any UNIT DualQuaternion: qrConjugate*qd+qdConjugate()*qr==0
  ///@param[out] bool
  //---------------------------------------------------
  bool isNormalized();
  //-----------------------------------------------
  /// @brief calculates the magnitude
  /// it is sqrt(qr.x*qr.x+qr.y*qr.y+qr.z*qr.z+qr.w*qr.w)
  ///@param[out] ngl::Real
  //---------------------------------------------------
  ngl::Real magnitude();

  //-----------------------------------------------
  /// @brief converts a DualQuaternion to affine Matrix4x4
  ///the rotation matrix is derived using equation
  /// while the translation is just using getTranslation to get the translation
  /// and set matrix numbers m_30,m_31,m_32
  ///@param[out] ngl::Mat4
  //---------------------------------------------------
  ngl::Mat4 toMatrix() const ;

  //-----------------------------------------------
  /// @brief function to set the dual Quaternion from a NGL::Mat4 matrix
  /// it uses the NGL::Quaternion class to extract the rotation
  /// while the translation is just a copy of the matrix number at 30,31,32 of the matrix
  ///@param[in] _m ngl::mat4 rigid affine transform matrix
  //---------------------------------------------------
  void fromMatrix(ngl::Mat4 _m);

  //-----------------------------------------------
  /// @brief transform a point using the current DualQuaternion to transform
  /// it uses the sandwidtch product to calcualte it
  /// ie transformedPoint=DualQuaternion*point*DualQuaternionConjugate
  ///@param[in] _p ngl::Vec3 point to transform
  ///@param[out] ngl::Vec3
  //---------------------------------------------------
  ngl::Vec3 transformPoint(ngl::Vec3 _p);

  //-----------------------------------------------
  /// @brief used for calculation simplicity
  /// for any quaternion q
  /// dot=q.x*q.x+q.y*q.y+q.z*q.z+q.w*q.w
  ///@param[in] _p ngl::Quaternion
  ///@param[in] _q ngl::Quaternion
  ///@param[out] ngl::Real result
  //---------------------------------------------------
  ngl::Real quaternionDot(ngl::Quaternion _p, ngl::Quaternion _q);
protected:

  //-----------------------------------------------
  /// @brief set the rotation for rigid transformation
  ///@param[in] _r ngl::Quaternion rotation
  //---------------------------------------------------
  inline void setRotation(ngl::Quaternion _r) {
    m_real = _r;
  }

  //-----------------------------------------------
  /// @brief set the rotation for rigid transformation
  ///@param[in] _r Euler rotation in degrees
  //---------------------------------------------------
  inline void setRotationEuler(ngl::Vec3 _r) {
    m_real.fromEulerAngles(_r.m_x, _r.m_y, _r.m_z);
  }

  //-----------------------------------------------
  /// @brief set the translation of a rigid transform
  ///refer class notes above for equation
  ///@param[in] _t ngl::Quaternion
  //---------------------------------------------------
  void setTranslation(ngl::Quaternion _t) {
    m_dual = _t * m_real * 0.5f;
  }

  //-----------------------------------------------
  /// @brief set the translation of a rigid transform
  ///refer class notes above for equation
  ///@param[in] _t ngl::vec3
  //---------------------------------------------------
  void setTranslation(ngl::Vec3 _t) {
    setTranslation(ngl::Quaternion(0, _t.m_x, _t.m_y, _t.m_z));
  }
  //-----------------------------------------------
  /// @brief ngl::Quaternion variable that represent the real part
  ///refer class notes above for more information
  //---------------------------------------------------
  ngl::Quaternion m_real;
  //-----------------------------------------------
  /// @brief ngl::Quaternion variable that represent the dual part
  ///refer class notes above for more information
  //---------------------------------------------------
  ngl::Quaternion m_dual;

};

#endif // DUALQUATERNION_H

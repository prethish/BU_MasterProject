//----------------------------------------------------------------------------------------------------------------------
/// @file SkinDeformer.cpp
/// @brief member fucntions of class SkinDeformer
/// @author Prethish Bhasuran
/// @version 1.0
/// @date 12/9/14
//----------------------------------------------------------------------------------------------------------------------
#include "SkinDeformer.h"
#include "Dualquaternion.h"
#include"Util.h"

SkinDeformer::SkinDeformer()
{
  m_deformMeshVAO = 0;
  m_skinAlgorithm = LINEAR_BLEND;
}

SkinDeformer::~SkinDeformer()
{
  m_deformMesh.clear();
  m_deformMeshVAO->removeVOA();
  delete m_deformMeshVAO;
}

void SkinDeformer::setMeshData(SceneLoader *_scene)
{
    //set the scene for the deformer to access data
  m_scene = _scene;
  if (m_meshSet)
    m_deformMesh.clear();
  m_deformMesh = m_scene->m_vertData;
  m_origMesh = m_scene->m_vertData;
  m_nVerts = m_scene->m_vertData.size();
  m_meshSet = true;
  setDeformMeshVAO();
}

void SkinDeformer::setSkinAlgorithm(int _i)
{
  std::cout << "Setting" << _i << std::endl;
  switch (_i) {
  case 0: {
    m_skinAlgorithm = LINEAR_BLEND;
    break;
  }
  case 1: {
    m_skinAlgorithm = DUAL_QUATERNION;
    break;
  }
  case 2: {
    m_skinAlgorithm = STRETCH_TWIST;
    break;
  }
  }
}

void SkinDeformer::setDeformMeshVAO()
{
  if (m_deformMeshVAO != 0) {
    m_deformMeshVAO->unbind();
    m_deformMeshVAO->removeVOA();
    delete m_deformMeshVAO;
  }
  // attribute vec3 inVert; attribute 0
  // attribute vec2 inUV; attribute 1
  // attribute vec3 inNormal; attribure 2
  // u,v,nx,ny,nz,x,y,z

  std::vector<ngl::Face> faces = m_scene->getFaceList();
  int nFaces = m_scene->getNumFaces();
  std::vector<vertData> drawVerts;
  for (int i = 0; i < nFaces; ++i) {
    ngl::Face f = faces[i];
    for (int j = 0; j < 3; ++j) {
      int vertNo = f.m_vert[j];
      drawVerts.push_back(m_deformMesh[vertNo]);
    }
  }
  int nDrawVerts = drawVerts.size();
  m_deformMeshVAO = ngl::VertexArrayObject::createVOA(GL_TRIANGLES);
  m_deformMeshVAO->bind();
  m_deformMeshVAO->setData(nDrawVerts * sizeof(vertData), drawVerts[0].u);
  //vertex
  m_deformMeshVAO->setVertexAttributePointer(0, 3, GL_FLOAT, sizeof(vertData), 5);
  m_deformMeshVAO->setVertexAttributePointer(1, 2, GL_FLOAT, sizeof(vertData), 0);
  m_deformMeshVAO->setVertexAttributePointer(2, 3, GL_FLOAT, sizeof(vertData), 2);
  m_deformMeshVAO->setNumIndices(nDrawVerts);
  m_deformMeshVAO->unbind();
}

void SkinDeformer::drawDeformMesh()
{
  m_deformMeshVAO->bind();
  m_deformMeshVAO->draw();
  m_deformMeshVAO->unbind();
}

void SkinDeformer::update()
{
  if (m_skinAlgorithm == LINEAR_BLEND) {
    deformMesh_LSB();
  } else if (m_skinAlgorithm == DUAL_QUATERNION) {
    deformMesh_DQ();
  } else if (m_skinAlgorithm == STRETCH_TWIST) {
    deformMesh_STBS();
  }

  setDeformMeshVAO();
}

void SkinDeformer::deformMesh_LSB()
{

  for (unsigned int i = 0; i < m_nVerts; i++) {
      //getting the bone index
    vertexBoneInfo attachedBones = m_scene->m_vertexBoneData[i];
    ngl::Mat4 totalBoneTransform;
    totalBoneTransform.null();
    for (int j = 0; j < attachedBones.m_nWeights; ++j) {
        //get the bone weight
      ngl::Real weight = attachedBones.m_skinWeights[j];
      //get the bone ID
      unsigned int boneId = attachedBones.m_boneIds[j];
      //get the final transform using the ID
      ngl::Mat4 boneTransform = m_scene->m_boneData[boneId].m_finalTransform;
      totalBoneTransform += (boneTransform * weight);
    }
    //get the orig point
    vertData v = m_origMesh[i];
    ngl::Vec3 origPoint(v.x, v.y, v.z);
    //transform the point
    ngl::Vec3 newPoint = multMatrix(origPoint, totalBoneTransform);
    v.x = newPoint.m_x;
    v.y = newPoint.m_y;
    v.z = newPoint.m_z;
    m_deformMesh[i] = v;
  }
}

//--------------------------------------------------------------------------------
// both DQ and STBS work on rigid transforms
//there any meshes imported should not have scale values
//even a overall scale transformation will not work
//----------------------------------------------------------------------------------
void SkinDeformer::deformMesh_DQ()
{
  DualQuaternion boneTransform;
  DualQuaternion  temp;
  for (unsigned int i = 0; i < m_nVerts; i++) {

    vertexBoneInfo attachedBones = m_scene->m_vertexBoneData[i];
    DualQuaternion totalBoneTransform;
    //initialize to zero
    totalBoneTransform.setNull();
    //storing the first bone dual quaternion for flipping calculation
    DualQuaternion firstBone_dq;
    int firstBoneId = attachedBones.m_boneIds[0];
    //convert matrix to DualQuaternion
    firstBone_dq.fromMatrix(m_scene->m_boneData[firstBoneId].m_finalTransform);
    ngl::Quaternion firstReal(firstBone_dq.getReal());
    for (int j = 0; j < attachedBones.m_nWeights; ++j) {
      ngl::Real weight = attachedBones.m_skinWeights[j];
      unsigned int boneId = attachedBones.m_boneIds[j];
      boneTransform.fromMatrix(m_scene->m_boneData[boneId].m_finalTransform);
      //antipodality checking
      if (Quat_dot(boneTransform.getReal(), firstReal) < 0.0f)
        weight *= -1;
      totalBoneTransform += boneTransform * weight ;
    }
//normalizing Dual qauternion
    totalBoneTransform = totalBoneTransform * (1 / totalBoneTransform.magnitude());
    vertData v = m_origMesh[i];
    ngl::Vec3 origPoint(v.x, v.y, v.z);
    ngl::Vec3 newPoint =  multMatrix(origPoint, totalBoneTransform.toMatrix());
    v.x = newPoint.m_x;
    v.y = newPoint.m_y;
    v.z = newPoint.m_z;
    m_deformMesh[i] = v;
  }
}
//--------------------------------------------------------------------------------
//STBS currently works only on twist meshes as I have only extracted the Z rotation to blend
//
//----------------------------------------------------------------------------------

void SkinDeformer::deformMesh_STBS()
{
  for (unsigned int i = 0; i < m_nVerts; i++) {
    vertexBoneInfo attachedBones = m_scene->m_vertexBoneData[i];
    vertData v = m_origMesh[i];
    ngl::Vec3 origPoint(v.x, v.y, v.z);
    ngl::Vec3 newPoint = 0, finalPos = 0;
    ngl::Real  finalRotation = 0;
    for (int j = 0; j < attachedBones.m_nWeights; ++j) {
      ngl::Vec3 currentPosA, currentPosB;
      ngl::Real weight = attachedBones.m_skinWeights[j];
      unsigned int boneId = attachedBones.m_boneIds[j];
      //get the rest position (bind)of child joint A
      ngl::Vec3 restPosA = m_scene->m_boneData[boneId].m_restPosition;
      ngl::Mat4 boneTransform = m_scene->m_boneData[boneId].m_finalTransform;
      ngl::Mat3 rotA, rotB;
      ngl::Real rotA_Z = 0, rotB_Z = 0, t = 0, finalRotZ;
      //get the rotation3x3 matrix and the translate
      getTranslateRotate(boneTransform, rotA, currentPosA);
      //decompose to euler
      rotA_Z = decomposeRotation(rotA).m_z;
      finalRotZ = rotA_Z;
      //if it has parent
      if (m_scene->m_boneData[boneId].m_parentBoneId != -1) {
        int parentId = m_scene->m_boneData[boneId].m_parentBoneId;
        boneTransform = m_scene->m_boneData[parentId].m_finalTransform;
        ngl::Vec3 restPosB = m_scene->m_boneData[parentId].m_restPosition;
        //initial Bone length
        ngl::Real restLength = (restPosA - restPosB).length();
        ngl::Real currentLength = (currentPosA - currentPosB).length();
        //here I have assumed that the Bone is lying along +Z direction
        // and beyond the origin
        //endPoint weights have smooth falloff from one to another.
        ngl::Real pointLength = origPoint.m_z - restPosA.m_z;
        ngl::Vec3 scalingFactor = ((currentLength / restLength) - 1) * (restPosA - restPosB);
        //calculating the rate of twist along the bone
        t = pointLength / restLength;
        getTranslateRotate(boneTransform, rotB, currentPosB);
        rotB_Z = decomposeRotation(rotB).m_z;
        //using simple linear interpolation instead of matrix to prevent scale loss
        finalRotZ = (rotB_Z * t + (1 - t) * rotA_Z);
      }
      //blending them separately to avoid collapse
      finalPos += weight * currentPosA;
      finalRotation += weight * finalRotZ;
    }
    ngl::Mat3 rotation;
    rotation.rotateZ(-finalRotation);
    //this equation is based
    newPoint = newPoint + (finalPos + rotation * origPoint);
    v.x = newPoint.m_x;
    v.y = newPoint.m_y;
    v.z = newPoint.m_z;
    m_deformMesh[i] = v;
  }
}




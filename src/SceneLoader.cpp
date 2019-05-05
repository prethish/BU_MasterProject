//----------------------------------------------------------------------------------------------------------------------
/// @file SceneLoader.cpp
/// @brief member fucntions of class Sceneloader
/// @author Prethish Bhasuran
/// @version 1.0
/// @date 12/9/14
/// Modified from :-
///NGL demos-skeltal animation
/// @author Jonathan Macey
/// @date 10/10/10
//----------------------------------------------------------------------------------------------------------------------
#include "SceneLoader.h"
#include"AIUtil.h"

bool SceneLoader::load(const std::string &_fname, bool _calcBB)
{
#ifdef ASSIMP_DEBUG
//attaching the asset import to the output and input stream to get the debug messages
  struct aiLogStream stream;
  stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT, NULL);
  aiAttachLogStream(&stream);
// log messages to assimp_log.txt
//  stream = aiGetPredefinedLogStream(aiDefaultLogStream_FILE, "assimp_log.txt");
//  aiAttachLogStream(&stream);
#endif

  m_scene = NULL;
  //load the scene file
  m_scene = m_loader.ReadFile(_fname.c_str(),
                              aiProcessPreset_TargetRealtime_Quality |
                              aiProcess_Triangulate
                             );
  m_rootNode = m_scene->mRootNode;
  m_globalInverse = AIU::aiMatrix4x4ToNGLMat4(m_scene->mRootNode->mTransformation);
  m_globalInverse.inverse();
//the mesh information
  if (m_scene->HasMeshes()) {
    loadPrimitives();
  }

  if (m_scene->HasAnimations()) {
    m_numBones = 0;
    loadBones();
  }

  return true;
}

void SceneLoader::loadPrimitives()
{
  vertData v;
  const aiMesh *sceneMesh = m_scene->mMeshes[0];
  //store the faces
  for (int k = 0; k < sceneMesh->mNumFaces; ++k) {
    ngl::Face f;
    const aiFace face = sceneMesh->mFaces[k];
    f.m_numVerts = face.mNumIndices;
    for (int j = 0; j < f.m_numVerts; ++j) {
      f.m_vert.push_back(face.mIndices[j]);
    }
    m_face.push_back(f);
  }

  m_nFaces = sceneMesh->mNumFaces;
  //store the vertices
  for (int k = 0; k < sceneMesh->mNumVertices; ++k) {
    //normals
    if (sceneMesh->mNormals != NULL) {
      v.nx = sceneMesh->mNormals[k].x;
      v.ny = sceneMesh->mNormals[k].y;
      v.nz = sceneMesh->mNormals[k].z;
    }
    //uvs
    if (sceneMesh->mTextureCoords != NULL && sceneMesh->HasTextureCoords(0)) {
      v.u = sceneMesh->mTextureCoords[0]->x;
      v.v = sceneMesh->mTextureCoords[0]->y;
    }
    //position
    v.x = sceneMesh->mVertices[k].x;
    v.y = sceneMesh->mVertices[k].y;
    v.z = sceneMesh->mVertices[k].z;
    m_vertData.push_back(v);
  }
  m_nVerts = sceneMesh->mNumVertices;
  m_vertexBoneData.resize(m_nVerts);
}

void SceneLoader::loadBones()
{
  unsigned int n = m_scene->mMeshes[0]->mNumBones;
//first pass to build and store the bonedata without parent information
  for (unsigned int i = 0 ; i < n ; ++i) {
    aiBone *bone = m_scene->mMeshes[0]->mBones[i];
    unsigned int BoneIndex = 0;
    std::string boneName(bone->mName.data);
    if (m_boneMapping.find(boneName) == m_boneMapping.end()) {
      // Allocate an index for a new bone
      BoneIndex = m_numBones;
      m_numBones++;
      boneInfo bi;
      bi.m_bindTransform = AIU::aiMatrix4x4ToNGLMat4(bone->mOffsetMatrix);
      //since the inverse matrix is passed ,the rest position will need to be inverted
      //instead of doing a costly invert matrix ,the position is multiplied by -1
      bi.m_restPosition = -ngl::Vec3(bi.m_bindTransform.m_03,
                                     bi.m_bindTransform.m_13,
                                     bi.m_bindTransform.m_23);

      bi.m_parentBoneId = -1;
      // this is the Matrix that transforms from mesh space to bone space in bind pose.
      m_boneMapping[boneName] = BoneIndex;
      m_boneData.push_back(bi);
    } else {
      BoneIndex = m_boneMapping[boneName];
    }
    for (unsigned int j = 0 ; j < bone->mNumWeights ; ++j) {
      unsigned int VertexID = bone->mWeights[j].mVertexId;
      float Weight  = bone->mWeights[j].mWeight;
      m_vertexBoneData[VertexID].addBoneData(BoneIndex, Weight);
    }
  }
//second pass to build the bone parent relationship
  for (unsigned int i = 0 ; i < n ; ++i) {
    aiBone *bone = m_scene->mMeshes[0]->mBones[i];
    std::string boneName(bone->mName.data);
    unsigned int BoneIndex = m_boneMapping[boneName];
    const aiNode* boneNode = m_rootNode->FindNode(boneName.c_str());

    std::string parentBoneName(boneNode->mParent->mName.data);
    if (m_boneMapping.find(parentBoneName) != m_boneMapping.end()) {
      unsigned int parentBoneIndex = m_boneMapping[parentBoneName];
      m_boneData[BoneIndex].m_parentBoneId = parentBoneIndex;
    }

  }
}

void SceneLoader::boneTransform(float _timeInSeconds, std::vector<ngl::Mat4>& o_transforms)
{
  ngl::Mat4 identity(1.0);
  // calculate the current animation time at present this is set to only one animation in the scene and
  // hard coded to animaiton 0 but if we have more we would set it to the proper animation data
  float ticksPerSecond = m_scene->mAnimations[0]->mTicksPerSecond != 0 ? m_scene->mAnimations[0]->mTicksPerSecond : 25.0f;
  float timeInTicks = _timeInSeconds * ticksPerSecond;
  float animationTime = fmod(timeInTicks, m_scene->mAnimations[0]->mDuration);
  // now traverse the animaiton heirarchy and get the transforms for the bones
  recurseNodeHeirarchy(animationTime, m_scene->mRootNode, identity);
  o_transforms.resize(m_numBones);

  for (unsigned int i = 0 ; i < m_numBones ; ++i) {
    // now copy this data note that we need to transpose for NGL useage
    // this data will be copied to the shader and used in the animation skinning
    // process
    o_transforms[i] = m_boneData[i].m_finalTransform.transpose();
  }
}


ngl::Vec3 SceneLoader::calcInterpolatedScaling(float _animationTime, const aiNodeAnim* _nodeAnim)
{
  // this grabs the scale from this frame and the next and returns the interpolated version
  if (_nodeAnim->mNumScalingKeys == 1) {
    return AIU::aiVector3DToNGLVec3(_nodeAnim->mScalingKeys[0].mValue);
  }

  unsigned int scalingIndex = 0;
  for (unsigned int i = 0 ; i < _nodeAnim->mNumScalingKeys - 1 ; ++i) {
    if (_animationTime < (float)_nodeAnim->mScalingKeys[i + 1].mTime) {
      // once we find the data break out of the loop
      scalingIndex = i;
      break;
    }
  }
  unsigned int nextScalingIndex = (scalingIndex + 1);
  assert(nextScalingIndex < _nodeAnim->mNumScalingKeys);
  float deltaTime = _nodeAnim->mScalingKeys[nextScalingIndex].mTime - _nodeAnim->mScalingKeys[scalingIndex].mTime;
  float factor = (_animationTime - (float)_nodeAnim->mScalingKeys[scalingIndex].mTime) / deltaTime;
  //assert(factor >= 0.0f && factor <= 1.0f);
  ngl::Vec3 start = AIU::aiVector3DToNGLVec3(_nodeAnim->mScalingKeys[scalingIndex].mValue);
  ngl::Vec3 end   = AIU::aiVector3DToNGLVec3(_nodeAnim->mScalingKeys[nextScalingIndex].mValue);
  ngl::Vec3 delta = end - start;
  return (start + factor * delta);
}

ngl::Quaternion SceneLoader::calcInterpolatedRotation(float _animationTime, const aiNodeAnim* _nodeAnim)
{
  // we need at least two values to interpolate...
  if (_nodeAnim->mNumRotationKeys == 1) {
    return AIU::aiQuatToNGLQuat(_nodeAnim->mRotationKeys[0].mValue);
  }

  unsigned int rotationIndex = 0;

  for (unsigned int i = 0 ; i < _nodeAnim->mNumRotationKeys - 1 ; ++i) {
    if (_animationTime < (float)_nodeAnim->mRotationKeys[i + 1].mTime) {
      // search for index and break out of loop
      rotationIndex = i;
      break;
    }
  }

  unsigned int nextRotationIndex = (rotationIndex + 1);
  float deltaTime = _nodeAnim->mRotationKeys[nextRotationIndex].mTime - _nodeAnim->mRotationKeys[rotationIndex].mTime;
  float factor = (_animationTime - (float)_nodeAnim->mRotationKeys[rotationIndex].mTime) / deltaTime;
  ngl::Quaternion startRotation = AIU::aiQuatToNGLQuat(_nodeAnim->mRotationKeys[rotationIndex].mValue);
  ngl::Quaternion endRotation   = AIU::aiQuatToNGLQuat(_nodeAnim->mRotationKeys[nextRotationIndex].mValue);
  ngl::Quaternion out = ngl::Quaternion::slerp(startRotation, endRotation, factor);
  out.normalise();
  return out;
}

ngl::Vec3 SceneLoader::calcInterpolatedPosition(float _animationTime, const aiNodeAnim* _nodeAnim)
{
  if (_nodeAnim->mNumPositionKeys == 1) {
    return AIU::aiVector3DToNGLVec3(_nodeAnim->mPositionKeys[0].mValue);
  }

  unsigned int positionIndex = 0;
  for (unsigned int i = 0 ; i < _nodeAnim->mNumPositionKeys - 1 ; ++i) {
    if (_animationTime < (float)_nodeAnim->mPositionKeys[i + 1].mTime) {
      // if we find the key assign and exit search
      positionIndex = i;
      break;
    }
  }
  unsigned int nextPositionIndex = (positionIndex + 1);
  assert(nextPositionIndex < _nodeAnim->mNumPositionKeys);
  float deltaTime = _nodeAnim->mPositionKeys[nextPositionIndex].mTime - _nodeAnim->mPositionKeys[positionIndex].mTime;
  float factor = (_animationTime - (float)_nodeAnim->mPositionKeys[positionIndex].mTime) / deltaTime;
  ngl::Vec3 start = AIU::aiVector3DToNGLVec3(_nodeAnim->mPositionKeys[positionIndex].mValue);
  ngl::Vec3 end = AIU::aiVector3DToNGLVec3(_nodeAnim->mPositionKeys[nextPositionIndex].mValue);

  return ngl::lerp(start, end, factor);
}


const aiNodeAnim* SceneLoader::findNodeAnim(const aiAnimation* _animation, const std::string &_nodeName)
{
  for (unsigned int i = 0 ; i < _animation->mNumChannels ; ++i) {
    const aiNodeAnim* nodeAnim = _animation->mChannels[i];

    if (std::string(nodeAnim->mNodeName.data) == _nodeName) {
      return nodeAnim;
    }
  }

  return NULL;
}

void SceneLoader::recurseNodeHeirarchy(float _animationTime, const aiNode* _node, const ngl::Mat4& _parentTransform)
{
  std::string name(_node->mName.data);
  const aiAnimation* animation = m_scene->mAnimations[0];
  ngl::Mat4 nodeTransform = AIU::aiMatrix4x4ToNGLMat4(_node->mTransformation);
  const aiNodeAnim* nodeAnim = findNodeAnim(animation, name);
  if (nodeAnim) {
    // Interpolate scaling and generate scaling transformation matrix
    ngl::Vec3 scale = calcInterpolatedScaling(_animationTime, nodeAnim);
    ngl::Mat4 scaleMatrix;
    scaleMatrix.scale(scale.m_x, scale.m_y, scale.m_z);
    // Interpolate rotation and generate rotation transformation matrix
    ngl::Quaternion rotation = calcInterpolatedRotation(_animationTime, nodeAnim);
    ngl::Mat4 rotationMatrix = rotation.toMat4();

    // Interpolate translation and generate translation transformation matrix
    ngl::Vec3 translation = calcInterpolatedPosition(_animationTime, nodeAnim);
    // Combine the above transformations
    nodeTransform = rotationMatrix * scaleMatrix;
    nodeTransform.m_30 = translation.m_x;
    nodeTransform.m_31 = translation.m_y;
    nodeTransform.m_32 = translation.m_z;
    nodeTransform.transpose();
  }

  ngl::Mat4 globalTransform = _parentTransform * nodeTransform;
  if (m_boneMapping.find(name) != m_boneMapping.end()) {
    unsigned int boneIndex = m_boneMapping[name];
    m_boneData[boneIndex].m_finalTransform = m_globalInverse * globalTransform * m_boneData[boneIndex].m_bindTransform;
  }

  for (unsigned int i = 0 ; i < _node->mNumChildren ; ++i) {
    recurseNodeHeirarchy(_animationTime, _node->mChildren[i], globalTransform);
  }
}



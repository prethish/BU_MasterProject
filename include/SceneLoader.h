//----------------------------------------------------------------------------------------------------------------------
/// @file SceneLoader.h
/// @brief a basic scene loader using assimp
/// @author Prethish Bhasuran
/// Modified from :-
/// @author Jonathan Macey
/// http://nccastaff.bournemouth.ac.uk/jmacey/Code/AssetImportDemos
/// @date 10/10/10
/// @brief SceneLoader class
/// currently the loader has a couple of restrictions
/// 1.Only one mesh can be loaded at a time
//----------------------------------------------------------------------------------------------------------------------
#ifndef SCENELOADER_H
#define SCENELOADER_H

#include<vector>
#include<map>

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/vector3.h>
#include <assimp/Importer.hpp>

#include<ngl/Camera.h>
#include<ngl/Light.h>
#include<ngl/AbstractMesh.h>
#include<ngl/Quaternion.h>
#include<ngl/Util.h>

#include "DataTypes.h"

//---------------------------------------------------
/// @brief Sceneloader Class to load a single mesh and animated bones
/// the ngl::Abstract mesh class was inherited and assimp used to import mesh
 //---------------------------------------------------
class SceneLoader:public ngl::AbstractMesh
{
public:
    //---------------------------------------------------
    /// @brief constructor
     //---------------------------------------------------
    SceneLoader():AbstractMesh()  {; }
    //---------------------------------------------------
    /// @brief virtual function inherited from Abstractmesh and defined
    /// here using assimp
    /// @param[in] _fname folder path to file
    /// @param[in] _calcBB calculate scene bounding box if true.
     //---------------------------------------------------
    virtual bool load(const std::string &_fname,bool _calcBB=true);
    //---------------------------------------------------
    /// @brief vertex data(UV,Normal,Position) that accessed by skindeformer class
     //---------------------------------------------------
    std::vector <vertData> m_vertData;
    //---------------------------------------------------
    /// @brief per bone data(bind pose Matrix,final transform matrix) that accessed by skindeformer class
     //---------------------------------------------------
    std::vector<boneInfo> m_boneData;
    //---------------------------------------------------
    /// @brief per vertex data that stores the influence bone ids and weights
     //---------------------------------------------------
    std::vector<vertexBoneInfo > m_vertexBoneData;
    //---------------------------------------------------
    /// @brief per vertex endPoint weights
     //---------------------------------------------------
    std::vector<ngl::Real> m_endPointWeights;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief accessor for the number of bones in the mesh
    //----------------------------------------------------------------------------------------------------------------------
    inline unsigned int numBones() const { return m_numBones;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief accessor for the time in seconds of the animation
    //----------------------------------------------------------------------------------------------------------------------
    inline double getDuration() const { return m_scene->mAnimations[0]->mDuration;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief how many tick in the animation per second
    //----------------------------------------------------------------------------------------------------------------------
    inline double getTicksPerSec() const { return m_scene->mAnimations[0]->mTicksPerSecond;}
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this set the bone transformation for the current time. This is then passed to the shader
    /// to do the animation of the mesh
    /// @param[in] _timeInSeconds the time to set the animation frame to
    /// @param[out] _transforms an array of transform matrices for the current frame
    //----------------------------------------------------------------------------------------------------------------------
    void boneTransform(float _timeInSeconds, std::vector<ngl::Mat4>& o_transforms);

    ngl::Face getFace(unsigned int _index)
    {
        ngl::Face f=getFaceList()[_index];
        return f;
    }

private:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief root node matrix inverse
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Mat4 m_globalInverse;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Assimp data that stores the entire upon using the importer
    //----------------------------------------------------------------------------------------------------------------------
    const aiScene * m_scene;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Assimp node that stores the root node data,used to build a heirarchy for the skeleton
    //----------------------------------------------------------------------------------------------------------------------
     aiNode *m_rootNode;
     //----------------------------------------------------------------------------------------------------------------------
     /// @brief Assimp Importer
     //----------------------------------------------------------------------------------------------------------------------
    Assimp::Importer m_loader;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief maps a bone name to its index
    //----------------------------------------------------------------------------------------------------------------------
    std::map<std::string,unsigned int> m_boneMapping;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief number of bones in the mesh
    //----------------------------------------------------------------------------------------------------------------------
    unsigned int m_numBones;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief calculate the scale value between two keys
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Vec3 calcInterpolatedScaling(float _animationTime, const aiNodeAnim* _nodeAnim);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief calculate the rotation value between two keys
    //---------------------------------------------------------------------------------------------------------------------
    ngl::Quaternion calcInterpolatedRotation(float _animationTime, const aiNodeAnim* _nodeAnim);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief calculate the position value between two keys
    //----------------------------------------------------------------------------------------------------------------------
    ngl::Vec3 calcInterpolatedPosition(float _animationTime, const aiNodeAnim* _nodeAnim);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief find the current node animation
    //----------------------------------------------------------------------------------------------------------------------
    const aiNodeAnim* findNodeAnim(const aiAnimation* _animation, const std::string &_nodeName);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief recurse the node for the next animation node
    //----------------------------------------------------------------------------------------------------------------------
    void recurseNodeHeirarchy(float _animationTime, const aiNode* _node, const ngl::Mat4& _parentTransform);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Load all meshes and store the data
    //----------------------------------------------------------------------------------------------------------------------
    void loadPrimitives();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief load bone data and also create a skeleton heriarchy
    //----------------------------------------------------------------------------------------------------------------------
    void loadBones();
};

#endif // SCENELOADER_H

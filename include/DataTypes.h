//----------------------------------------------------------------------------------------------------------------------
/// @file DataTypes.h
/// @brief the commonly used struct data types stored here to access
/// between classes
/// @author Prethish Bhasuran
/// @version 1.0
/// @date 12/9/14
/// @class NGLScene
//----------------------------------------------------------------------------------------------------------------------
#ifndef DATATYPES_H
#define DATATYPES_H

#include<vector>
#include<string>
#include<iostream>

#include<ngl/Mat4.h>
#include<ngl/Vec3.h>


// -------------------------------------
/// @brief vertex data structure containing position,UV
/// and normal information
// ------------------------------------------
struct vertData
{
    //------------------
    /// @brief UV coordinates at position 0,1 in structure
    //--------------------
  float u;
  float v;
  //------------------
  /// @brief normal coordinates at position 2,3,4 in structure
  //--------------------
  float nx;
  float ny;
  float nz;
  //------------------
  /// @brief vertex coordiantes at position 5,6,7 in structure
  //--------------------
  float x;
  float y;
  float z;
};

//-----------------------------------------------
/// @brief struct to store bone data per bone for skinning
///the matrix stored are in the assimp order,so transpose must be used to use
/// with NGL::mat4 matrices
//---------------------------------------------------
struct boneInfo
{
    //------------------
    /// @brief final world transformation for animation
    //--------------------
    ngl::Mat4 m_finalTransform;
    //------------------
    /// @brief bind pose world transformation
    //--------------------
    ngl::Mat4 m_bindTransform;
    //------------------
    /// @brief bind pose rest position in vector3d
    //--------------------
    ngl::Vec3 m_restPosition;
    //------------------
    /// @brief parent IDs of the bone connected with the current bone
    /// for this project i have not coded for multiple parents
    //--------------------
    int m_parentBoneId;

};

//-----------------------------------------------
/// @brief structure to store data per vertex for skinning
///it stores weights per bone and the bone IDs corresponding to the weights
/// usually the number of weights per bone is limited to 4 for standard use.
//---------------------------------------------------
struct vertexBoneInfo
{
    //-----------------------------------------------
    /// @brief constructor
    //---------------------------------------------------
    vertexBoneInfo()
    {
        m_nWeights=0;
    }
    //------------------
    /// @brief Number of bones that influences the vertex
    //--------------------
    short int m_nWeights;
    //------------------
    /// @brief The ids of the bones the influences the vertex,
    /// the index of boneData in array of struct boneInfo is the ID
    //--------------------
    std::vector<int> m_boneIds;
    //------------------
    /// @brief the weights of each bone pre vertex
    /// usually adding all weights will equal to 1
    //--------------------
    std::vector<ngl::Real> m_skinWeights;
    //------------------
    /// @brief the end weights pre vertex
    //--------------------
    std::vector<ngl::Real> m_endWeights;

    //-----------------------------------------------
    /// @brief member function to add the bone data and increment the number of weights
    ///@param[in] BoneID
    ///@param[in] Weight
    //---------------------------------------------------
    void addBoneData(uint BoneID, float Weight)
    {
        m_boneIds.push_back(BoneID);
        m_skinWeights.push_back(Weight);
        m_endWeights.push_back(Weight);
        m_nWeights++;
    }

};




#endif // DATATYPES_H

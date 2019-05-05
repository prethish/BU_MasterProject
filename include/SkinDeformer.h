//----------------------------------------------------------------------------------------------------------------------
/// @file SkinDeformer.h
/// @brief the main SkinDeformer class that deforms the mesh based on the skinning algorithm used
/// @author Prethish Bhasuran
/// @version 1.0
/// @date 12/9/14
/// @class SkinDeformer
///@brief it first creates and stores the vertex data from the scene data passed to it
/// A vertexArrayObject for openGL drawing is also created for the deformed mesh
/// Based on the skinning algorithm selected the mesh is deformed
//----------------------------------------------------------------------------------------------------------------------


#ifndef SKINDEFORMER_H
#define SKINDEFORMER_H

#include<ngl/Vec3.h>
#include<ngl/Obj.h>
#include<ngl/Mat3.h>
#include<ngl/VertexArrayObject.h>
#include<ngl/VAOPrimitives.h>

#include "SceneLoader.h"
#include "DataTypes.h"

//-----------------------------------------------
/// @brief enum desribing the different skin Algorithms
//---------------------------------------------------
enum SkinDeformTypes{
    LINEAR_BLEND,DUAL_QUATERNION,STRETCH_TWIST
};


class SkinDeformer
{
public:
    //-----------------------------------------------
    /// @brief default constructor
    //---------------------------------------------------
    SkinDeformer();

    //-----------------------------------------------
    /// @brief destructor to uninitialize the setVAO and delete pointer objects
    //---------------------------------------------------
    ~SkinDeformer();

    //-----------------------------------------------
    /// @brief function that is called to draw the deformed mesh Using VAO
    //---------------------------------------------------
    void drawDeformMesh();

    //-----------------------------------------------
    /// @brief update the defomed mesh based on the update scene data
    /// calls the corresponding skin deformer based on the set algorithm
    //---------------------------------------------------
    void update();

    //-----------------------------------------------
    /// @brief this sets a pointer to the scene data and also creates a
    /// copy of the vertex data
    /// @param[is] SceneLoader sceneData
    //---------------------------------------------------
    void setMeshData(SceneLoader *_scene);

    //-----------------------------------------------
    /// @brief function to set the Skinning algorithm
    ///param[in] _i index
    //---------------------------------------------------
    void setSkinAlgorithm(int _i);

private:
    //-----------------------------------------------
    /// @brief vertex data that is used to draw the deformed mesh
    //---------------------------------------------------
    std::vector<vertData> m_deformMesh;
    //-----------------------------------------------
    /// @brief the original mesh data
    //---------------------------------------------------
    std::vector<vertData> m_origMesh;
    //-----------------------------------------------
    /// @brief total number of vertices
    //---------------------------------------------------
    unsigned int m_nVerts;
    //-----------------------------------------------
    /// @brief deformed mesh VAO to draw in OpenGL
    //---------------------------------------------------
    ngl::VertexArrayObject *m_deformMeshVAO;
    //-----------------------------------------------
    /// @brief Boolean variable to check if the deformed is initilazed
    //---------------------------------------------------
    bool m_meshSet;
    //-----------------------------------------------
    /// @brief pointer to the imported scene data
    //---------------------------------------------------
    SceneLoader *m_scene;
    //-----------------------------------------------
    /// @brief enum to select the skinAlgorithm
    //---------------------------------------------------
    SkinDeformTypes m_skinAlgorithm;

    //-----------------------------------------------
    /// @brief deform and set the deformed vertices using linear blend algorithm
    //---------------------------------------------------
    void deformMesh_LSB();
    //-----------------------------------------------
    /// @brief deform and set the deformed vertices using Dual Quaternion algorithm
    //---------------------------------------------------
    void deformMesh_DQ();
    //-----------------------------------------------
    /// @brief deform and set the deformed vertices using Stretch and twistable algorithm
    //---------------------------------------------------
    void deformMesh_STBS();
     //-----------------------------------------------
     /// @brief set the VAO from the deformed vertex data for OpenGL
     //---------------------------------------------------
     void setDeformMeshVAO();
};

#endif // SKINDEFORMER_H

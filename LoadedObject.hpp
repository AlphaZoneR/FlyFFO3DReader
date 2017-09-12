#pragma once
#include "Utils.h"
#include "CTextureManager.hpp"
#include "file.hpp"
int	LoadObject(LPCTSTR);

class LoadedObject{
private:
    char * m_szFileName;
    int m_nID, m_nMaxFrame, m_nMaxEvent, m_bLOD, m_nMaxBone, m_bSendVS, m_nMaxFace;
    D3DXVECTOR3 m_vForce1, m_vForce2, m_vForce3, m_vForce4, m_vBBMin, m_vBBMax, m_vEvent[MAX_MDL_EVENT];
    float m_fScrlU, m_fScrlV, m_fPerSlerp;
    D3DXMATRIX * m_pBaseBone, * m_pBaseBoneInv;
    CMotion * m_pMotion;
    LOD_GROUP m_Group[ MAX_GROUP ];
    MOTION_ATTR * m_pAttr;
    BOOL m_nHavePhysique;
    CTextureManager g_TextureMng;
    GMOBJECT * pObject;

    std::vector<int> indices;
    std::vector<float> vertexList;
    std::vector< pair<float, float> > uvs;
    std::string texName;


public:
    LoadedObject();
    LoadedObject(const char *);

    ~LoadedObject(){
        this->m_bLOD = 0;
    }


    std::vector<float> getVerticeList();
    std::vector<int> getIndiceList();

    std::vector< pair <float, float> > getUvs();

    const char * getTextureName();

    int LoadTMAni( CResFile *, GMOBJECT *);
    int	LoadObject(LPCTSTR);
    int LoadGMObject(CResFile *, GMOBJECT *);
};

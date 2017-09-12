#pragma once
#include "Utils.h"
#include "file.hpp"


struct TM_ANIMATION
{
//	D3DXMATRIX			m_mAniTM;
	D3DXQUATERNION		m_qRot;
	D3DXVECTOR3			m_vPos;
};

struct	MOTION_ATTR
{
	DWORD	m_dwAttr;

	int		m_nSndID;
	float	m_fFrame;
};



struct	BONE_FRAME
{
	TM_ANIMATION	*m_pFrame;
	D3DXMATRIX		m_mLocalTM;
};

struct BONE;
struct BONE
{
	BONE		*m_pParent;			// »óÀ§ ºÎ¸ð ³ëµå

//	int			m_nID;				// °èÃþ¼ø¼­ ´ë·Î ºÎ¿©µÈ ¹øÈ£ - ¾îÂ÷ÇÇ ÇÊ¿ä¾øÀ»°Å °°´Ù.
	int			m_nParentIdx;		// ºÎ¸ð ÀÎµ¦½º
	char		m_szName[32];		// * ½ºÆ®¸µÇüÅÂÀÇ ³ëµå ÀÌ¸§
	D3DXMATRIX		m_mTM;		// ase¿øº» TM
	D3DXMATRIX		m_mInverseTM;	// * ¹Ì¸® º¯È¯µÈ ÀÎ¹ö½ºTM
	D3DXMATRIX		m_mLocalTM;	// * ¹Ì¸®º¯È¯µÈ ·ÎÄÃ TM
};



class CMotion
{
private:
	TM_ANIMATION	*m_pMotion;			// »À´ë¼ø¼­´ë·Î °¢°¢ÀÇ ¾Ö´Ï¸ÞÀÌ¼ÇÀÌ µé¾îÀÖ´Ù.  ¾Ö´Ï°¡ ¾ø´Â »À´ë´Â LocalTM¸¸ ÀÖ´Ù.
	MOTION_ATTR		*m_pAttr;			// ÇÁ·¹ÀÓÅ©±â¸¸Å­ ÇÒ´ç.  °¢ ÇÁ·¹ÀÓ¼Ó¼º.
	int		m_nMaxFrame;				// ¸ð¼Ç ÇÁ·¹ÀÓ¼ö
public:
	int		m_nID;						// ¸ð¼ÇÀÇ °íÀ¯ ID(ÆÄÀÏ³»¿¡ ÀÖ´Ù)
	char	m_szName[64];				// ¸ð¼Ç ÆÄÀÏ¸í - ³ªÁß¿¡ ÀÌ°Ç ºüÁø´Ù.
	float	m_fPerSlerp;				//
	int		m_nMaxBone;					// ÀÌ°ÍÀº ¾îµð±îÁö³ª È®ÀÎ¿ë º¯¼öÀÏ»ÓÀÌ´Ù.  ½ÇÁ¦ º»ÀÇ °¹¼ö°¡ ÇÊ¿äÇÏ´Ù¸é CBone¿¡¼­ ÀÐ¾î¾ß ÇÑ´Ù.
	D3DXVECTOR3	m_vPivot;				// 0,0,0(¹ß)±âÁØ¿¡¼­ °ñ¹Ý(·çÆ®)ÀÇ ÁÂÇ¥.  0¹øÅ°ÀÇ ÁÂÇ¥¿Í °°´Ù.
	BONE_FRAME	*m_pBoneFrame;		// »À´ë°¹¼ö ¸¸Å­ ÇÒ´ç. ·ÎµùÀÌ ³¡³­ÈÄ »ç¿ëÇÏ±â ÁÁµµ·Ï Æ÷ÀÎÅÍ¸¦ Àç¹èÄ¡ÇÑ´Ù.  ÇÁ·¹ÀÓÀÌ ¾ø´Â°÷Àº NULL
	BONE		*m_pBoneInfo;		// »À´ë±¸¼ºÁ¤º¸
	int		m_nMaxEvent;
	D3DXVECTOR3	m_vEvent[4];
	D3DXVECTOR3		*m_pPath;			// path

	CMotion();
	~CMotion();

	void Init( void );
	void Destroy( void );

	MOTION_ATTR *GetMotionAttr( int nNumFrm ) { return m_pAttr + nNumFrm; }
	DWORD	GetAttr( int nNumFrm )
	{
		return m_pAttr[ nNumFrm ].m_dwAttr;
	}
//#if 0
	DWORD	IsAttrHit( float fOldFrm, float fNumFrm )
	{
		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( pAttr->m_dwAttr & MA_HIT )
			if( fOldFrm < pAttr->m_fFrame && pAttr->m_fFrame <= fNumFrm )	// ÀÌÀü ÇÁ·¹ÀÓÀÌ¶û ÇöÀç ÇÁ·¹ÀÓ »çÀÌ¿¡ Å¸Á¡ÀÌ µé¾îÀÖ¾ú´Â°¡.
				return pAttr->m_dwAttr;
			return 0;
	}
	MOTION_ATTR *IsAttrSound( float fOldFrm, float fNumFrm )
	{
		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( pAttr->m_dwAttr & MA_SOUND )
			if( fOldFrm < pAttr->m_fFrame && pAttr->m_fFrame <= fNumFrm )	// ÀÌÀü ÇÁ·¹ÀÓÀÌ¶û ÇöÀç ÇÁ·¹ÀÓ »çÀÌ¿¡ Å¸Á¡ÀÌ µé¾îÀÖ¾ú´Â°¡.
				return pAttr;
			return NULL;
	}
	DWORD	IsAttrQuake( float fOldFrm, float fNumFrm )
	{
		MOTION_ATTR		*pAttr = &m_pAttr[ (int)fNumFrm ];
		if( pAttr->m_dwAttr & MA_QUAKE )
			if( fOldFrm < pAttr->m_fFrame && pAttr->m_fFrame <= fNumFrm )	// ÀÌÀü ÇÁ·¹ÀÓÀÌ¶û ÇöÀç ÇÁ·¹ÀÓ »çÀÌ¿¡ Å¸Á¡ÀÌ µé¾îÀÖ¾ú´Â°¡.
				return pAttr->m_dwAttr;
			return 0;
	}
	void	SetAttr( float fNumFrm, DWORD dwAttr )
	{
		m_pAttr[ (int)fNumFrm ].m_dwAttr |= dwAttr;
		m_pAttr[ (int)fNumFrm ].m_fFrame = fNumFrm;
	}
	void	ResetAttr( int nNumFrm, DWORD dwAttr ) { m_pAttr[ nNumFrm ].m_dwAttr &= (~dwAttr); }
	TM_ANIMATION*	GetFrame( int nNumFrm ) { return &m_pMotion[nNumFrm]; }
	int		GetMaxFrame( void ) { return m_nMaxFrame; }

	int		LoadMotion( LPCTSTR strFileName );			// ÆÄÀÏ¿¡¼­ ¸ð¼ÇÆÄÀÏÀ» ÀÐÀ½
	void	ReadTM( CResFile *file, int nNumBone, int nNumFrame );

	void	AnimateBone( D3DXMATRIX *pmUpdateBone, CMotion *pMotionOld, float fFrameCurrent, int nNextFrame, int nFrameOld, BOOL bMotionTrans, float fBlendWeight );

};


enum GMTYPE
{
	GMT_ERROR = -1,
	GMT_NORMAL,
	GMT_SKIN,
	GMT_BONE
};

enum VBTYPE
{
	VBT_ERROR = -1,
	VBT_NORMAL,
	VBT_BUMP,
};

struct GMOBJECT;
//
// ¹öÅØ½º¿Í ¸éÁ¤º¸¸¦ °®´Â ¸Þ½¬ÇüÅÂÀÇ µ¥ÀÌÅ¸ ±¸Á¶
// Geometry Object
//
struct MATERIAL
{
	D3DMATERIAL9			m_Material;
	LPDIRECT3DTEXTURE9		m_pTexture;		// ÅØ½ºÃÄ¸Þ¸ð¸® Æ÷ÀÎÅÍ
	char			strBitMapFileName[64];	// ÅØ½ºÃÄ ÆÄÀÏ¸í
	LPDIRECT3DDEVICE9       m_pd3dDevice;
	int						m_nUseCnt;		// °°Àº ÅØ½ºÃÄ°¡ ¸î¹ø »ç¿ëµÆ´ÂÁö ...
	BOOL					m_bActive;
};


struct	MATERIAL_BLOCK
{
	int		m_nStartVertex;
	int		m_nPrimitiveCount;
	int		m_nTextureID;
	DWORD	m_dwEffect;
	int		m_nAmount;
	int		m_nMaxUseBone;
	int		m_UseBone[MAX_VS_BONE];
};
struct GMOBJECT		// base object
{
	GMTYPE	m_Type;					// GMOBJECT Type
	int			m_nVertexSize;		// sizeof(FVF)
	DWORD		m_dwFVF;			// FVF Æ÷¸Ë
	int			m_nMaxUseBone;
	int			m_UseBone[ MAX_VS_BONE ];
	int			m_nID;
	int			m_bOpacity;
	int			m_bBump;
	int			m_bRigid;
	int			m_bLight;
	int			m_nMaxVertexList;			// read
	int			m_nMaxFaceList;				// read
	int			m_nMaxMtrlBlk;				// read
	int			m_nMaxMaterial;				// read
	GMTYPE		m_ParentType;				// ºÎ¸ðÀÇ Å¸ÀÔ
	int			m_nParentIdx;				// read		// ½ºÅ²ÀÏ¶© º»ÀÇ ÀÎµ¦½º°¡ µÇ°í ÀÏ¹ÝÀÏ¶© GMOBJECTÀÇ ÀÎµ¦½º
	GMOBJECT*	m_pParent;					// ºÎ¸ð, ¹ÙÀÌÆäµå¸¦ ºÎ¸ð·Î °¡Áø ¸µÅ©¸Þ½Ã´Â ÀÌ°É¾È¾´´Ù.
	D3DXMATRIX		m_mLocalTM;			// read
	D3DXMATRIX		m_mInverseTM;			// read ¹Ì¸® º¯È¯µÈ ÀÎ¹ö½ºTM

	D3DXVECTOR3		*m_pVertexList;			// read °¢ °üÀý¿¡ ´ëÇØ LocalÁÂÇ¥°¡ µé¾î°¨
//	INDEX_BUFFER	*m_pIndexBuffer;		// read
	int				*m_pPhysiqueVertex;		// read ÇÇÁöÅ©, °¢ ¹öÅØ½º ÇÇÁöxÅ©µéÀÌ ÂüÁ¶ÇÏ´Â º» ID
	MATERIAL_BLOCK	*m_pMtrlBlk;			// read
	LPDIRECT3DTEXTURE9	*m_pMtrlBlkTexture;		// ¸ÅÅÍ¸®¾ó ºí·°³» ÅØ½ºÃÄÆ÷ÀÎÅÍ

#ifdef __YENV
	VBTYPE		m_VBType;
	LPDIRECT3DTEXTURE9	*m_pNormalTexture;		// ³ë¸»¸Ê(¹üÇÁ¿ë)
	LPDIRECT3DTEXTURE9	*m_pNoSpecTexture;		// ³ë½ºÆÑÅ§·¯¸Ê(¹üÇÁ¿ë)
#endif //__YENV

	TM_ANIMATION	*m_pFrame;				// ¾Ö´Ï¸ÞÀÌ¼ÇÀÌ ÀÖÀ¸¸é ÇÁ·¹ÀÓ¼ö¸¸Å­ ÀÖ´Ù

	D3DXVECTOR3		m_vBBMin, m_vBBMax;			// read bound box min, max
//	D3DXVECTOR3		m_vBBVList[8];				// read bound box vlist

	int				m_bMaterial;				// MESÆÄÀÏ¿¡ ¸ÅÅÍ¸®¾óÀÌ ÀÖ¾ú´ÂÁö?.  ÀúÀåÀ» À§ÇØ¼­ ¹é¾÷ ¹Þ¾ÆµÒ
	MATERIAL		m_MaterialAry[16];			// MES¿¡ ÀúÀåµÇ¾î ÀÖ´ø MaterialÁ¤º¸.  Save¶§³ª ´Ù½Ã ¸ÅÅÍ¸®¾óÀ» ¼ÂÆÃ ÇØ¾ßÇÒ ÇÊ¿ä°¡ ÀÖÀ»¶§ »ç¿ë

	// void*ÀÇ VB¸¦ ¾µ¶© Ç×»ó Ä³½ºÆÃÀ» ÇÒ°Í.
//	NORMALVERTEX	*m_ptempVB;
	void			*m_pVB;			// Indexed¿ë ¹öÅØ½º ¹öÆÛ - ¹öÅØ½º ¸®½ºÆ®¿Í ´Ù¸¥Á¡Àº ÁÂÇ¥´Â °°À¸³ª TU,TV°¡ ´Ù¸¥ ¹öÅØ½ºµéÀ» µû·Î »©³½°ÍÀÌ´Ù. matIdx´Â °¡Áö°í ÀÖÁö ¾Ê´Ù.

	WORD			*m_pIB;			// ÀÎµ¦½º ¹öÆÛ, m_pIIndexBufferÀÇ º¹»çº».
	WORD			*m_pIIB;		// m_pVB°¡ ¾î´À m_pVertexList¿¡ ´ëÀÀµÇ´ÂÁö¿¡ ´ëÇÑ ÀÎµ¦½º, m_nMaxIVB, ½ºÅ°´×¿¡ ¾²ÀÓ.  ¹öÅØ½º ¹öÆÛ¿Í ¹öÅØ½º¸®½ºÆ®ÀÇ Å©±â°¡ ´Ù¸£±â¶§¹®¿¡ ÀÌ·±°Ô ÇÊ¿äÇÏ´Ù.
	int				m_nMaxVB;
	int				m_nMaxIB;

	// ½ºÅ²Çü ¿ÀºêÁ§Æ®ÀÏ¶§´Â VB´Â NULLÀÌ´Ù. ¿ÜºÎÁöÁ¤VB¸¦ µû·Î ½á¾ß ÇÑ´Ù.
	LPDIRECT3DVERTEXBUFFER9		m_pd3d_VB;		// Indexed¿ë VB
	LPDIRECT3DINDEXBUFFER9		m_pd3d_IB;		// Indexed¿ë IB

	// temporary - ÀÏ½ÃÀûÀ¸·Î writeÇÔ.
//	SKINVERTEX		*_pVB;					// ½ºÅ°´×¿ÀºêÁ§Æ®¿¡¸¸ »ç¿ë. d3d_VB¿Í °°Àº ³»¿ë. ÀåÂ÷ »ç¶óÁúÁöµµ...


};

struct LOD_GROUP
{
	int			m_nMaxObject;
	GMOBJECT*	m_pObject;		// sizeof(GMOBJECT) * m_nMaxObject
	// °èÃþ±¸Á¶ ¾Ö´Ï¸ÞÀÌ¼ÇÈÄ °»½ÅµÈ ¸ÅÆ®¸¯½ºµéÀÌ ´ã±æ ÀÓ½Ã¹öÆÛ m_nMaxObject¸¸Å­ »ý¼º.
	// ¸¸¾à ½ºÅ²¸¸ ÀÖ´Â ¿ÀºêÁ§Æ®¶ó¸é ÀÌ°ÍÀº »ý¼ºµÇÁö ¾Ê´Â´Ù.
	D3DXMATRIX	*_mUpdate;
};




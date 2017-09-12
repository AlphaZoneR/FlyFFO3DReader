//#define __YENV
#define __VER 15
#include "LoadedObject.hpp"
//#include "CMotion.h"


LoadedObject::LoadedObject(const char * fileName){
    this->LoadObject(fileName);
}


LoadedObject::LoadedObject(){
    this->m_bLOD = 0;
}

int	LoadedObject::LoadObject( LPCTSTR szFileName )
{
	int		j, nTemp;
	int		nVer;
	D3DXVECTOR3	vTemp;
	BOOL	bNormalObj = FALSE;

	CResFile resFp;

	BOOL bRet = resFp.Open(szFileName, "rb");
	if( bRet == FALSE )
	{
	    printf("No such file as: %s", szFileName);
		return 0;
	}
 	char szName[MAX_PATH];
	char buff[MAX_PATH];
	char cLen;
	resFp.Read( &cLen, 1, 1 );
	resFp.Read( buff, cLen, 1 );
	for( j = 0; j < cLen; j ++ )
		buff[j] = buff[j] ^ (char)0xcd;

	if( cLen >= 64 )
	{
		return 0;
	}
	buff[cLen] = 0;

	resFp.Read( &nVer, 4, 1 );
	if( nVer < VER_MESH )
	{
		resFp.Close();
		return 0;
	}
	resFp.Read( &m_nID, 4, 1 );		// Serial ID
	resFp.Read( &m_vForce1, sizeof(D3DXVECTOR3), 1 );		// °Ë±¤1,2ÀÇ ÁÂÇ¥ÀÎµ¥ ÀÏ´Ü ÀÌ·¸°Ô ÇÏÀÚ.
	resFp.Read( &m_vForce2, sizeof(D3DXVECTOR3), 1 );

	if(nVer >= 22)
	{
		resFp.Read( &m_vForce3, sizeof(D3DXVECTOR3), 1 );		// °Ë±¤3,4ÀÇ ÁÂÇ¥ÀÎµ¥ ÀÏ´Ü ÀÌ·¸°Ô ÇÏÀÚ.
		resFp.Read( &m_vForce4, sizeof(D3DXVECTOR3), 1 );
	}


	resFp.Read( &m_fScrlU, sizeof(float), 1 );
	resFp.Read( &m_fScrlV, sizeof(float), 1 );
	resFp.Seek( 16, SEEK_CUR );		// reserved

	resFp.Read( &m_vBBMin, sizeof(D3DXVECTOR3), 1 );		// ´ëÇ¥ ¹Ù¿îµù ¹Ú½º
	resFp.Read( &m_vBBMax, sizeof(D3DXVECTOR3), 1 );
	resFp.Read( &m_fPerSlerp, sizeof(float), 1 );		// per slerp
	resFp.Read( &m_nMaxFrame, 4, 1 );					// ani frame ¼ö.  ¾Ö´Ï°¡ ¾øÀ¸¸é 0ÀÌµÇµµ·Ï ÀúÀåÇÒ°Í.

	resFp.Read( &m_nMaxEvent, 4, 1 );
	if( m_nMaxEvent > 0 )
		resFp.Read( m_vEvent, sizeof(D3DXVECTOR3) * m_nMaxEvent, 1 );

	resFp.Read( &nTemp, 4, 1 );
	cout << nTemp << endl;
	if( nTemp )
	{
	    this->pObject->m_Type = GMT_NORMAL;
        LoadGMObject(&resFp, this->pObject);
	}
	resFp.Read( &m_bLOD, 4, 1 );					// LOD°¡ ÀÖ´Â°¡?

	//--- ÀÚÃ¼³»Àå º» ¾Ö´Ï¸ÞÀÌ¼ÇÀÌ ÀÖ´Ù¸é º» °³¼ö°¡ ÀÖÀ»°ÍÀÌ´Ù.  ex) Obj_Ç³¼±.o3d    parts_female.o3d, mvr_Å¸Á¶.o3d´Â º»ÆÄÀÏÀÌ µû·Î ÀÖÀ½.
	resFp.Read( &m_nMaxBone, 4, 1 );
	if( m_nMaxBone > 0 )
	{
		m_pBaseBone = new D3DXMATRIX[ m_nMaxBone * 2 ];
		m_pBaseBoneInv = m_pBaseBone + m_nMaxBone;		// InverseTMÀº µÚÂÊ¿¡ ºÙ´Â´Ù.
		resFp.Read( m_pBaseBone,	   sizeof(D3DXMATRIX) * m_nMaxBone, 1 );		// µðÆúÆ® »À´ë ¼ÂÆ®
		resFp.Read( m_pBaseBoneInv, sizeof(D3DXMATRIX) * m_nMaxBone, 1 );		// InverseTM ¼¼Æ®
		if( m_nMaxFrame > 0 )	// º»ÀÌ ÀÖ°í MaxFrameÀÌ ÀÖÀ¸¸é ¾Ö´Ï¸ÞÀÌ¼ÇÀÌ ÀÖ´Ù´Â°É·Î °£ÁÖ.
		{
			m_pMotion = new CMotion;
			m_pMotion->ReadTM( &resFp, m_nMaxBone, m_nMaxFrame );		// º» ¾Ö´Ï¸ÞÀÌ¼Ç ÀÐÀ½.
		}
		resFp.Read( &m_bSendVS, 4, 1 );		// º»°³¼ö°¡ MAX_VS_BONEº¸´Ù Àû¾î VS·Î ÇÑ¹ø¿¡ Àü¼Û°¡´ÉÇÑ °ÍÀÎ°¡?
	}

	int nMaxGroup = (m_bLOD) ? MAX_GROUP : 1;	// LOD°¡ ÀÖÀ¸¸é 3°³´Ù ÀÐÀ½. ¾øÀ¸¸é 1°³¸¸ ÀÐÀ½.
	LOD_GROUP *pGroup;
	int nPoolSize, nDebugSize = 0;
	resFp.Read( &nPoolSize, 4, 1 );		// ¸Þ¸ð¸® Ç® »çÀÌÁî.
	GMOBJECT *pPool = new GMOBJECT[ nPoolSize ];	// ¸Þ¸ð¸® Ç®.
	if( pPool == NULL )
		printf( "¸Þ¸ð¸® ÇÒ´ç ½ÇÆÐ:CObject3D::LoadObject( %s ) %d", m_szFileName, nPoolSize );
	memset( pPool, 0, sizeof(GMOBJECT) * nPoolSize );
	for( int i = 0; i < nMaxGroup; i ++ )
	{
		pGroup = &m_Group[i];
		resFp.Read( &pGroup->m_nMaxObject, 4, 1 );		// geometry °¹¼ö

		pGroup->m_pObject = pPool;		// ¸Þ¸ð¸® Æ÷ÀÎÅÍ ÇÒ´ç.
		pPool += pGroup->m_nMaxObject;

		nDebugSize += pGroup->m_nMaxObject;
		if( nDebugSize > nPoolSize )
			printf( "LoadObject : ¸Þ¸ð¸® ¿À¹öÇÃ·Î¿ì %d, %d", nPoolSize, nDebugSize );

		memset( pGroup->m_pObject, 0, sizeof(GMOBJECT) * pGroup->m_nMaxObject );
		for( j = 0; j < pGroup->m_nMaxObject; j ++ )		pGroup->m_pObject[j].m_nID = -1;

		GMOBJECT	*pObject;
		int		nParentIdx;
		BOOL	bAnimate = FALSE;
		int		nType;
		//------  Geometry Object
		cout << pGroup->m_nMaxObject << endl;
		for( j = 0; j < pGroup->m_nMaxObject; j ++ )
		{
			pObject = &pGroup->m_pObject[j];

			resFp.Read( &nType, 4, 1 );	// Type
			pObject->m_Type = (GMTYPE)(nType & 0xffff);
			if( nType & 0x80000000 )
				pObject->m_bLight = TRUE;

			resFp.Read( &pObject->m_nMaxUseBone, 4, 1 );
			if( pObject->m_nMaxUseBone > 0 )
				resFp.Read( pObject->m_UseBone, sizeof(int) * pObject->m_nMaxUseBone, 1 );	// ¿ÀºêÁ§Æ®°¡ »ç¿ëÇÏ´Â º»¸®½ºÆ®


#if !defined(__YENV)
			if( pObject->m_Type == GMT_SKIN )
			{
				pObject->m_nVertexSize = sizeof(SKINVERTEX);
				pObject->m_dwFVF	   = D3DFVF_SKINVERTEX;
			} else
			{
				pObject->m_nVertexSize = sizeof(NORMALVERTEX);
				pObject->m_dwFVF	   = D3DFVF_NORMALVERTEX;
			}
#endif //__YENV

			resFp.Read( &pObject->m_nID, 4, 1 );	// Object ID
			resFp.Read( &nParentIdx, 4, 1 );			// parent idx
			pObject->m_nParentIdx = nParentIdx;		// save¸¦ À§ÇØ ¹é¾÷ ¹Þ¾ÆµÒ

			// ºÎ¸ð°¡ ÀÖ´Ù¸é ºÎ¸ð Æ÷ÀÎÅÍ ÁöÁ¤
			if( nParentIdx != -1 )
			{
				resFp.Read( &pObject->m_ParentType, 4, 1 );		// ºÎ¸ðÀÇ Å¸ÀÔ ÀÐÀ½.
				// ºÎ¸ðÀÇ Æ÷ÀÎÅÍ¸¦ ¼¼ÆÃ
				switch( pObject->m_ParentType )
				{
				case GMT_BONE:		pObject->m_pParent = NULL;	break;		// ºÎ¸ð°¡ º»ÀÌ¸é m_pParent¸¦ »ç¿ëÇÏÁö ¾ÊÀ½.
				case GMT_NORMAL:	pObject->m_pParent = &pGroup->m_pObject[ nParentIdx ];		break;	// ºÎ¸ð°¡ ÀÏ¹Ý¿ÀºêÁ§ÀÌ¶ó¸é ±× Æ÷ÀÎÅÍ ÁöÁ¤.
					break;
				}
			}

			// ¿øÁ¡±âÁØÀÇ LocalTM.  ºÎ¸ð°¡ ÀÖ´Ù¸é ¿øÁ¡Àº ºÎ¸ð°¡ µÈ´Ù.
			resFp.Read( &pObject->m_mLocalTM, sizeof(D3DXMATRIX), 1 );

			// load geometry
			cout << "gets" << endl;
			LoadGMObject(&resFp, pObject);


						// MeshºÎ ÀÐÀ½

#ifdef __YENV
			// ¹öÅÃ½º ¹öÆÛ Å¸ÀÔÀ» ÀÏ¹ÝÀ¸·Î..
			// ¹üÇÁ¿ë ¹öÆÛ´Â ExtractBuffersÇÔ¼ö È£ÃâÇÔÀ¸·Î½á »çÀÌÁî°¡ ¹Ù²ï´Ù
			if( g_Option.m_bSpecBump )
				pObject->m_VBType = VBT_NORMAL;
#endif //__YENV

			if( pObject->m_Type == GMT_SKIN )
			{
#ifdef __YENV
				if( pObject->m_bBump && g_Option.m_bSpecBump )
				{
	#ifdef __YENV_WITHOUT_BUMP
					pObject->m_nVertexSize = sizeof(SKINVERTEX);
					pObject->m_dwFVF	   = D3DFVF_SKINVERTEX;
	#else //__YENV_WITHOUT_BUMP
					pObject->m_nVertexSize = sizeof(SKINVERTEX_BUMP);
					pObject->m_dwFVF	   = D3DFVF_SKINVERTEX_BUMP;
	#endif //__YENV_WITHOUT_BUMP
				}
				else
#endif //__YENV
				{
					pObject->m_nVertexSize = sizeof(SKINVERTEX);
					pObject->m_dwFVF	   = D3DFVF_SKINVERTEX;
				}
			} else
			{
#ifdef __YENV
				if( pObject->m_bBump && g_Option.m_bSpecBump )
				{
	#ifdef __YENV_WITHOUT_BUMP
					pObject->m_nVertexSize = sizeof(NORMALVERTEX);
					pObject->m_dwFVF	   = D3DFVF_NORMALVERTEX;
	#else //__YENV_WITHOUT_BUMP
					pObject->m_nVertexSize = sizeof(NORMALVERTEX_BUMP);
					pObject->m_dwFVF	   = D3DFVF_NORMALVERTEX_BUMP;
	#endif //__YENV_WITHOUT_BUMP
				}
				else
#endif //__YENV
				{
					pObject->m_nVertexSize = sizeof(NORMALVERTEX);
					pObject->m_dwFVF	   = D3DFVF_NORMALVERTEX;
				}
			}

			if( pObject->m_pPhysiqueVertex == NULL )	bNormalObj = TRUE;
			else										m_nHavePhysique = TRUE;
			if( pObject->m_Type == GMT_NORMAL )
			{
				if( m_nMaxFrame > 0 )	LoadTMAni( &resFp, pObject );
			}

		}
	}

	if( bNormalObj == TRUE )
	{
		D3DXMATRIX *pmPool = new D3DXMATRIX[ nPoolSize ];
		for(int i = 0; i < nMaxGroup; i ++ )
		{
			m_Group[i]._mUpdate = pmPool;
			pmPool += m_Group[i].m_nMaxObject;
			for( j = 0; j < m_Group[i].m_nMaxObject; j ++ )
				D3DXMatrixIdentity( &m_Group[i]._mUpdate[j] );
		}
	}

	if( nVer >= 21 )
	{
		int nAttr = 0;

		resFp.Read( &nAttr, 4, 1 );
		if( nAttr == m_nMaxFrame )
		{

			if(m_nMaxFrame > 0)
				m_pAttr	= new MOTION_ATTR[ m_nMaxFrame ];
			else
				m_pAttr = NULL;

			resFp.Read( m_pAttr, sizeof(MOTION_ATTR) * m_nMaxFrame, 1 );
		}
	}


	resFp.Close();

	return 1;
}




int LoadedObject::LoadGMObject( CResFile *file, GMOBJECT* pObject)
{
	DWORD	dwTemp;
	int		i;

	file->Read( &pObject->m_vBBMin, sizeof(D3DXVECTOR3), 1 );
	cout << "gets" << endl;
	file->Read( &pObject->m_vBBMax, sizeof(D3DXVECTOR3), 1 );

	file->Read( &pObject->m_bOpacity,    4, 1 );
	file->Read( &pObject->m_bBump, 4, 1 );
	file->Read( &pObject->m_bRigid,		4, 1 );
	file->Seek( 28, SEEK_CUR );		// reserved

#ifdef __YENV_WITHOUT_BUMP
	pObject->m_bBump = TRUE;
#endif //__YENV_WITHOUT_BUMP

	// size of list
	file->Read( &pObject->m_nMaxVertexList,	4, 1 );
	file->Read( &pObject->m_nMaxVB,			4, 1 );
	file->Read( &pObject->m_nMaxFaceList,	4, 1 );
	file->Read( &pObject->m_nMaxIB,		4, 1 );
	m_nMaxFace += pObject->m_nMaxFaceList;

	if( pObject->m_Type == GMT_SKIN )
	{
		pObject->m_pVertexList	= new D3DXVECTOR3[ pObject->m_nMaxVertexList ];
		SKINVERTEX *pVB			= new SKINVERTEX[ pObject->m_nMaxVB ];
		pObject->m_pVB = pVB;
	} else
	{
		pObject->m_pVertexList	= new D3DXVECTOR3[ pObject->m_nMaxVertexList ];
		NORMALVERTEX *pVB		= new NORMALVERTEX[ pObject->m_nMaxVB ];

		pObject->m_pVB = pVB;
	}
	pObject->m_pIB  = new WORD[ pObject->m_nMaxIB + pObject->m_nMaxVB ];
	pObject->m_pIIB = pObject->m_pIB + pObject->m_nMaxIB;


	file->Read( pObject->m_pVertexList, sizeof(D3DXVECTOR3) * pObject->m_nMaxVertexList, 1 );

	SKINVERTEX * SpVB	= new SKINVERTEX[ pObject->m_nMaxVB ];
	NORMALVERTEX * NpVB		= new NORMALVERTEX[ pObject->m_nMaxVB ];

	if( pObject->m_Type == GMT_SKIN )
		file->Read( SpVB,  sizeof(SKINVERTEX) * pObject->m_nMaxVB, 1 );
	else
		file->Read( NpVB,  sizeof(NORMALVERTEX) * pObject->m_nMaxVB, 1 );


	file->Read( pObject->m_pIB,  sizeof(WORD) * pObject->m_nMaxIB, 1 );
	file->Read( pObject->m_pIIB, sizeof(WORD) * pObject->m_nMaxVB, 1 );

	if(pObject->m_Type == GMT_SKIN && this->vertexList.size() == 0){
		for(int k = 0; k < pObject->m_nMaxVB; k++){
            this->vertexList.push_back(SpVB[k].position.x);
            this->vertexList.push_back(SpVB[k].position.y);
            this->vertexList.push_back(SpVB[k].position.z);
            pair<float, float> a(SpVB[k].tu, SpVB[k].tv);
            this->uvs.push_back(a);
		}
	}else if(this->vertexList.size() == 0){
        for(int k = 0; k < pObject->m_nMaxVB; k++){
			this->vertexList.push_back(NpVB[k].position.x);
            this->vertexList.push_back(NpVB[k].position.y);
            this->vertexList.push_back(NpVB[k].position.z);
            pair<float, float> a(NpVB[k].tu, NpVB[k].tv);
            this->uvs.push_back(a);
		}
	}

    if(this->indices.size() == 0){
        for(int k = 0; k < pObject->m_nMaxIB; k++){
            this->indices.push_back(pObject->m_pIB[k]);
        }
    }

	file->Read( &dwTemp, 4, 1 );
	if( dwTemp )
	{
		pObject->m_pPhysiqueVertex = new int[ pObject->m_nMaxVertexList ];
		file->Read( pObject->m_pPhysiqueVertex, sizeof(int) * pObject->m_nMaxVertexList, 1 );
	}

	D3DMATERIAL9	mMaterial;
	char			szBitmap[256];
	int				nLen;
	MATERIAL		*mMaterialAry[16];
	int				bIsMaterial;

	file->Read( &bIsMaterial, 4, 1 );
	pObject->m_bMaterial = bIsMaterial;
	if( bIsMaterial )
	{
		memset( mMaterialAry, 0, sizeof(mMaterialAry) );

		file->Read( &pObject->m_nMaxMaterial, 4, 1 );

		if( pObject->m_nMaxMaterial == 0 )	pObject->m_nMaxMaterial = 1;

		for( i = 0; i < pObject->m_nMaxMaterial; i ++ )
		{
			file->Read( &mMaterial, sizeof(D3DMATERIAL9), 1 );
			file->Read( &nLen, 4, 1 );
			if( nLen > sizeof(szBitmap) )
				printf( "CObject3D::LoadGMObject : %s ÅØ½ºÃÄ ÆÄÀÏ¸íÀÌ ³Ê¹«±æ´Ù : ±æÀÌ = %d", m_szFileName, nLen );
			file->Read( szBitmap, nLen, 1 );
			if (i == 0)
                this->texName = szBitmap;
			strlwr( szBitmap );
			pObject->m_MaterialAry[i].m_Material = mMaterial;

			if( strlen(szBitmap)+1 > sizeof(pObject->m_MaterialAry[i].strBitMapFileName) )
				printf( "CObject3D::LoadGeoMesh() : %sÀÇ ±æÀÌ°¡ ³Ê¹« ±æ´Ù", szBitmap );

			strcpy( pObject->m_MaterialAry[i].strBitMapFileName, szBitmap );

		}

	}

	file->Read( &pObject->m_nMaxMtrlBlk, 4, 1 );
	if( pObject->m_nMaxMtrlBlk >= 32 )
	{
		printf( "¸ÅÅÍ¸®¾ó °¹¼ö°¡ 32°³¸¦ ³Ñ¾ú´Ù. %s", m_szFileName );
		return 0;
	}
	if( pObject->m_nMaxMtrlBlk > 0 )
	{
		pObject->m_pMtrlBlk = new MATERIAL_BLOCK[ pObject->m_nMaxMtrlBlk ];		// ¸ÅÅÍ¸®¾ó ºí·°À» ÇÒ´çÇÏ°í ÀÐÀ½
		pObject->m_pMtrlBlkTexture = new LPDIRECT3DTEXTURE9[ pObject->m_nMaxMtrlBlk * 8 ];	// È®ÀåÅØ½ºÃÄ(ÃÖ´ë8°³)¶û °°ÀÌ ¾´´Ù. ¾øÀ¸¸é °Á ³ÎÀÌ´Ù.
		memset( pObject->m_pMtrlBlkTexture, 0, sizeof(LPDIRECT3DTEXTURE9) * (pObject->m_nMaxMtrlBlk * 8) );

		file->Read( pObject->m_pMtrlBlk, sizeof(MATERIAL_BLOCK) * pObject->m_nMaxMtrlBlk, 1 );
#ifndef __WORLDSERVER

#ifdef __YENV
		if( g_Option.m_bSpecBump )
		{
			// ³ë¸»¸Ê
			pObject->m_pNormalTexture = new LPDIRECT3DTEXTURE9[ pObject->m_nMaxMtrlBlk ];
			memset( pObject->m_pNormalTexture, 0, sizeof(LPDIRECT3DTEXTURE9) * (pObject->m_nMaxMtrlBlk) );

			// ³ë½ºÆÑÅ§·¯¸Ê
			pObject->m_pNoSpecTexture = new LPDIRECT3DTEXTURE9[ pObject->m_nMaxMtrlBlk ];
			memset( pObject->m_pNoSpecTexture, 0, sizeof(LPDIRECT3DTEXTURE9) * (pObject->m_nMaxMtrlBlk) );
		}
#endif //__YENV


		for( i = 0; i < pObject->m_nMaxMtrlBlk; i ++ )
		{
			if( bIsMaterial )
			{
				if( mMaterialAry[ pObject->m_pMtrlBlk[i].m_nTextureID ] )
				{
					pObject->m_pMtrlBlkTexture[i] = mMaterialAry[ pObject->m_pMtrlBlk[i].m_nTextureID ]->m_pTexture;

			#ifdef __YENV
				#ifdef __YENV_WITHOUT_BUMP
					if( g_Option.m_bSpecBump )
				#else //__YENV_WITHOUT_BUMP
					if( pObject->m_pMtrlBlk[i].m_dwEffect & XE_BUMP && g_Option.m_bSpecBump )
				#endif //__YENV_WITHOUT_BUMP
					{
						char	szTexture[MAX_PATH], szFileExt[MAX_PATH];
						char*	strFileName = mMaterialAry[ pObject->m_pMtrlBlk[i].m_nTextureID ]->strBitMapFileName;

						::GetFileTitle( strFileName, szTexture );
						lstrcat( szTexture, "-n." );
						::GetFileExt( strFileName, szFileExt );
						lstrcat( szTexture, szFileExt );

						if( IsEmpty(szTexture) == FALSE )
						{
							 CreateNormalMap( pObject->m_Type, m_pd3dDevice, &(pObject->m_pNormalTexture[i]), szTexture );
						}
					}

				#ifdef __YENV_WITHOUT_BUMP
					if( g_Option.m_bSpecBump )
				#else //__YENV_WITHOUT_BUMP
					if( pObject->m_pMtrlBlk[i].m_dwEffect & XE_BUMP && g_Option.m_bSpecBump )
				#endif //__YENV_WITHOUT_BUMP
					{
						char	szTexture[MAX_PATH], szFileExt[MAX_PATH];
						char*	strFileName		= mMaterialAry[ pObject->m_pMtrlBlk[i].m_nTextureID ]->strBitMapFileName;

						::GetFileTitle( strFileName, szTexture );
						lstrcat( szTexture, "-s." );
						::GetFileExt( strFileName, szFileExt );
						lstrcat( szTexture, szFileExt );

						if( IsEmpty(szTexture) == FALSE )
						{
					#ifdef _DEBUG
							if( (_access( MakePath( DIR_MODELTEX, szTexture ), 0 )) != -1 )
								LoadTextureFromRes( m_pd3dDevice, MakePath( DIR_MODELTEX, szTexture ), &(pObject->m_pNoSpecTexture[i] ) );
					#else //_DEBUG
							char szSerchPath[MAX_PATH];
							GetCurrentDirectory( sizeof( szSerchPath ), szSerchPath );
							TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], name[ _MAX_FNAME ], ext[_MAX_EXT];
							_splitpath( szTexture, drive, dir, name, ext );

							TCHAR szFileName[ _MAX_PATH ];
							RESOURCE* lpRes;
							strcpy( szFileName, MakePath( DIR_MODELTEX, szTexture ) );
							strlwr( szFileName );

							if( CResFile::m_mapResource.Lookup( szFileName, (void*&) lpRes ) )
							{
								LoadTextureFromRes( m_pd3dDevice, MakePath( DIR_MODELTEX, szTexture ), &(pObject->m_pNoSpecTexture[i] ) );
							}
					#endif //_DEBUG
						}
					}

			#endif //__YENV
				}


			}
		}
#endif	// __WORLDSERVER
	}
	return 1;
}



int	LoadedObject::LoadTMAni( CResFile *file, GMOBJECT *pObject )
{
	int	bFrame;
	file->Read( &bFrame, 4, 1 );
	if( bFrame == 0 )	return 0;

	pObject->m_pFrame = new TM_ANIMATION[ m_nMaxFrame ];
	file->Read( pObject->m_pFrame, sizeof(TM_ANIMATION) * m_nMaxFrame, 1 );

	return 1;
}

std::vector<float> LoadedObject::getVerticeList(){
    return this->vertexList;
}

std::vector<int> LoadedObject::getIndiceList(){
    return this->indices;
}


std::vector< pair <float, float> > LoadedObject::getUvs(){
    return this->uvs;
}

const char * LoadedObject::getTextureName(){
    return this->texName.c_str();
}


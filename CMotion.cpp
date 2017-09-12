#include "CMotion.h"


CMotion :: CMotion()
{
	Init();
}

CMotion :: ~CMotion()
{
	Destroy();
}

void	CMotion :: Init( void )
{
	m_pMotion = NULL;
	m_pPath = NULL;
	m_pBoneFrame = NULL;
	m_fPerSlerp = 0.5f;
	m_nMaxEvent = 0;
	m_pBoneInfo = NULL;
	memset( m_vEvent, 0, sizeof(m_vEvent) );
	m_pAttr = NULL;
	m_pBoneInfo = NULL;
}

void	CMotion :: Destroy( void )
{
	int		i;

//	SAFE_DELETE_ARRAY( m_pAttr );
//	SAFE_DELETE_ARRAY( m_pMotion );
//	SAFE_DELETE_ARRAY( m_pPath );
	if( m_pBoneFrame )
	{
		for( i = 0; i < m_nMaxBone; i ++ )
			m_pBoneFrame[i].m_pFrame = NULL;
	}
//	SAFE_DELETE_ARRAY( m_pBoneFrame );
//	SAFE_DELETE_ARRAY( m_pBoneInfo );

	Init();
}

//
//
//
int		CMotion :: LoadMotion( LPCTSTR szFileName )
{
	int		nNumBone;
	int		nNumFrame;
//	int		nNumSize;
//	int		i, j;
	int		nVer;

	CResFile resFp;
	BOOL bRet = resFp.Open(szFileName, "rb" );
	if( bRet == FALSE )
	{
		resFp.Close();
		return 0;
	}

	resFp.Read( &nVer, 4, 1 );		// version
	if( nVer != VER_MOTION )
	{
		resFp.Close();
		return 0;
	}

	// IDÀÐ±â¸¦ ³ÖÀ½.
	resFp.Read( &m_nID, 4, 1 );

	if( strlen( szFileName ) > sizeof(m_szName) )
	{
		resFp.Close();
		return 0;
	}

	// ÆÄÀÏ¸í Ä«ÇÇ
	strcpy( m_szName, szFileName );

	resFp.Read( &m_fPerSlerp, sizeof(float), 1 );		//
	resFp.Seek( 32, SEEK_CUR );		// reserved

	resFp.Read( &nNumBone, 4, 1 );			// »À´ë °¹¼ö ÀÐÀ½
	resFp.Read( &nNumFrame, 4, 1 );		// ¾Ö´Ï¸ÞÀÌ¼Ç ÇÁ·¹ÀÓ °³¼ö ÀÐÀ½
	m_nMaxFrame = nNumFrame;
	m_nMaxBone = nNumBone;

	if( nNumFrame <= 0 )
	{
		resFp.Close();
		return 0;
	}

	// path
	int nTemp;
	resFp.Read( &nTemp, 4, 1 );	// pathÁ¤º¸°¡ ÀÖ´Â°¡?
	if( nTemp )
	{
		m_pPath = new D3DXVECTOR3[ nNumFrame ];
		resFp.Read( m_pPath, sizeof(D3DXVECTOR3) * nNumFrame, 1 );		// nNumFrame¸¸Å­ ÇÑ¹æ¿¡ ÀÐ¾î¹ö¸®±â.
	}

	//
	// TM_Animation µ¥ÀÌÅ¸ ÀÐÀ½.
	ReadTM( &resFp, nNumBone, nNumFrame );

	// ÇÁ·¹ÀÓ ¼Ó¼º ÀÐÀ½.
	resFp.Read( m_pAttr, sizeof(MOTION_ATTR) * nNumFrame, 1 );

	resFp.Read( &m_nMaxEvent, 4, 1 );	// ÀÌº¥Æ® ÁÂÇ¥ ÀúÀå
	if( m_nMaxEvent > 0 )
		resFp.Read( m_vEvent, sizeof(D3DXVECTOR3) * m_nMaxEvent, 1 );


	resFp.Close();

//	m_vPivot = m_pMotion[0].m_vPos;			// Bip01ÀÇ POS°ªÀ» ¹ÞÀ½.  °ñ¹ÝÀÇ ±âÁØ ÁÂÇ¥´Ù.

	return 1;
}

//
//  MotionÀÇ coreºÎºÐÀ» ÀÐÀ½. ¿ÜºÎ¿¡¼­ ´Üµ¶ È£Ãâ °¡´É.
//
void	CMotion :: ReadTM( CResFile *file, int nNumBone, int nNumFrame )
{
	int		nNumSize;
	int		nFrame;
	int		i;

	m_nMaxBone = nNumBone;		// LoadMotion()¿¡¼­ ºÒ·ÁÁ³´Ù¸é ÀÌºÎºÐÀº ÇÊ¿ä¾øÀ¸³ª ReadTM¸¸ µû·Î ºÒ¸± ÀÏÀÌ ÀÖÀ¸¸é ÀÌ°Ô ÇÊ¿äÇÏ´Ù.
	m_nMaxFrame = nNumFrame;

	//--- »À´ë±¸¼ºÁ¤º¸ ÀÐÀ½.
	m_pBoneInfo = new BONE[ nNumBone ];			// º» °³¼ö ¸¸Å­ ÇÒ´ç
	memset( m_pBoneInfo, 0, sizeof(BONE) * nNumBone );		// zero clear

	int		nLen;
	for( i = 0; i < nNumBone; i ++ )
	{
		file->Read( &nLen, 4, 1 );
		if( nLen > 32 )

		file->Read( m_pBoneInfo[i].m_szName,	nLen, 1 );		// bone node ÀÌ¸§
		file->Read( &m_pBoneInfo[i].m_mInverseTM,	sizeof(D3DXMATRIX), 1 );			// Inv NODE TM
		file->Read( &m_pBoneInfo[i].m_mLocalTM,		sizeof(D3DXMATRIX), 1 );			// LocalTM
		file->Read( &m_pBoneInfo[i].m_nParentIdx,	4, 1 );								// parent bone index
	}

	// ºÎ¸ð Æ÷ÀÎÅÍ¸¦ ¼ÂÆÃ
	for( i = 0; i < nNumBone; i ++ )
	{
		if( m_pBoneInfo[i].m_nParentIdx == -1 )			// ºÎ¸ð°¡ ¾øÀ¸¸é ºÎ¸ðÆ÷ÀÎÅÍ´Â ³Î
			m_pBoneInfo[i].m_pParent = NULL;
		else
			m_pBoneInfo[i].m_pParent = &m_pBoneInfo[ m_pBoneInfo[i].m_nParentIdx ];
	}


	file->Read( &nNumSize, 4, 1 );			// ÇÁ·¹ÀÓ »çÀÌÁî ÀÐÀ½ - ¸Þ¸ð¸® Ç® »çÀÌÁî
	//--- ¸ð¼Ç ÀÐÀ½.
	m_pMotion		= new TM_ANIMATION[ nNumSize ];		// ¸Þ¸ð¸® Ç®
	m_pBoneFrame	= new BONE_FRAME[ nNumBone ];
	m_pAttr			= new MOTION_ATTR[ nNumFrame ];
	memset( m_pAttr, 0, sizeof(MOTION_ATTR) * nNumFrame );	// nNumSize¿´´Âµ¥ nNumFrameÀÌ ¸Â´Â°Å °°´Ù.
	TM_ANIMATION	*p = m_pMotion;
	int		nCnt = 0;

	// »À´ë ¼ö ¸¸Å­ ·çÇÁ
	for( i = 0; i < nNumBone; i ++ )
	{
		file->Read( &nFrame, 4, 1 );
		if( nFrame == 1 )		// 1ÀÌ¸é ÇöÀç »À´ë¿¡ ÇÁ·¹ÀÓ ÀÖÀ½
		{
			m_pBoneFrame[i].m_pFrame = p;
			file->Read( m_pBoneFrame[i].m_pFrame, sizeof(TM_ANIMATION) * nNumFrame, 1 );		// ÇÑ¹æ¿¡ ÀÐ¾î¹ö¸®±â.
			p += nNumFrame;
			nCnt += nNumFrame;
//			for( j = 0; j < nNumFrame; j ++ )
//			{
//				file->Read( &(p->m_mAniTM), sizeof(D3DXMATRIX), 1 );
//				file->Read( &(p->m_qRot), sizeof(D3DXQUATERNION), 1 );
//				file->Read( &(p->m_vPos), sizeof(D3DXVECTOR3), 1 );
//				p ++;
//				nCnt ++;
//			}
		} else			// ÇöÀç »À´ë¿¡ ÇÁ·¹ÀÓ ¾øÀ½
		{
			file->Read( &(m_pBoneFrame[i].m_mLocalTM), sizeof(D3DXMATRIX), 1 );			// ÇÁ·¹ÀÓÀÌ ¾øÀ¸¸é LocalTM¸¸ ÀÐ°í
			m_pBoneFrame[i].m_pFrame = NULL;
			// m_mLocalTM¿¡ ³Ö¾úÀ¸¹Ç·Î ¸Þ¸ð¸® Ç®¿¡´Â ³ÖÀ»ÇÊ¿ä ¾ø´Ù.
		}
	}

	if( nCnt != nNumSize )
	{
		printf( "%s : frame size error", m_szName );
	}
}

//
// »À´ëÀÇ ¾Ö´Ï¸ÞÀÌ¼Ç
//
void	CMotion :: AnimateBone( D3DXMATRIX *pmUpdateBone, CMotion *pMotionOld, float fFrameCurrent, int nNextFrame, int nFrameOld, BOOL bMotionTrans, float fBlendWeight )
{
	int		i;
	BONE	*pBone = m_pBoneInfo;		// ÀÌ ¸ðµ¨ÀÇ »À´ë ½ÃÀÛ Æ÷ÀÎÅÍ.
	TM_ANIMATION	*pFrame = NULL;					// ÇöÀç ÇÁ·¹ÀÓ
	TM_ANIMATION	*pNext = NULL;					// ´ÙÀ½ ÇÁ·¹ÀÓ
	TM_ANIMATION	*pFrameOld = NULL;				// ÀÌÀü¸ð¼ÇÀÇ ÇÁ·¹ÀÓ
//	D3DXMATRIX	*pMatBone = m_mUpdateBone;
	D3DXMATRIX	*pMatBones = pmUpdateBone;			// ¾÷µ¥ÀÌÆ® ¸ÅÆ®¸¯½º ¸®½ºÆ® ½ÃÀÛ Æ÷ÀÎÅÍ
	int			nCurrFrame;
	float		fSlp;
	D3DXQUATERNION		qSlerp;
	D3DXVECTOR3			vSlerp;
	BONE_FRAME	*pBoneFrame = NULL;
	BONE_FRAME	*pBoneFrameOld = NULL;		// º»ÀÇ °èÃþ±¸Á¶
	D3DXMATRIX	m1, m2;

	if( pMotionOld == NULL )	// pMotionOld-> ÇÒ¶§ ³ÎÂüÁ¶ ¿¡·¯¸¸ ¾È³ª°ÔÇÔ.
		pMotionOld = this;

	// ¾Ö´Ï¸ÞÀÌ¼Ç µ¿ÀÛÀÌ ÀÖ´Â °æ¿ì
	i = m_nMaxBone;
	// º¸°£À» À§ÇÑ Slerp °è»ê.
	nCurrFrame = (int)fFrameCurrent;				// ¼Ò¼ýÁ¡ ¶¼³»°í Á¤¼öºÎ¸¸..
	fSlp = fFrameCurrent - (float)nCurrFrame;	// ¼Ò¼ýÁ¡ ºÎºÐ¸¸ °¡Á®¿È
	/////////////////////////////

//	int	nNextFrame = GetNextFrame();
	pBoneFrame	  = m_pBoneFrame;
	pBoneFrameOld = pMotionOld->m_pBoneFrame;		// ÀÌÀüµ¿ÀÛÀÇ »À´ë °èÃþ
	// »À´ë °¹¼ö¸¸Å­ ·çÇÁ µ·´Ù
	while( i-- )
	{
		if( pBoneFrameOld->m_pFrame )
			pFrameOld = &pBoneFrameOld->m_pFrame[nFrameOld];		// ÀÏ´ÜÀº 0À¸·Î ÇÑ´Ù.  ÀÌÀü¸ð¼ÇÀÇ nCurrFrameµµ ±â¾ïÇÏ°í ÀÖ¾î¾ß ÇÑ´Ù.
		else
			pFrameOld = NULL;		// ÀÏ´ÜÀº 0À¸·Î ÇÑ´Ù.  ÀÌÀü¸ð¼ÇÀÇ nCurrFrameµµ ±â¾ïÇÏ°í ÀÖ¾î¾ß ÇÑ´Ù.
		if( pBoneFrame->m_pFrame )		// ÀÌ »À´ë¿¡ ÇÁ·¹ÀÓÀÌ ÀÖÀ»¶§
		{
			pFrame = &pBoneFrame->m_pFrame[ nCurrFrame ];		// ÇöÀç ÇÁ·¹ÀÓ Æ÷ÀÎÅÍ ¹ÞÀ½
			pNext  = &pBoneFrame->m_pFrame[ nNextFrame ];		// ´ÙÀ½ ÇÁ·¹ÀÓ Æ÷ÀÎÅÍ ¹ÞÀ½

			D3DXQuaternionSlerp( &qSlerp, &pFrame->m_qRot, &pNext->m_qRot, fSlp );	// º¸°£µÈ ÄõÅÍ´Ï¾ð ±¸ÇÔ
			D3DXVec3Lerp( &vSlerp, &pFrame->m_vPos, &pNext->m_vPos, fSlp );		// º¸°£µÈ Posº¤ÅÍ ±¸ÇÔ

			if( bMotionTrans )	// ÀÌÀü ¸ð¼Ç°ú ºí·»µù ÇØ¾ß ÇÏ´Â°¡
			{
				float	fBlendSlp = fBlendWeight;
				if( pFrameOld )
				{
					D3DXQuaternionSlerp( &qSlerp, &pFrameOld->m_qRot, &qSlerp, fBlendSlp );	// ÀÌÀü¸ð¼Ç°ú ÇöÀç ÇÁ·¹ÀÓÀ» ºí·»µù
					D3DXVec3Lerp( &vSlerp, &pFrameOld->m_vPos, &vSlerp, fBlendSlp );		// º¸°£µÈ Posº¤ÅÍ ±¸ÇÔ
				} else
				{
					D3DXQUATERNION	qLocal;
					D3DXVECTOR3		vLocal;
					D3DXMATRIX		*pmLocalTM = &pBoneFrameOld->m_mLocalTM;
					D3DXQuaternionRotationMatrix( &qLocal, pmLocalTM );
					vLocal.x = pmLocalTM->_41;
					vLocal.y = pmLocalTM->_42;
					vLocal.z = pmLocalTM->_43;
					D3DXQuaternionSlerp( &qSlerp, &qLocal, &qSlerp, fBlendSlp );	// ÀÌÀü¸ð¼Ç°ú ÇöÀç ÇÁ·¹ÀÓÀ» ºí·»µù
					D3DXVec3Lerp( &vSlerp, &vLocal, &vSlerp, fBlendSlp );		// º¸°£µÈ Posº¤ÅÍ ±¸ÇÔ
				}
			}

			// matAniTM°è»ê
			// ÀÌµ¿Çà·Ä ¸¸µê
			D3DXMatrixTranslation( &m1,	 vSlerp.x,  vSlerp.y,  vSlerp.z );

			// ÄõÅÍ´Ï¿Â º¸°£µÈ È¸ÀüÅ°°ªÀ» È¸ÀüÇà·Ä·Î ¸¸µê
			D3DXMatrixRotationQuaternion( &m2, &qSlerp );
			m2 *= m1;		// ÀÌµ¿Çà·Ä X È¸ÀüÇà·Ä = ¾Ö´ÏÇà·Ä

			if( pBone->m_pParent )
				m2 *= pMatBones[ pBone->m_nParentIdx ];		// ºÎ¸ð°¡ ¾ø´Ù¸é ·çÆ®ÀÎµ¥ Ç×µîÇà·Ä°ú °öÇÒ ÇÊ¿ä ¾ø¾î¼­ »ý·«Çß´Ù.
//				else
//					m2 = m2 * *pmWorld;  // ÀÌºÎºÐÀÌ »ý·«µÆ´Ù.  mCenter´Â Ç×µîÇà·Ä.

			// WorldTM = LocalTM * ParentTM;
			// ÃÖÁ¾ º¯È¯µÈ ¿ùµå¸ÅÆ®¸¯½º¸¦ ¸â¹ö¿¡ ³Ö¾î¼­ Render()¿¡¼­ SetTransfromÇÔ.
		} else
		// ÀÌ »À´ë¿¡ ÇÁ·¹ÀÓÀÌ ¾øÀ»¶§.
		{
			if( bMotionTrans )	// ÀÌÀü ¸ð¼Ç°ú ºí·»µù ÇØ¾ß ÇÏ´Â°¡
			{
				float	fBlendSlp = fBlendWeight;
				D3DXQUATERNION	qLocal;
				D3DXVECTOR3		vLocal;
				D3DXMATRIX		*pmLocalTM = &pBoneFrame->m_mLocalTM;
				D3DXQuaternionRotationMatrix( &qLocal, pmLocalTM );
				vLocal.x = pmLocalTM->_41;
				vLocal.y = pmLocalTM->_42;
				vLocal.z = pmLocalTM->_43;	// ÇöÀçÇÁ·¹ÀÓÀÇ LocalTMÀ» ÄõÅÍ´Ï¿Â°ú º¤ÅÍ·Î »©³¿.

				if( pFrameOld )
				{
					D3DXQuaternionSlerp( &qSlerp, &pFrameOld->m_qRot, &qLocal, fBlendSlp );	// ÀÌÀü¸ð¼Ç°ú ÇöÀç ÇÁ·¹ÀÓÀ» ºí·»µù
					D3DXVec3Lerp( &vSlerp, &pFrameOld->m_vPos, &vLocal, fBlendSlp );		// º¸°£µÈ Posº¤ÅÍ ±¸ÇÔ
				} else
				{
					D3DXQUATERNION	qLocal2;
					D3DXVECTOR3		vLocal2;
					D3DXMATRIX		*pmLocalTM2 = &pBoneFrameOld->m_mLocalTM;
					D3DXQuaternionRotationMatrix( &qLocal2, pmLocalTM2 );
					vLocal2.x = pmLocalTM2->_41;
					vLocal2.y = pmLocalTM2->_42;
					vLocal2.z = pmLocalTM2->_43;
					D3DXQuaternionSlerp( &qSlerp, &qLocal2, &qLocal, fBlendSlp );	// ÀÌÀü¸ð¼Ç°ú ÇöÀç ÇÁ·¹ÀÓÀ» ºí·»µù
					D3DXVec3Lerp( &vSlerp, &vLocal2, &vLocal, fBlendSlp );		// º¸°£µÈ Posº¤ÅÍ ±¸ÇÔ
				}
				D3DXMatrixTranslation( &m1,	 vSlerp.x,  vSlerp.y,  vSlerp.z );

				// ÄõÅÍ´Ï¿Â º¸°£µÈ È¸ÀüÅ°°ªÀ» È¸ÀüÇà·Ä·Î ¸¸µê
				D3DXMatrixRotationQuaternion( &m2, &qSlerp );
				m2 *= m1;			// ÀÌµ¿Çà·Ä X È¸ÀüÇà·Ä = ¾Ö´ÏÇà·Ä
			} else
				m2 = pBoneFrame->m_mLocalTM;

			if( pBone->m_pParent )
				m2 *= pMatBones[ pBone->m_nParentIdx ];
//				else
//					m2 = m2 * *pmWorld;
		}

		*pmUpdateBone = m2;		// ÀÌ »À´ëÀÇ ÃÖÁ¾ ¿ùµå º¯È¯ ¸ÅÆ®¸¯½º

		pBoneFrame ++;
		pBoneFrameOld ++;
		pBone ++;					// º» ++
		pmUpdateBone ++;			// º» Æ®·£½ºÆû ¸ÅÆ®¸¯½º ++
	}
}

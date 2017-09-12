#include "Utils.h"

class CTextureManager
{
protected:
//	LPDIRECT3DDEVICE9 m_pd3dDevice;        // The D3D rendering device
	int		m_nMaxTexture;

public:
	MATERIAL	m_pMaterial[ MAX_MATERIAL ];		// ���ӿ��� ����ϴ� ��� ��Ʈ������ ����.
	CTextureManager();
	~CTextureManager();

	HRESULT DeleteDeviceObjects();

	int		DeleteMaterial( LPDIRECT3DTEXTURE9 pTexture );	// pTexture�� ����ϴ� ���͸����� ã�� �����Ѵ�.
//	void	SetD3DDevice( LPDIRECT3DDEVICE9 pd3dDevice ) { m_pd3dDevice = pd3dDevice;  }

	D3DMATERIAL9			*GetMaterial( LPDIRECT3DDEVICE9 pd3dDevice, int nIdx );
	LPDIRECT3DTEXTURE9		GetTexture( LPDIRECT3DDEVICE9 pd3dDevice, int nIdx );

	//D3DMATERIAL9			*GetMaterial( int nIdx ) { return &m_pMaterial[ nIdx ].m_Material; }
	//LPDIRECT3DTEXTURE9		GetTexture( int nIdx ) { return m_pMaterial[ nIdx ].m_pTexture; }

	MATERIAL	*AddMaterial( LPDIRECT3DDEVICE9 m_pd3dDevice, D3DMATERIAL9 *pMaterial, LPCTSTR strFileName, LPCTSTR szPath = NULL );		// ���͸��� �ϳ��� �߰��ϰ� �� �����͸� �ǵ�����.
};

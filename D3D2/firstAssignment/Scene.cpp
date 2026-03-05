//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Scene.h"

CScene::CScene()
{
}

CScene::~CScene()
{
}

void CScene::BuildDefaultLightsAndMaterials()
{
	m_nLights = 4;
	m_pLights = new LIGHT[m_nLights];
	::ZeroMemory(m_pLights, sizeof(LIGHT) * m_nLights);

	m_xmf4GlobalAmbient = XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f);

	m_pLights[0].m_bEnable = true;
	m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights[0].m_fRange = 1000.0f;
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.0f);
	m_pLights[0].m_xmf3Position = XMFLOAT3(30.0f, 30.0f, 30.0f);
	m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);
	m_pLights[1].m_bEnable = true;
	m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights[1].m_fRange = 500.0f;
	m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	m_pLights[1].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[1].m_fFalloff = 8.0f;
	m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));
	m_pLights[2].m_bEnable = true;
	m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_pLights[2].m_xmf4Specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 0.0f);
	m_pLights[2].m_xmf3Direction = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_pLights[3].m_bEnable = true;
	m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights[3].m_fRange = 600.0f;
	m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.7f, 0.0f, 1.0f);
	m_pLights[3].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[3].m_xmf3Position = XMFLOAT3(50.0f, 30.0f, 30.0f);
	m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[3].m_fFalloff = 8.0f;
	m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));
}

void CScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{

	// Initialize SoundPlayer
	sound[0].Initialize();
	sound[0].LoadWave(inGame, 0);


	sound[1].Initialize();
	sound[1].LoadWave(opening, 0);

	sound[3].Initialize();
	sound[3].LoadWave(close, 0);

	sound[4].Initialize();
	sound[4].LoadWave(win, 0);


	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	BuildDefaultLightsAndMaterials();

	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	XMFLOAT3 xmf3Scale(18.0f, 6.0f, 18.0f);
	//XMFLOAT4 xmf4Color(0.0f, 0.5f, 0.0f, 0.0f);
	//XMFLOAT3 xmf3Scale(16.0f, 3.0f, 16.0f);
	XMFLOAT4 xmf4Color(0.0f, 0.5f, 0.0f, 0.0f);
	m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, _T("Image/HeightMap.raw"), 257, 257, 13, 13, xmf3Scale, xmf4Color);

	m_pTerrainWater = new CRippleWater(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, 257, 257, 13, 13, xmf3Scale, xmf4Color);
	m_pTerrainWater->SetPosition(+(257 * 0.5f), /*225*/ /*607*/ m_pTerrain->GetHeight(257 * 0.5f, 257 * 0.5f) - 200, +(257 * 0.5f));

	//m_pTerrainWater->SetPosition(+(257 * 0.5f), 155.0f, +(257 * 0.5f));

	m_nShaders = 3;

	m_ppShaders = new CShader * [m_nShaders];

	pObjectsShader = new CObjectsShader();
	pObjectsShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pObjectsShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, NULL);

	m_ppShaders[0] = pObjectsShader;


	pMultiSpriteObjectShader = new CMultiSpriteObjectsShader();
	pMultiSpriteObjectShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pMultiSpriteObjectShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);
	pMultiSpriteObjectShader->SetActive(false);
	m_ppShaders[1] = pMultiSpriteObjectShader;

	pMultiSpriteObjectShader->m_ppObjects[6]->m_ppMaterials[0]->m_pTexture->m_bActive = true;


	CBillboardObjectsShader* pBillboardObjectShader = new CBillboardObjectsShader();
	pBillboardObjectShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pBillboardObjectShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);
	m_ppShaders[2] = pBillboardObjectShader;

	m_pShadowMapToViewport = new CTextureToViewportShader * [2];
	m_pShadowMapToViewport[0] = new CTextureToViewportShader();
	m_pShadowMapToViewport[0]->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	m_pShadowMapToViewport[1] = new CTextureToViewportShader();
	m_pShadowMapToViewport[1]->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_pShadowMapToViewport[1]->color = 1;



	//---------------------------------------------------------------------

	m_nEnvironmentMappingShaders = 1;
	m_ppEnvironmentMappingShaders = new CDynamicCubeMappingShader * [m_nEnvironmentMappingShaders];

	m_ppEnvironmentMappingShaders[0] = new CDynamicCubeMappingShader(256);
	m_ppEnvironmentMappingShaders[0]->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	m_ppEnvironmentMappingShaders[0]->BuildObjects(pd3dDevice, pd3dCommandList, m_pTerrain);




	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	ReleaseShaderVariables();

	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}

	if (m_pShadowMapToViewport) {
		for (int i{}; i < 2; ++i) {
			m_pShadowMapToViewport[i]->ReleaseShaderVariables();
			m_pShadowMapToViewport[i]->ReleaseObjects();
			m_pShadowMapToViewport[i]->Release();
		}
		delete[]m_pShadowMapToViewport;
	}


	if (m_ppEnvironmentMappingShaders)
	{
		for (int i = 0; i < m_nEnvironmentMappingShaders; i++)
		{
			m_ppEnvironmentMappingShaders[i]->ReleaseShaderVariables();
			m_ppEnvironmentMappingShaders[i]->ReleaseObjects();
			m_ppEnvironmentMappingShaders[i]->Release();
		}
		delete[] m_ppEnvironmentMappingShaders;
	}

	if (m_pTerrain) delete m_pTerrain;
	if (m_pSkyBox) delete m_pSkyBox;
	if (m_pTerrainWater) delete m_pTerrainWater;

	if (m_ppGameObjects)
	{
		for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Release();
		delete[] m_ppGameObjects;
	}

	if (m_pLights) delete[] m_pLights;
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[12];


	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 6; //t6: gtxtAlbedoTexture
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 7; //t7: gtxtSpecularTexture
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[2].NumDescriptors = 1;
	pd3dDescriptorRanges[2].BaseShaderRegister = 8; //t8: gtxtNormalTexture
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[3].NumDescriptors = 1;
	pd3dDescriptorRanges[3].BaseShaderRegister = 9; //t9: gtxtMetallicTexture
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[4].NumDescriptors = 1;
	pd3dDescriptorRanges[4].BaseShaderRegister = 10; //t10: gtxtEmissionTexture
	pd3dDescriptorRanges[4].RegisterSpace = 0;
	pd3dDescriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[5].NumDescriptors = 1;
	pd3dDescriptorRanges[5].BaseShaderRegister = 11; //t11: gtxtDetailAlbedoTexture
	pd3dDescriptorRanges[5].RegisterSpace = 0;
	pd3dDescriptorRanges[5].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[6].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[6].NumDescriptors = 1;
	pd3dDescriptorRanges[6].BaseShaderRegister = 12; //t12: gtxtDetailNormalTexture
	pd3dDescriptorRanges[6].RegisterSpace = 0;
	pd3dDescriptorRanges[6].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[7].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[7].NumDescriptors = 1;
	pd3dDescriptorRanges[7].BaseShaderRegister = 13; //t13: gtxtSkyBoxTexture
	pd3dDescriptorRanges[7].RegisterSpace = 0;
	pd3dDescriptorRanges[7].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	pd3dDescriptorRanges[8].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[8].NumDescriptors = 3;
	pd3dDescriptorRanges[8].BaseShaderRegister = 14; //t14~16: gtxtTerrainTexture
	pd3dDescriptorRanges[8].RegisterSpace = 0;
	pd3dDescriptorRanges[8].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	pd3dDescriptorRanges[9].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[9].NumDescriptors = 1;
	pd3dDescriptorRanges[9].BaseShaderRegister = 0; //t0: gtxtTexture
	pd3dDescriptorRanges[9].RegisterSpace = 0;
	pd3dDescriptorRanges[9].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[10].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[10].NumDescriptors = 3;
	pd3dDescriptorRanges[10].BaseShaderRegister = 3; //t3: gtxtWaterBaseTexture, t4: gtxtWaterDetailTexture, t5: gtxtWaterDetailAlphaTexture
	pd3dDescriptorRanges[10].RegisterSpace = 0;
	pd3dDescriptorRanges[10].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[11].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[11].NumDescriptors = 1;
	pd3dDescriptorRanges[11].BaseShaderRegister = 1; //t1: gtxtTextureCube
	pd3dDescriptorRanges[11].RegisterSpace = 0;
	pd3dDescriptorRanges[11].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER pd3dRootParameters[15];


	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 30;
	pd3dRootParameters[1].Constants.ShaderRegister = 2; //GameObject
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[3].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[0]);
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[1]);
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[5].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[2]);
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[6].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[3]);
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[7].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[7].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[4]);
	pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[8].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[8].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[5]);
	pd3dRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[9].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[9].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[6]);
	pd3dRootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[10].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[10].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[7]);
	pd3dRootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[11].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[11].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[8]);
	pd3dRootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


	pd3dRootParameters[12].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[12].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[12].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[9];
	pd3dRootParameters[12].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[13].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[13].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[13].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[10];
	pd3dRootParameters[13].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[14].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[14].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[14].DescriptorTable.pDescriptorRanges = &pd3dDescriptorRanges[11]; //t3: gtxtTextureCube
	pd3dRootParameters[14].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC pd3dSamplerDescs[2];

	pd3dSamplerDescs[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].MipLODBias = 0;
	pd3dSamplerDescs[0].MaxAnisotropy = 1;
	pd3dSamplerDescs[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[0].MinLOD = 0;
	pd3dSamplerDescs[0].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[0].ShaderRegister = 0;
	pd3dSamplerDescs[0].RegisterSpace = 0;
	pd3dSamplerDescs[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dSamplerDescs[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].MipLODBias = 0;
	pd3dSamplerDescs[1].MaxAnisotropy = 1;
	pd3dSamplerDescs[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[1].MinLOD = 0;
	pd3dSamplerDescs[1].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[1].ShaderRegister = 1;
	pd3dSamplerDescs[1].RegisterSpace = 0;
	pd3dSamplerDescs[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags
		= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = _countof(pd3dSamplerDescs);
	d3dRootSignatureDesc.pStaticSamplers = pd3dSamplerDescs;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature),
		(void**)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256�� ���
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void**)&m_pcbMappedLights);
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList)
{
	::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
	::memcpy(&m_pcbMappedLights->renderMode, x, sizeof(UINT));
}

void CScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}

	if (m_pTerrain) m_pTerrain->ReleaseShaderVariables();
	if (m_pSkyBox) m_pSkyBox->ReleaseShaderVariables();
}

void CScene::ReleaseUploadBuffers()
{
	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();
	if (m_pSkyBox) m_pSkyBox->ReleaseUploadBuffers();
	if (m_pTerrainWater) m_pTerrainWater->ReleaseUploadBuffers();

	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();
	for (int i = 0; i < m_nGameObjects; i++) m_ppGameObjects[i]->ReleaseUploadBuffers();
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_UP:

			for (int i{}; i < pObjectsShader->m_nObjects; ++i)
				pObjectsShader->m_ppObjects[i]->MoveForward(+1.0f);
			break;
		case VK_DOWN:

			for (int i{}; i < pObjectsShader->m_nObjects; ++i)
				pObjectsShader->m_ppObjects[i]->MoveForward(-1.0f);
			break;
		case VK_LEFT:

			for (int i{}; i < pObjectsShader->m_nObjects; ++i)
				pObjectsShader->m_ppObjects[i]->MoveStrafe(-1.0f);
			break;
		case VK_RIGHT:

			for (int i{}; i < pObjectsShader->m_nObjects; ++i)
				pObjectsShader->m_ppObjects[i]->MoveStrafe(+1.0f);
			break;
		case VK_RETURN:

			for (int i{}; i < pObjectsShader->m_nObjects; ++i)
				pObjectsShader->m_ppObjects[i]->MoveUp(+1.0f);
			break;
		case 0x10://SHIFT

			for (int i{}; i < pObjectsShader->m_nObjects; ++i)
				pObjectsShader->m_ppObjects[i]->MoveUp(-1.0f);
			break;



		default:
			break;
		}
		break;
	default:
		break;
	}
	return(false);
}

bool CScene::ProcessInput(UCHAR* pKeysBuffer)
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed)
{

	for (int i{}; i < m_nGameObjects; ++i) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Animate(fTimeElapsed, NULL);
	for (int i{}; i < m_nGameObjects; ++i) if (m_ppGameObjects[i]) m_ppGameObjects[i]->UpdateTransform(NULL);



	m_ppShaders[0]->AnimateObjects(fTimeElapsed);
	m_ppShaders[2]->AnimateObjects(fTimeElapsed);

	if (pMultiSpriteObjectShader->m_ppObjects[0]->m_ppMaterials[0]->m_pTexture->m_bActive) {
		pMultiSpriteObjectShader->AnimateObjects(fTimeElapsed);
	}

	if (m_pLights)
	{
		m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}



	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dist(1, 6);

	if (!pObjectsShader->obj.empty()) {
		for (const auto& o : pObjectsShader->obj)
		{
			if (1 == dist(gen))
				o->MoveForward(+1.0f);//forward
			else if (2 == dist(gen))
				o->MoveForward(-1.0f);//back
			else if (3 == dist(gen))
				o->MoveStrafe(-1.0f);//left
			else if (4 == dist(gen))
				o->MoveStrafe(+1.0f);//right
			else if (5 == dist(gen))
				o->MoveUp(+1.0f);//up
			else if (6 == dist(gen))
				o->MoveUp(-1.0f);//down
		}
	}

	if (m_pPlayer->attack)
	{
		XMFLOAT3 Cur_LookVector = m_pPlayer->GetLookVector();
		XMFLOAT3 Cur_Pos = m_pPlayer->GetPosition();

		XMVECTOR Bullet_Origin = XMLoadFloat3(&Cur_Pos);
		XMVECTOR Bullet_Direction = XMLoadFloat3(&Cur_LookVector);

		if (!pObjectsShader->obj.empty() && start) {
			for (int i{}; i < pObjectsShader->obj.size(); ++i)
			{
				float bullet_monster_distance = Vector3::Length(Vector3::Subtract(pObjectsShader->obj[i]->aabb.Center, Cur_Pos));

				if (pObjectsShader->obj[i]->aabb.Intersects(Bullet_Origin, Bullet_Direction, bullet_monster_distance))
				{

					cout << i << "����" << endl;
					pMultiSpriteObjectShader->hit = pObjectsShader->obj[i]->GetPosition();
					pObjectsShader->obj.erase(pObjectsShader->obj.begin() + i);
					cout << "obj.size : " << pObjectsShader->obj.size() << endl;
					cout << "obj.i : " << i << endl;
					m_pPlayer->attack = false;
					++crashCnt;
					if (!sound[5].attOnce ) {
						sound[5].Initialize();
						sound[5].LoadWave(monster, 0);
						sound[5].Play();

						sound[5].attOnce = true;
					}
					

					if (crashCnt == pObjectsShader->m_nObjects) {
						sound[0].Stop();//??????
						sound[2].Stop();
						sound[5].Stop();
						sound[4].Play();//?????

						pMultiSpriteObjectShader->m_ppObjects[7]->m_ppMaterials[0]->m_pTexture->m_bActive = true;
						pMultiSpriteObjectShader->m_ppObjects[9]->m_ppMaterials[0]->m_pTexture->m_bActive = false;
						start = false;
					}
				}
			}
		}
	}

	for (int i{}; i < m_nEnvironmentMappingShaders; ++i)
		m_ppEnvironmentMappingShaders[i]->AnimateObjects(fTimeElapsed);

}



void CScene::OnPreRender(ID3D12Device* pd3dDevice, ID3D12CommandQueue* pd3dCommandQueue, ID3D12Fence* pd3dFence, HANDLE hFenceEvent)
{
	for (int i{}; i < m_nEnvironmentMappingShaders; ++i)
	{
		m_ppEnvironmentMappingShaders[i]->OnPreRender(pd3dDevice, pd3dCommandQueue, pd3dFence, hFenceEvent, this);
	}
}

void CScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	// Play sound
	sound[1].Play();//??????

	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights



	if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);
	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);


	pd3dCommandList->SetGraphicsRoot32BitConstants(1, 1, cuT, 28);

	m_pTerrainWater->m_nMaterials = 1;
	if (m_pTerrainWater)
		m_pTerrainWater->Render(pd3dCommandList, pCamera);


	m_ppShaders[2]->Render(pd3dCommandList, pCamera);//Ǯ

	pObjectsShader->Render(pd3dCommandList, pCamera);//���

	for (int i{}; i < m_nEnvironmentMappingShaders; ++i)
		m_ppEnvironmentMappingShaders[i]->Render(pd3dCommandList, pCamera);


	if (pCamera->GetPlayer() && start) {
		m_pShadowMapToViewport[1]->Render(pd3dCommandList, pCamera, 5400 / 25.f, XMFLOAT2(30, 21));
		m_pShadowMapToViewport[0]->Render(pd3dCommandList, pCamera, m_pPlayer->HP / 25.f, XMFLOAT2(38, 27));
		pMultiSpriteObjectShader->m_ppObjects[9]->m_ppMaterials[0]->m_pTexture->m_bActive = true;
	}

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);

	if (start) {

		if (0 < m_pPlayer->HP) {
			//m_pPlayer->HP -= 1.5314f;
			m_pPlayer->HP -= 0.09259f;
			//m_pPlayer->HP -= 0.00009259f;
			m_pPlayer->Render(pd3dCommandList, pCamera);
		}
		else {
			pMultiSpriteObjectShader->m_ppObjects[8]->m_ppMaterials[0]->m_pTexture->m_bActive = true;
			pMultiSpriteObjectShader->m_ppObjects[9]->m_ppMaterials[0]->m_pTexture->m_bActive = false;
			start = false;
			sound[0].Stop();//??????
			sound[3].Play();//?????
		}

	}

	pMultiSpriteObjectShader->Render(pd3dCommandList, pCamera);//�Ҳ�
}

SoundPlayer::SoundPlayer()
	: xAudio2_(nullptr), masterVoice_(nullptr), sourceVoice_(nullptr)
{
	ZeroMemory(&waveFormat_, sizeof(waveFormat_));
	ZeroMemory(&buffer_, sizeof(buffer_));
}

SoundPlayer::~SoundPlayer()
{
	Terminate();
}

bool SoundPlayer::Initialize()
{
	HRESULT hr;


	// XAudio2 ��ü ����
	hr = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(hr)) {
		return false;
	}



	// ������ ���̽� ����
	hr = xAudio2_->CreateMasteringVoice(&masterVoice_);
	if (FAILED(hr)) {
		return false;
	}


	waveFormat_.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat_.nChannels = 2;
	waveFormat_.nSamplesPerSec = 48000;
	waveFormat_.nAvgBytesPerSec = 48000 * 4;
	waveFormat_.nBlockAlign = 4;
	waveFormat_.wBitsPerSample = 16;
	waveFormat_.cbSize = 0;


	// �ҽ� ���̽� ����
	hr = xAudio2_->CreateSourceVoice(&sourceVoice_, &waveFormat_);
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

void SoundPlayer::Terminate()
{
	if (sourceVoice_ != nullptr) {
		sourceVoice_->DestroyVoice();
		sourceVoice_ = nullptr;
	}


	if (masterVoice_ != nullptr) {
		masterVoice_->DestroyVoice();
		masterVoice_ = nullptr;
	}

	if (xAudio2_ != nullptr) {
		xAudio2_->Release();
		xAudio2_ = nullptr;
	}
}

HRESULT SoundPlayer::LoadWaveFile(const wchar_t* filename)
{
	// WAV ������ �б� �������� ����
	FILE* file = nullptr;
	errno_t err = _wfopen_s(&file, filename, L"rb");
	if (err != 0)
	{
		return HRESULT_FROM_WIN32(err);
	}

	// WAV ���� ��� �б�
	WAVEHEADER header;
	size_t bytesRead = fread(&header, 1, sizeof(WAVEHEADER), file);
	if (bytesRead != sizeof(WAVEHEADER))
	{
		fclose(file);
		return E_FAIL;
	}

	// WAV ���� ����
	if (memcmp(header.chunkId, "RIFF", 4) != 0 ||
		memcmp(header.format, "WAVE", 4) != 0 ||
		memcmp(header.subchunk1Id, "fmt ", 4) != 0 ||
		memcmp(header.subchunk2Id, "data", 4) != 0)
	{
		fclose(file);
		return E_FAIL;
	}

	// ���̺� ���� ���� �б�
	WAVEFORMATEX* pWaveFormat = (WAVEFORMATEX*)malloc(header.subchunk1Size);
	if (!pWaveFormat)
	{
		fclose(file);
		return E_OUTOFMEMORY;
	}

	bytesRead = fread(pWaveFormat, 1, header.subchunk1Size, file);
	if (bytesRead != header.subchunk1Size)
	{
		free(pWaveFormat);
		fclose(file);
		return E_FAIL;
	}

	// ���� ������ �б�
	BYTE* pData = (BYTE*)malloc(header.subchunk2Size);
	if (!pData)
	{
		free(pWaveFormat);
		fclose(file);
		return E_OUTOFMEMORY;
	}

	bytesRead = fread(pData, 1, header.subchunk2Size, file);
	if (bytesRead != header.subchunk2Size)
	{
		//free(pWaveFormat);
		//free(pData);
		//fclose(file);
		//return E_FAIL;//0506
	}

	// ��ȯ�� ����
	waveFormat_ = *pWaveFormat;
	buffer_.pAudioData = pData;
	buffer_.AudioBytes = header.subchunk2Size;

	buffer_.Flags = XAUDIO2_END_OF_STREAM; // ��Ʈ���� ������ ��Ÿ��
	buffer_.LoopCount = XAUDIO2_LOOP_INFINITE; // ���� Ƚ���� ���Ѵ�� ����


	// ���� �ݱ�
	fclose(file);

	return S_OK;
}

bool SoundPlayer::LoadWave(const wchar_t* filename, int type = 0)
{
	HRESULT hr;
	// WAVE ���� �ε�
	hr = LoadWaveFile(filename);


	if (FAILED(hr)) {
		return false;
	}

	if (type == 1)
	{
		buffer_.Flags = XAUDIO2_END_OF_STREAM;
		buffer_.LoopCount = 0; // �� ���� ����ϰ� ���߱� ���� ���� Ƚ���� 0���� ����
	}

	if (nullptr == sourceVoice_)
	{
		if (false == Initialize())
		{
			// �ҽ� ���̽� ����
			hr = xAudio2_->CreateSourceVoice(&sourceVoice_, &waveFormat_);
			if (FAILED(hr)) {
				cout << "error" << endl;
				return false;
			}
		}

		//Initialize();
	}

	// �ҽ� ���̽��� ���� ����
	hr = sourceVoice_->SubmitSourceBuffer(&buffer_);
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

void SoundPlayer::Play()
{
	// �ҽ� ���̽� ���
	if (sourceVoice_)
		sourceVoice_->Start();
}

void SoundPlayer::Stop()
{
	if (sourceVoice_)
	{
		// �ҽ� ���̽� ����
		sourceVoice_->Stop();
		sourceVoice_->FlushSourceBuffers();
	}
	//Terminate();
}

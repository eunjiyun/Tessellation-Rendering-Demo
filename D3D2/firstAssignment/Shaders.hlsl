//기본 그래픽스 파이프라인
//Vertex Shader -> Hull Shader -> Tessellator (고정기능)
//-> Domain Shader -> Geometry Shader (Optional) -> Pixel Shader

//Hull Shader는 테셀레이션 팩터 계산과 컨트롤 포인트 처리를 핵심 기능으로 둠
//이 값들은 테셀레이터라는 고정 기능 단계에 전달되어야만 의미 있음
//테셀레이터가 없는 파이프라인에서는 헐 셰이더를 호출할 이유가 없음

//Domain Shader는 테셀레이션 파이프라인 전용 셰이더.
//Hull Shader와 마찬가지로, 일반적인 렌더링에서는 쓰이지 않고, 테셀레이션을 활성화했을 때만 동작

//헐 셰이더와 도메인 셰이더는 테셀레이션 전용 셰이더.
//헐 셰이더는 얼마나 나눌까를 결정.
//도메인 셰이더는 나눠진 점들을 실제로 어디에 둘까? 계산
//테셀레이터는 gpu 고정 기능이라 프로그래머가 직접 제어 불가
//=> HS와 DS가 그 앞뒤를 담당.
//이 구조 덕분에 곡면 표현, LOD 최적화, 고품질 디테일 같은 기능을 효율적으로 구현 가능

//컴퓨터 그래픽스에서의 테셀레이션은 복잡한 도형(폴리곤이나 패치)를 
//더 단순한 기본 도형(주로 삼각형)으로 세분화하는 과정
//GPU가 실시간으로 물체의표면을 잘게 나누어더 정밀한 메쉬를 생성할 수 있게 함.

//헐 셰이더는 얼마나 나눌지 결정
//테셀레이터는 실제로 점을 분할 (고정 기능)
//도메인 셰이더는 새로 생긴 점들의 위치와 속성을 계산

//동적 디테일 조절 : 카메라 거리에 따라 자동으로 디테일을 늘리거나 줄임.
//곡면 표현 : 매끄러운 곡선/곡면을 저비용으로 구현 가능.
//성능 최적화 : 멀리 있는 물체는 단순하게, 가까운 물체는 정밀하게 표현.
//실루엣 개선 : 단순한 메시로는 표현하기 어려운 물체의 외곽선까지 정밀하게 다듬을 수 있음.

struct MATERIAL
{
	
	float4					m_cAmbient;
	float4					m_cDiffuse;
	//float4					m_cSpecular; //a = power
	//float4					m_cEmissive;

	float3					texMat;
	uint gnTexturesMask;

	//float		gfElapsedTime;
	float 		gfCurrentTime; 
	float hpColor;
};

#define DYNAMIC_TESSELLATION		0x60
#define DEBUG_TESSELLATION			0x80

cbuffer cbCameraInfo : register(b1)
{
	matrix		gmtxView : packoffset(c0);
	matrix		gmtxProjection : packoffset(c4);
	float3		gvCameraPosition : packoffset(c8);
};

cbuffer cbGameObjectInfo : register(b2)
{
	matrix		gmtxGameObject : packoffset(c0);
	MATERIAL	gMaterial : packoffset(c4);
};



#include "Light.hlsl"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//#define _WITH_VERTEX_LIGHTING

#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

#define _WITH_STANDARD_TEXTURE_MULTIPLE_DESCRIPTORS

#ifdef _WITH_STANDARD_TEXTURE_MULTIPLE_DESCRIPTORS
Texture2D gtxtAlbedoTexture : register(t6);
Texture2D gtxtSpecularTexture : register(t7);
Texture2D gtxtNormalTexture : register(t8);
Texture2D gtxtMetallicTexture : register(t9);
Texture2D gtxtEmissionTexture : register(t10);
Texture2D gtxtDetailAlbedoTexture : register(t11);
Texture2D gtxtDetailNormalTexture : register(t12);
#else
Texture2D gtxtStandardTextures[7] : register(t6);
#endif

Texture2D<float4> gtxtWaterBaseTexture : register(t3);
Texture2D<float4> gtxtWaterDetail0Texture : register(t4);
Texture2D<float4> gtxtWaterDetail1Texture : register(t5);

static matrix<float, 3, 3> sf3x3TextureAnimation = { { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f } };

SamplerState gssWrap : register(s0);


Texture2D gtxtTexture : register(t0);
struct VS_STANDARD_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
};

struct VS_STANDARD_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float3 tangentW : TANGENT;
	float3 bitangentW : BITANGENT;
	float2 uv : TEXCOORD;
};

VS_STANDARD_OUTPUT VSStandard(VS_STANDARD_INPUT input)
{
	VS_STANDARD_OUTPUT output;

	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.tangentW = (float3)mul(float4(input.tangent, 1.0f), gmtxGameObject);
	output.bitangentW = (float3)mul(float4(input.bitangent, 1.0f), gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

float4 PSStandard(VS_STANDARD_OUTPUT input) : SV_TARGET
{
	float4 cAlbedoColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cSpecularColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cNormalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cMetallicColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float4 cEmissionColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

#ifdef _WITH_STANDARD_TEXTURE_MULTIPLE_DESCRIPTORS
	if (gMaterial.gnTexturesMask & MATERIAL_ALBEDO_MAP) cAlbedoColor = gtxtAlbedoTexture.Sample(gssWrap, input.uv);
	if (gMaterial.gnTexturesMask & MATERIAL_SPECULAR_MAP) cSpecularColor = gtxtSpecularTexture.Sample(gssWrap, input.uv);
	if (gMaterial.gnTexturesMask & MATERIAL_NORMAL_MAP) cNormalColor = gtxtNormalTexture.Sample(gssWrap, input.uv);
	if (gMaterial.gnTexturesMask & MATERIAL_METALLIC_MAP) cMetallicColor = gtxtMetallicTexture.Sample(gssWrap, input.uv);
	if (gMaterial.gnTexturesMask & MATERIAL_EMISSION_MAP) cEmissionColor = gtxtEmissionTexture.Sample(gssWrap, input.uv);
#else
	if (gMaterial.gnTexturesMask & MATERIAL_ALBEDO_MAP) cAlbedoColor = gtxtStandardTextures[0].Sample(gssWrap, input.uv);
	if (gMaterial.gnTexturesMask & MATERIAL_SPECULAR_MAP) cSpecularColor = gtxtStandardTextures[1].Sample(gssWrap, input.uv);
	if (gMaterial.gnTexturesMask & MATERIAL_NORMAL_MAP) cNormalColor = gtxtStandardTextures[2].Sample(gssWrap, input.uv);
	if (gMaterial.gnTexturesMask & MATERIAL_METALLIC_MAP) cMetallicColor = gtxtStandardTextures[3].Sample(gssWrap, input.uv);
	if (gMaterial.gnTexturesMask & MATERIAL_EMISSION_MAP) cEmissionColor = gtxtStandardTextures[4].Sample(gssWrap, input.uv);
#endif

	float4 cIllumination = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float4 cColor = cAlbedoColor + cSpecularColor + cEmissionColor;
	if (gMaterial.gnTexturesMask & MATERIAL_NORMAL_MAP)
	{
		float3 normalW = input.normalW;
		float3x3 TBN = float3x3(normalize(input.tangentW), normalize(input.bitangentW), normalize(input.normalW));
		float3 vNormal = normalize(cNormalColor.rgb * 2.0f - 1.0f); //[0, 1] �� [-1, 1]
		normalW = normalize(mul(vNormal, TBN));
		cIllumination = Lighting(input.positionW, normalW);
		cColor = lerp(cColor, cIllumination, 0.5f);
	}

	return(cColor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_SKYBOX_CUBEMAP_INPUT
{
	float3 position : POSITION;
};

struct VS_SKYBOX_CUBEMAP_OUTPUT
{
	float3	positionL : POSITION;
	float4	position : SV_POSITION;
};

VS_SKYBOX_CUBEMAP_OUTPUT VSSkyBox(VS_SKYBOX_CUBEMAP_INPUT input)
{
	VS_SKYBOX_CUBEMAP_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.positionL = input.position;

	return(output);
}

TextureCube gtxtSkyCubeTexture : register(t13);
SamplerState gssClamp : register(s1);

float4 PSSkyBox(VS_SKYBOX_CUBEMAP_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtSkyCubeTexture.Sample(gssClamp, input.positionL);

	return(cColor);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_TEXTURED_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VS_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
	VS_TEXTURED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);

	output.uv = input.uv;

	return(output);
}
VS_TEXTURED_OUTPUT VSSpriteAnimation(VS_TEXTURED_INPUT input)
{
	VS_TEXTURED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);//gmtxGameObject

	if (gMaterial.texMat.z == 6)//���� 
	{
		output.uv.x = (input.uv.x) / gMaterial.texMat.z + gMaterial.texMat.x;
		output.uv.y = input.uv.y / gMaterial.texMat.z + gMaterial.texMat.y;
	}
	else if (gMaterial.texMat.z == 8)//�Ҳ�
	{
		output.uv.x = (input.uv.x) / gMaterial.texMat.z + gMaterial.texMat.x;
		output.uv.y = input.uv.y / (gMaterial.texMat.z * 0.75f) + gMaterial.texMat.y;
	}
	else
		output.uv = input.uv;

	return(output);
}

float4 PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(gssWrap, input.uv);

	if (gMaterial.texMat.z == 2 && cColor.x < 0.4f) 
		discard;

	return(cColor);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
Texture2D gtxtTerrainTexture : register(t14);
Texture2D gtxtDetailTexture : register(t15);
Texture2D gtxtAlphaTexture : register(t16);

float4 PSTerrain(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTerrainTexture.Sample(gssWrap, input.uv);
	return(cColor);
}

struct VS_TERRAIN_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

struct VS_TERRAIN_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

VS_TERRAIN_OUTPUT VSTerrain(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.color = input.color;
	output.uv0 = input.uv0;
	output.uv1 = input.uv1;

	return(output);
}

float4 PSTerrain(VS_TERRAIN_OUTPUT input) : SV_TARGET
{
	float4 cBaseTexColor = gtxtTerrainTexture.Sample(gssWrap, input.uv0);
	float4 cDetailTexColor = gtxtDetailTexture.Sample(gssWrap, input.uv1);
	//	float fAlpha = gtxtTerrainTexture.Sample(gssWrap, input.uv0);

	float4 cColor = cBaseTexColor * 0.5f + cDetailTexColor * 0.5f;
	//	float4 cColor = saturate(lerp(cBaseTexColor, cDetailTexColor, fAlpha));

	return(cColor);
}


//=====================================================================================

struct VS_RIPPLE_WATER_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
};

struct VS_RIPPLE_WATER_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
};

VS_RIPPLE_WATER_OUTPUT VSRippleWater(VS_RIPPLE_WATER_INPUT input)
{
	VS_RIPPLE_WATER_OUTPUT output;

	//	input.position.y += sin(gfCurrentTime * 0.5f + input.position.x * 0.01f + input.position.z * 0.01f) * 35.0f;
	//	input.position.y += sin(input.position.x * 0.01f) * 45.0f + cos(input.position.z * 0.01f) * 35.0f;
	//	input.position.y += sin(gfCurrentTime * 0.5f + input.position.x * 0.01f) * 45.0f + cos(gfCurrentTime * 1.0f + input.position.z * 0.01f) * 35.0f;
	//	input.position.y += sin(gfCurrentTime * 0.5f + ((input.position.x * input.position.x) + (input.position.z * input.position.z)) * 0.01f) * 35.0f;
	//	input.position.y += sin(gfCurrentTime * 1.0f + (((input.position.x * input.position.x) + (input.position.z * input.position.z)) - (1000 * 1000) * 2) * 0.0001f) * 10.0f;

	input.position.y += sin(gMaterial.gfCurrentTime * 1.0f + (((input.position.x * input.position.x) + (input.position.z * input.position.z))) * 0.0001f) * 10.0f;
	//input.position.y += sin(gMaterial.gfCurrentTime * 0.35f + input.position.x * 0.35f) * 2.95f + cos(gMaterial.gfCurrentTime * 0.30f + input.position.z * 0.35f) * 2.05f;
	output.position = mul(float4(input.position, 1.0f), gmtxGameObject);
	if (436.833f < output.position.y) output.position.y = 436.833f;
	output.position = mul(mul(output.position, gmtxView), gmtxProjection);

	//	output.color = input.color;
	output.color = (input.position.y / 200.0f) + 0.55f;
	output.uv0 = input.uv0;
	//	output.uv1 = input.uv1;

	return(output);
}

float4 PSRippleWater(VS_RIPPLE_WATER_OUTPUT input) : SV_TARGET
{
	float2 uv = input.uv0;

#ifdef _WITH_STATIC_MATRIX
	sf3x3TextureAnimation._m21 = gMaterial.gfCurrentTime * 0.00125f;
	uv = mul(float3(input.uv0, 1.0f), sf3x3TextureAnimation).xy;
#else
#ifdef _WITH_CONSTANT_BUFFER_MATRIX
	uv = mul(float3(input.uv0, 1.0f), (float3x3)gf4x4TextureAnimation).xy;
	//	uv = mul(float4(uv, 1.0f, 0.0f), gf4x4TextureAnimation).xy;
#else
	uv.y += gMaterial.gfCurrentTime * 0.0825f /*0.00125f*/;
	uv.x += gMaterial.gfCurrentTime * 0.00125f;
#endif
#endif
	//gssWrap
	//float4 cBaseTexColor = gtxtWaterBaseTexture.SampleLevel(gSamplerState, uv, 0);
	//float4 cDetail0TexColor = gtxtWaterDetail0Texture.SampleLevel(gSamplerState, uv * 10.0f, 0);
	//float4 cDetail1TexColor = gtxtWaterDetail1Texture.SampleLevel(gSamplerState, uv * 5.0f, 0);

	float4 cBaseTexColor = gtxtWaterBaseTexture.SampleLevel(gssWrap, uv, 0);
	float4 cDetail0TexColor = gtxtWaterDetail0Texture.SampleLevel(gssWrap, uv * 10.0f, 0);
	float4 cDetail1TexColor = gtxtWaterDetail1Texture.SampleLevel(gssWrap, uv * 5.0f, 0);

	float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	cColor = lerp(cBaseTexColor * cDetail0TexColor, cDetail1TexColor.r * 0.5f, 0.35f);

	return(cColor);
}

struct VS_LIGHTING_INPUT
{
	float3	position    : POSITION;
	float3	normal		: NORMAL;
};

struct VS_LIGHTING_OUTPUT
{
	float4	position    : SV_POSITION;
	float3	positionW   : POSITION;
	float3	normalW		: NORMAL;
};

VS_LIGHTING_OUTPUT VSCubeMapping(VS_LIGHTING_INPUT input)
{
	VS_LIGHTING_OUTPUT output;

	output.positionW = mul(float4(input.position, 1.0f), gmtxGameObject).xyz;
	//	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxGameObject);
	output.normalW = mul(float4(input.normal, 0.0f), gmtxGameObject).xyz;
	//	output.normalW = mul(input.normal, (float3x3)gmtxGameObject);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);

	return(output);
}
//gssWrap
TextureCube gtxtCubeMap : register(t1);

float4 PSCubeMapping(VS_LIGHTING_OUTPUT input) : SV_Target
{
	input.normalW = normalize(input.normalW);

	float4 cIllumination = Lighting(input.positionW, input.normalW);

	float3 vFromCamera = normalize(input.positionW - gvCameraPosition.xyz);
	float3 vReflected = normalize(reflect(vFromCamera, input.normalW));
	float4 cCubeTextureColor = gtxtCubeMap.Sample(gssWrap, vReflected);

	//	return(float4(vReflected * 0.5f + 0.5f, 1.0f));
		return(cCubeTextureColor);
		//	return(cIllumination * cCubeTextureColor);
}



struct VS_TERRAIN_TESSELLATION_OUTPUT
{
	float3 position : POSITION;
	float3 positionW : POSITION1;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

VS_TERRAIN_TESSELLATION_OUTPUT VSTerrainTessellation(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_TESSELLATION_OUTPUT output;

	output.position = input.position;
	output.positionW = mul(float4(input.position, 1.0f), gmtxGameObject).xyz;
	output.color = input.color;
	output.uv0 = input.uv0;
	output.uv1 = input.uv1;

	return(output);
}


//Edge는 이웃 패치와 공유되는 경계라서 crack 방지용으로 따로 계산하고,
//Inside는 그 패치 내부 전용이라 중심점 기준으로 계산해도 됨.
struct HS_TERRAIN_TESSELLATION_CONSTANT
{
	//GPU에게 이 패치를 몇 개로 쪼갤지를 결정

	//테두리 : 사각형의 4변을 각각 얼마나 쪼갤지
	float fTessEdges[4] : SV_TessFactor;

	//안쪽 : 내부를 얼마나 촘촘히 쪼갤지
	//fTessInsides[0] -> u 방향 분할 수
	//fTessInsides[1] -> v 방향 분할 수
	float fTessInsides[2] : SV_InsideTessFactor;//이 패치를 얼마나 잘게 쪼갤지를 GPU에 알려주는 숫자가 TessFactor
};

struct HS_TERRAIN_TESSELLATION_OUTPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

struct DS_TERRAIN_TESSELLATION_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
	float4 tessellation : TEXCOORD2;
};

//테셀레이션 핵심 

//Bezier Surface : 제어점(Control Points)로 정의되는 곡면.
	//Tessellation : 곡면을 삼각형이나 사각형 패치로 세분화하여 실제 렌더링 가능한
	//메시(mesh)로 만드는 과정

	//GPU Tessellation 파이프라인 : 
	//Hull Shader -> Tessellator(고정기능) -> Domain Shader 단계에서 
    //곡면을 세밀하게 분할하고, 최종적으로 화면에 그릴 정점 데이터를 생성함.

//VS (Vertex Shader) - 준비 단계
//VS_TERRAIN_TESSELLATION_OUTPUT VSTerrainTessellation(...)
//=>여기서는 아무것도 쪼개지 않는다.
//하는 일:
// - 원래 정점 좌표를 넘김
// - 월드 좌표(positionW) 계산
// - 색상/UV 전달
//positionW를 따로 계산한 이유는
//나중에 HS에서 카메라와의 거리 계산하려고.
//VS는 패치 계산을 위한 정보 전달 단계

//Hull Shader (HS) - 몇 개로 쪼갤지 결정
//*여기가 핵심*

//(1) Control Point Shader 부분
//HS_TERRAIN_TESSELLATION_OUTPUT HSTerrainTessellation(...)
//이건 그냥 control point를 그대로 전달하는 역할.

//(2) Patch Constant Function
//HSTerrainTessellationConstant(...)
//여기서 "이 패치를 몇 개로 나눠"하고 GPU에게 전달

//거리 기반 Adaptive Tessellation
//float fDistToCamera = distance(f3Position, gvCameraPosition);
//return(lerp(64.0f, 1.0f, s));

//의미
//가까우면 -> 64개로 많이 쪼갬
//멀면 -> 1개로 거의 안 쪼갬
//=>이게 LOD 개념

//Edge 계산
//output.fTessEdges[0] = CalculateTessFactor(e0);
//edge 기준으로 계산하는 이유는
//패치 경계에서 크랙 방지하려고.
//Edge 단위로 분할 정도를 정해야 옆 패치와 맞물린다.

//(3) Tessellator (고정 기능 하드웨어)
//여기는 코드가 없음.
//GPU 내부에서 자동으로:
// - Edge Factor 기준으로
// - 삼각형 또는 quad를 분할
//프로젝트에는 integer partitioning을 사용했음.

//(4) Domain Shader (DS) - 실제 위치 계산
//수학적으로 가장 깊은 부분
//25개 Control Point
//[

//특징
//정밀도 조절 가능 : 
//카메라와의 거리, 곡면의 곡률(curvature)에 따라 삼각형 분할 정도를 동적으로 조절 가능

//적응형 테셀레이션 : 
//곡면의 복잡한 부분은 더 세밀하게, 평평한 부분은 덜 세밀하게 분할하여 성능과 품질을 
//균형 있게 유지

//활용 예시 : 자동차 외형, 곡선 기반 건축물, 고품질 CAD 모델을 실시간 렌더링할 때 사용됨.

//테셀레이션 기본 단위를 패치(patch)라고 부르는데,
//패치는 크게 두 종류가 있음
//triangle 패치 (삼각형 기반)
//quad 패치 (사각형 기반)
//[domain("quad")]는 사각형 하나를 기준으로 쪼갤거라는 선언.
//사각형이 좋은 이유
// : 지형(terrain)은 대부분 격자(Grid) 기반이라
//사각형 패치로 나누는 게 자연스럽고 UV도 다루기 쉬움.

[domain("quad")]
//[partitioning("fractional_even")]
[partitioning("integer")]
//=>의미 : 
// - 소수 분할 없이 정수 단위 분할
// - 안정적
// - fractional보다 덜 부드럽지만 안전


[outputtopology("triangle_cw")]


//Control Point는 곡면을 만들기 위한 손잡이 (조절점)
//직선은 점 2개면 ok, 곡선은 점이 더 필요함, 곡면(2D로 휘는 면)은 점이 훨씬 더 필요함
//5x5 = 25개 점으로 한 면을 정의함.
//25개를 쓰는 이유는 Bezier Surface(베지어 곡면)을 쓰고 있음.
//베지어 곡면은 곡면을 매끈하게 만들기 위해 Control Point를 격자 형태로 깔아두고,
//그 점들을 기준으로 사이를 부드럽게 보간(중간값 계산)하는 방식.

//패치(사각형) 1개를
//Control Point 25개로 정의하고
//그 사이를 베지어 공식으로 매끈하게 만드는 거.
[outputcontrolpoints(25)]
[patchconstantfunc("HSTerrainTessellationConstant")]
[maxtessfactor(64.0f)]
HS_TERRAIN_TESSELLATION_OUTPUT HSTerrainTessellation(InputPatch<VS_TERRAIN_TESSELLATION_OUTPUT, 25> input, uint i : SV_OutputControlPointID)
{
	HS_TERRAIN_TESSELLATION_OUTPUT output;

	output.position = input[i].position;
	output.color = input[i].color;
	output.uv0 = input[i].uv0;
	output.uv1 = input[i].uv1;

	return(output);
}

float CalculateTessFactor(float3 f3Position)
{
	//카메라 가까우면 64로 많이 쪼개고
	//멀어지면 1로 줄인다

	//거리 기반 Adaptive Tessellation
	//distance() : 카메라와 해당 지점 사이 거리를 잰다
	float fDistToCamera = distance(f3Position, gvCameraPosition);

	//saturate() : 0~1 사이로 값 제한(안전장치)
	float s = saturate((fDistToCamera - 10.0f) / (500.0f - 10.0f));


	//lerp(64, 1, s) :
	//s가 0이면 64 (가까움 -> 많이 쪼갬)
	//s가 1이면 1 (멀어짐 -> 거의 안 쪼갬)
	return(lerp(64.0f, 1.0f, s));
	//	return(pow(2, lerp(20.0f, 4.0f, s)));

	//=>이렇게 하는 이유는 사람 눈은 가까운 곳 디테일은 민감한데
	//멀리 있는 건 디테일이 커도 잘 티가 안 나기에
	//멀리 있는 지형까지 정점을 잔뜩 만들면 GPU 낭비가 됨
}

HS_TERRAIN_TESSELLATION_CONSTANT HSTerrainTessellationConstant(InputPatch<VS_TERRAIN_TESSELLATION_OUTPUT, 25> input)
{
	HS_TERRAIN_TESSELLATION_CONSTANT output;

	if (gnRenderMode & DYNAMIC_TESSELLATION)
	{
		float3 e0 = 0.5f * (input[0].positionW + input[4].positionW);
		float3 e1 = 0.5f * (input[0].positionW + input[20].positionW);
		float3 e2 = 0.5f * (input[4].positionW + input[24].positionW);
		float3 e3 = 0.5f * (input[20].positionW + input[24].positionW);

		output.fTessEdges[0] = CalculateTessFactor(e0);
		output.fTessEdges[1] = CalculateTessFactor(e1);
		output.fTessEdges[2] = CalculateTessFactor(e2);
		output.fTessEdges[3] = CalculateTessFactor(e3);

		float3 f3Sum = float3(0.0f, 0.0f, 0.0f);
		for (int i = 0; i < 25; i++) f3Sum += input[i].positionW;
		float3 f3Center = f3Sum / 25.0f;
		output.fTessInsides[0] = output.fTessInsides[1] = CalculateTessFactor(f3Center);
	}
	else
	{
		output.fTessEdges[0] = 20.0f;
		output.fTessEdges[1] = 20.0f;
		output.fTessEdges[2] = 20.0f;
		output.fTessEdges[3] = 20.0f;

		output.fTessInsides[0] = 20.0f;
		output.fTessInsides[1] = 20.0f;
	}

	return(output);
}

void BernsteinCoeffcient5x5(float t, out float fBernstein[5])
{
	float tInv = 1.0f - t;
	fBernstein[0] = tInv * tInv * tInv * tInv;
	fBernstein[1] = 4.0f * t * tInv * tInv * tInv;
	fBernstein[2] = 6.0f * t * t * tInv * tInv;
	fBernstein[3] = 4.0f * t * t * t * tInv;
	fBernstein[4] = t * t * t * t;
}

float3 CubicBezierSum5x5(OutputPatch<HS_TERRAIN_TESSELLATION_OUTPUT, 25> patch, float uB[5], float vB[5])
{
	float3 f3Sum = float3(0.0f, 0.0f, 0.0f);
	f3Sum = vB[0] * (uB[0] * patch[0].position + uB[1] * patch[1].position + uB[2] * patch[2].position + uB[3] * patch[3].position + uB[4] * patch[4].position);
	f3Sum += vB[1] * (uB[0] * patch[5].position + uB[1] * patch[6].position + uB[2] * patch[7].position + uB[3] * patch[8].position + uB[4] * patch[9].position);
	f3Sum += vB[2] * (uB[0] * patch[10].position + uB[1] * patch[11].position + uB[2] * patch[12].position + uB[3] * patch[13].position + uB[4] * patch[14].position);
	f3Sum += vB[3] * (uB[0] * patch[15].position + uB[1] * patch[16].position + uB[2] * patch[17].position + uB[3] * patch[18].position + uB[4] * patch[19].position);
	f3Sum += vB[4] * (uB[0] * patch[20].position + uB[1] * patch[21].position + uB[2] * patch[22].position + uB[3] * patch[23].position + uB[4] * patch[24].position);

	return(f3Sum);
}

[domain("quad")]
DS_TERRAIN_TESSELLATION_OUTPUT DSTerrainTessellation(HS_TERRAIN_TESSELLATION_CONSTANT patchConstant, float2 uv : SV_DomainLocation, OutputPatch<HS_TERRAIN_TESSELLATION_OUTPUT, 25> patch)
{
	DS_TERRAIN_TESSELLATION_OUTPUT output = (DS_TERRAIN_TESSELLATION_OUTPUT)0;

	float uB[5], vB[5];
	BernsteinCoeffcient5x5(uv.x, uB);
	BernsteinCoeffcient5x5(uv.y, vB);

	output.color = lerp(lerp(patch[0].color, patch[4].color, uv.x), lerp(patch[20].color, patch[24].color, uv.x), uv.y);
	output.uv0 = lerp(lerp(patch[0].uv0, patch[4].uv0, uv.x), lerp(patch[20].uv0, patch[24].uv0, uv.x), uv.y);
	output.uv1 = lerp(lerp(patch[0].uv1, patch[4].uv1, uv.x), lerp(patch[20].uv1, patch[24].uv1, uv.x), uv.y);

	//25개 control point를 이용해서
	//Bezier 곡면으로 실제 위치 계산

	//Bezier Surface 기반 테셀레이션은 컴퓨터 그래픽스에서 곡면을 더 세밀하게 
	//삼각형(폴리곤)으로 분할하는 과정을 말함.
	//특히 Bezier 곡면은 수학적으로 정의된 곡선/곡면으로, 이를 GPU에서 직접 렌더링하기 위해서는
	//테셀레이션 셰이더를 통해 작은 삼각형 패치로 나누어야 함.
	float3 position = CubicBezierSum5x5(patch, uB, vB);
	matrix mtxWorldViewProjection = mul(mul(gmtxGameObject, gmtxView), gmtxProjection);
	output.position = mul(float4(position, 1.0f), mtxWorldViewProjection);

	output.tessellation = float4(patchConstant.fTessEdges[0], patchConstant.fTessEdges[1], patchConstant.fTessEdges[2], patchConstant.fTessEdges[3]);

	return(output);
}

float4 PSTerrainTessellation(DS_TERRAIN_TESSELLATION_OUTPUT input) : SV_TARGET
{
	float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	if (gnRenderMode& (DEBUG_TESSELLATION | DYNAMIC_TESSELLATION))
	{
		if (input.tessellation.w <= 5.0f) cColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
		else if (input.tessellation.w <= 10.0f) cColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
		else if (input.tessellation.w <= 20.0f) cColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
		else if (input.tessellation.w <= 30.0f) cColor = float4(1.0f, 0.0f, 1.0f, 1.0f);
		else if (input.tessellation.w <= 40.0f) cColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
		else if (input.tessellation.w <= 50.0f) cColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
		else if (input.tessellation.w <= 55.0f) cColor = float4(0.2f, 0.2f, 0.72f, 1.0f);
		else if (input.tessellation.w <= 60.0f) cColor = float4(0.5f, 0.75f, 0.75f, 1.0f);
		else cColor = float4(0.87f, 0.17f, 1.0f, 1.0f);
	}
	else
	{
		float4 cBaseTexColor = gtxtTerrainTexture.Sample(gssWrap, input.uv0);
		float4 cDetailTexColor = gtxtDetailTexture.Sample(gssWrap, input.uv1);
		float fAlpha = gtxtAlphaTexture.Sample(gssWrap, input.uv0);

		cColor = saturate(lerp(cBaseTexColor, cDetailTexColor, fAlpha));
	}

	return(cColor);
}



VS_TEXTURED_OUTPUT VSTextureToViewport(uint nVertexID : SV_VertexID)
{
	VS_TEXTURED_OUTPUT output = (VS_TEXTURED_OUTPUT)0;

	if (nVertexID == 0) { output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 0.0f); }
	if (nVertexID == 1) { output.position = float4(+1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 0.0f); }
	if (nVertexID == 2) { output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 1.0f); }
	if (nVertexID == 3) { output.position = float4(-1.0f, +1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 0.0f); }
	if (nVertexID == 4) { output.position = float4(+1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(1.0f, 1.0f); }
	if (nVertexID == 5) { output.position = float4(-1.0f, -1.0f, 0.0f, 1.0f); output.uv = float2(0.0f, 1.0f); }

	return(output);
}

float4 PSTextureToViewport(VS_TEXTURED_OUTPUT input) : SV_Target
{
	if(gMaterial.hpColor==0)
		return float4(1.f,0.f,0.f,0.f);
	else
		return float4(0.f,1.f,1.f,0.f);
}
#pragma once


namespace Math {
	float favgf(const float* fparr, const unsigned int count);
	// 아직은 사용하지 않지만, 최대한 함수 오버로딩을 사용하자.
	float favgf(std::vector<float>& farr);


	float radians(float fDegrees);
	
	float	Rand(float fMin, float fMax);
	int		Rand(int nMin, int nMax);
	double	Rand(double dMin, double dMax);
	
	namespace Vector3 {
		DirectX::XMFLOAT3 XMVectorToFloat3(DirectX::XMVECTOR& xmVector);
		DirectX::XMFLOAT3 ScalarProduct(DirectX::XMFLOAT3 xmf3Vector,float fScalar,bool bNormalize = true);
		DirectX::XMFLOAT3 Add(const DirectX::XMFLOAT3& xmf3Vector1, DirectX::XMFLOAT3& xmf3Vector2);

	}
	

	namespace Matrix4x4 {
		DirectX::XMFLOAT4X4 Identity();
	}
	
}

#include "pch.h"
#include "Math.h"

/// <summary>
/// Get Float Average
/// </summary>
/// <param name="arr"> : Float Array</param>
/// <param name="count"> : Count of Float</param>
/// <returns></returns>
float Math::favgf(const float* arr, const unsigned int count) {
	float sum = 0.f;
	for (unsigned long i = 0; i < count; ++i) {
		sum += arr[i];
	}
	if (sum == 0.f or count == 0) {
		return 0.f;
	}
	else {
		return sum / count;
	}
}

float Math::favgf(std::vector<float>& farr) {
	return 0.f;
}

constexpr float PI = 3.1415927f;
float Math::radians(float Degrees){
	return Degrees * (PI/180.f);
}



std::random_device rd{};
std::mt19937 gen(rd());



float Math::Rand(float fMin, float fMax){
	std::uniform_real_distribution<float> drf(fMax, fMax);
	return drf(gen);
}

int Math::Rand(int nMin, int nMax){
	std::uniform_int_distribution<int> dri(nMin, nMax);
	return dri(gen);
}

double Math::Rand(double dMin, double dMax){
	std::uniform_real_distribution<double> drd(dMin, dMax);
	return drd(gen);
}

DirectX::XMFLOAT3 Math::Vector3::XMVectorToFloat3(DirectX::XMVECTOR& xmVector){
	return DirectX::XMFLOAT3();
}

DirectX::XMFLOAT3 Math::Vector3::ScalarProduct(DirectX::XMFLOAT3 xmf3Vector, float fScalar, bool bNormalize)
{
	return DirectX::XMFLOAT3();
}

DirectX::XMFLOAT3 Math::Vector3::Add(const DirectX::XMFLOAT3& xmf3Vector1, DirectX::XMFLOAT3& xmf3Vector2)
{
	return DirectX::XMFLOAT3();
}

DirectX::XMFLOAT4X4 Math::Matrix4x4::Identity(){
	DirectX::XMFLOAT4X4 Result{};
	DirectX::XMStoreFloat4x4(&Result, DirectX::XMMatrixIdentity());
	return Result;
}

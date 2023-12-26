#pragma once

struct VertexShader_ConstantBuffer_Camera_Info {
	DirectX::XMFLOAT4X4 m_xmf4x4View{ Math::Matrix4x4::Identity() };
	DirectX::XMFLOAT4X4 m_xmf4x4Projection{ Math::Matrix4x4::Identity() };
};
using VS_CB_CAMERAINFO = VertexShader_ConstantBuffer_Camera_Info;

class Camera{
public:
	Camera();
	~Camera();


private:

	DirectX::XMFLOAT3 m_eye{};
	DirectX::XMFLOAT3 m_at{};
	DirectX::XMFLOAT3 m_up{ 0.f,1.f,0.f };

	DirectX::XMFLOAT3 m_basisX{};
	DirectX::XMFLOAT3 m_basisY{};
	DirectX::XMFLOAT3 m_basisZ{};


	float m_fNearZ{};
	float m_fFarZ{};
	float m_fAspect{};
	float m_fFovY{};

	DirectX::XMFLOAT4X4 m_xmView{};
	DirectX::XMFLOAT4X4 m_xmProjection{};

	D3D12_VIEWPORT m_d3dViewport{ 0,0,FRAMEBUFFER_WIDTH,FRAMEBUFFER_HEIGHT,0.f,1.f };
	D3D12_RECT m_d3dScissorRect{ 0,0,FRAMEBUFFER_WIDTH,FRAMEBUFFER_HEIGHT };

public:
	virtual void Update(float fTime) PURE;
	virtual void Render() PURE;

};


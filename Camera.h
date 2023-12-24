#pragma once
class Camera{
public:
	Camera();
	~Camera();


private:

	DirectX::XMFLOAT3 m_position{};
	DirectX::XMFLOAT3 m_eye{};
	DirectX::XMFLOAT3 m_at{};
	DirectX::XMFLOAT3 m_up{ 0.f,1.f,0.f };

	DirectX::XMFLOAT3 m_basisX{};
	DirectX::XMFLOAT3 m_basisY{};
	DirectX::XMFLOAT3 m_basisZ{};


	float m_nearZ{};
	float m_farZ{};
	float m_aspect{};
	float m_fovY{};

	DirectX::XMFLOAT4X4 m_view{};
	DirectX::XMFLOAT4X4 m_projection{};

private:



};


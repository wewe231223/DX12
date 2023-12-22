#pragma once



class Scene{
protected:
	ID3D12RootSignature* m_pd3dGraphicsRootSignature{ nullptr };

public:

	Scene();
	~Scene();


public:

	void Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void Terminate();


	void Update(float DeltaTime);
	void Render(ID3D12GraphicsCommandList* pd3dCommandList);



};


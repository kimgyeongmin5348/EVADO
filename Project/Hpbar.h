#pragma once
#include"stdafx.h"
#include "object.h"
class Hpbar : public CGameObject
{
public:
    Hpbar(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
    virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* camera) override;
    void SetHpbar(float ratio);
private:
    std::vector<CGameObject*> m_hpSegments;
    int m_nSegments = 4;

};


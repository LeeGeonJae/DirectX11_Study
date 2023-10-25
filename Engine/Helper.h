#pragma once
#include "pch.h"

DirectX::XMFLOAT3 CalculateNormalVector(const DirectX::XMFLOAT3& vertex1, const DirectX::XMFLOAT3& vertex2, const DirectX::XMFLOAT3& vertex3)
{
    DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&vertex1);
    DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&vertex2);
    DirectX::XMVECTOR v3 = DirectX::XMLoadFloat3(&vertex3);

    // Calculate two edge vectors
    DirectX::XMVECTOR edge1 = DirectX::XMVectorSubtract(v2, v1);
    DirectX::XMVECTOR edge2 = DirectX::XMVectorSubtract(v3, v1);

    // Calculate the cross product of the edge vectors
    DirectX::XMVECTOR normal = DirectX::XMVector3Cross(edge1, edge2);

    // Normalize the result
    normal = DirectX::XMVector3Normalize(normal);

    DirectX::XMFLOAT3 result;
    DirectX::XMStoreFloat3(&result, normal);

    return result;
}

std::string GetFileName(const aiString& str)
{
    std::string filename = str.C_Str();

    filename.erase(filename.begin(), filename.begin() + filename.find_last_of('\\') + 1);

    return filename;
}
#pragma once

#include "../Engine/Header.h"
#include "BufferStruct.h"
#include "Struct.h"
#include "enum.h"

// ��¥ ���� �ڵ�.. �����ؾ���.. node class ���� ���� ����
class Mesh
{
public:
	void Draw(ID3D11DeviceContext* deviceContext);
	void Close();


public:
	void SetupMesh(ID3D11Device* device);

	inline string GetName();
	inline void SetName(string name);
	inline string GetParentName();
	inline void SetParentName(string name);

public:
	vector<BoneWeightVertex> m_BoneWeightVertices;
	vector<UINT> m_Indices;

private:
	ID3D11Buffer* m_VertexBuffer;
	ID3D11Buffer* m_IndexBuffer;

	string m_Name;
	string m_ParnetName;
};

string Mesh::GetName()
{
	return m_Name;
}

void Mesh::SetName(string name)
{
	m_Name = name;
}

string Mesh::GetParentName()
{
	return m_ParnetName;
}

void Mesh::SetParentName(string name)
{
	m_ParnetName = name;
}
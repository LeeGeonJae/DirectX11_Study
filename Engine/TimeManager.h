#pragma once

class TimeManager
{
public:
	static TimeManager* GetInstance()
	{
		static TimeManager mgr;
		return &mgr;
	}
private:
	TimeManager();
	~TimeManager();

public:
	void Initialize();
	void Update();
	void Render();

public:
	double GetDT() { return m_dDT; }
	float GetfDT() { return (float)m_dDT; }

private:
	LARGE_INTEGER	m_llcurCount;
	LARGE_INTEGER	m_llPrevCount;
	LARGE_INTEGER	m_llFrequency;

	double			m_dDT;	// ������ ������ �ð���
	double			m_dAcc;	// 1�� üũ�� ���� ���� �ð�
	UINT			m_iCallCount;	// �Լ� ȣ�� Ƚ�� üũ
	UINT			m_iFPS;		//�ʴ� ȣ�� Ƚ��
};
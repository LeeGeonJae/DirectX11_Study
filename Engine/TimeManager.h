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

	double			m_dDT;	// 프레임 사이의 시간값
	double			m_dAcc;	// 1초 체크를 위한 누적 시간
	UINT			m_iCallCount;	// 함수 호출 횟수 체크
	UINT			m_iFPS;		//초당 호출 횟수
};
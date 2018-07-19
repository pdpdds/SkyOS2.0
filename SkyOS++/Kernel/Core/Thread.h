#pragma once
#include "windef.h"
#include "struct.h"

class Process;

class Thread
{
public:
	Thread();
	virtual ~Thread();
		
	int			m_taskState; //�½�ũ ����. Init, Running, Stop, Terminate
	UINT32		m_dwPriority; //�켱����
	int			m_waitingTime; // �½�ũ CPU ���� �ð�

	Process*	m_pParent; //�θ� ���μ���

	LPVOID		m_startParam; //������ ���۽� �����Ǵ� �Ķ����
	void*		m_initialStack; //���̽� ���� �ּ�
	
	uint32_t	m_esp;	//�½�ũ�� ���� ������
	UINT32		m_stackLimit; //������ ũ��	
	trapFrame	frame;

	uint32_t	m_imageBase;  // ���Ͽ��� �ڵ带 �ε��� ���, ������ �޸𸮿� �ε�� ���̽� �ּ�
	uint32_t	m_imageSize;  //������ ũ��. ������ �ε��ؼ� �����Ǵ� �����尡 �ƴ� ��� m_imageBase�� m_imageSize�� �ǹ̰� ����.
	
	registers_t m_contextSnapshot; //�½�ũ�� ���ؽ�Ʈ ������(��������)

	void SetThreadId(DWORD threadId) { m_threadId = threadId; }
	DWORD GetThreadId() { return m_threadId; }
	
	//Thread Local Storage. Not Implemented
	void*		m_lpTLS = nullptr;

protected:
	DWORD			m_threadId;	
};
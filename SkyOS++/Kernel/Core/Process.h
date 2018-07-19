#pragma once
#include "windef.h"
#include "list.h"
#include "map.h"
#include "VirtualMemoryManager.h"

#define PROC_INVALID_ID -1

using namespace std;

class Thread;

extern void HaltSystem(const char* errMsg);

class Process
{	
public:
	Process();
	virtual ~Process();

	typedef map<int, Thread*> ThreadList;

	bool AddMainThread(Thread* pThread); //���ν����带 �߰��Ѵ�.
	bool AddThread(Thread* pThread); //�����带 �߰��Ѵ�.

	Thread* GetThreadById(int index); //������ ���̵�� ������ ��ü�� ��´�.
	Thread* GetMainThread(); //������ ���̵�� ������ ��ü�� ��´�.
	
	char		m_processName[MAX_PROCESS_NAME]; //���μ��� �̸�

	UINT32		m_dwRunState; //���μ��� ����
	UINT32		m_dwPriority; //���μ��� �켱����
	int			m_dwRunningTime; // ���μ��� CPU ���� �ð�

	UINT32		m_dwPageCount; //���μ����� �����ϴ� ������ ��
	UINT32		m_dwProcessType; //���μ��� Ÿ��		
		
	int			m_stackIndex; //�����尡 �����ɶ� ���� �� ������ �Ҵ��� �־�� �ϴµ� �׶� ���Ǵ� �ε���
	ThreadList m_threadList; //���μ������� ���� ������ ����Ʈ

	uint32_t	m_imageBase; //���Ϸ� ���� �ڵ带 �ε��� ��� ����. �޸𸮿� �ε�� �ּ�
	uint32_t	m_imageSize; //������ ũ��

	int			m_mainThreadId;
	bool		m_IskernelProcess;

	void*		m_lpHeap; //���� ���μ����� ����ϴ� ��

//������ ���丮�� �����Ѵ�.
	PageDirectory* GetPageDirectory() { return m_pPageDirectory; }
	void SetPageDirectory(PageDirectory* pPageDirectory);
	
	int GetProcessId() {return m_processId;}
	void SetProcessId(int processId) { m_processId = processId; }

	bool AddMessage(char* pMsg);
	list<char*>& GetMessageList() { return m_messageList; }

private:
	PageDirectory * m_pPageDirectory; //��	���μ����� ����ϴ� ������ ���丮		
	int		m_processId; //���μ��� ���̵�
	list<char*> m_messageList;
	
};
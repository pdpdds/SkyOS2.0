-- lua coroutine �׽�Ʈ �Լ�
function ThreadTest()
	print("ThreadTest ����")
	
	print("TestFunc ȣ��")
	-- ���ο��� lua_yield() �� ȣ��ȴ�.
	TestFunc()
	TestFunc2(1.2)
	print("TestFunc ����")
	
	print("g_test::TestFunc() ȣ��")
	-- ���ο��� lua_yield() �� ȣ��ȴ�.
	g_test:TestFunc()
	g_test:TestFunc2(2.3)
	print("g_test::TestFunc() ����")
	
	print("ThreadTest ����")
end

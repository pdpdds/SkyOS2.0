-- �׽�Ʈ �Լ�
function test_error()
	
	print("test_error() called !!")
	
	test_error_1()
end

-- �׽�Ʈ �Լ�
function test_error_1()

	print("test_error_1() called !!")
	
	test_error_2()
end

-- �׽�Ʈ �Լ�
function test_error_2()

	print("test_error_2() called !!")
	
	-- �������� �ʴ� �Լ� ȣ���� �õ��Ѵ�.
	test_error_3()
end
-- ���� haha ���̺��� Ȯ���Ѵ�.
print(haha)

-- ���� haha.value�� ����Ѵ�.
print(haha.value)

-- ���� haha.inside ���̺��� Ȯ���Ѵ�.
print(haha.inside)

-- ���� haha.inside.value�� ����Ѵ�.
print(haha.inside.value)

-- haha.test ���� �ִ´�.
haha.test = "input from lua"

-- LuaTinker �� �Է����� ���̺��� �ѱ��.
function print_table(arg)
	print("arg = ", arg)
	print("arg.name = ", arg.name)
end

-- LuaTinker ���� ���̺��� �ѱ��.
function return_table(arg)
	local ret = {}
	ret.name = arg
	return ret
end
#pragma once

enum enumDumpDetectionLevel
{
	DL_DEFAULT, //�Ϲ����� �ڵ鷯(�׳� �־���. ���� �ȳ��� ��Ȳ�� �߻��� �� �ִ�)
	DL_MY_HANDLER, //�ٸ� �ڵ鷯�� ��� ��ȿȭ ��Ű�� �ڽ��� ����� �ڵ鷯�� �۵��ϵ��� �Ѵ�.
	DL_MY_HANDLER_STACKOVERFLOW, //���ÿ����÷ο� �ڵ鷯�� �� �ڵ鷯���� ���� ���Ϳ� ����ϰ� �������� �۵��� ���� �ʵ��� �Ѵ�.
	DL_MAX,
};

enum enumExceptionHandler
{	
	EH_MINIDUMP = 0, //�̴ϴ��� Ŭ����
	EH_BUGTRAP, //����Ʈ��
	EH_BREAKPAD, //���� �극��ũ �е�
	EH_USER_DEFINED, //�α� �� ���� �ӽ�
	EH_BUGTRAP_MIX, //����Ʈ���� ����� ���� �ڵ鷯 Mix
	EH_MAX,
};
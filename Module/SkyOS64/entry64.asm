[BITS 64] ;64��Ʈ �ڵ�

SECTION .text
extern kmain64 ; C++ Ŀ�� ��Ʈ������Ʈ
global main64 ; PE���� ��Ʈ�� ����Ʈ

main64:	
; IA-32e ��� Ŀ�� ������ ���׸�Ʈ ��ũ���� ����
; 32��Ʈ���� 64��Ʈ�� �����ϸ鼭 Ŀ�� ���׸�Ʈ ��ũ���ʹ� 0X08�� ������ ����
	mov ax, 0x10        
	mov ds, ax          
	mov es, ax          
	mov fs, ax          
	mov gs, ax          
	
	; ���� ���� 0x500000~0x600000
	mov ss, ax
	mov rsp, 0x600000   
	mov rbp, 0x600000  

	call kmain64 
	
	jmp $
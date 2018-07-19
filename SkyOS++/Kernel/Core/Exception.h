#pragma once
#include "stdint.h"

void kHandleDivideByZero();

//�̱� ����
void kHandleSingleStepTrap();

// non maskable interrupt trap
void kHandleNMITrap();

//�극��ũ ����Ʈ
void kHandleBreakPointTrap();

//�����÷ο�
void kHandleOverflowTrap();

// bounds check
void kHandleBoundsCheckFault();

//��ȿ���� ���� OPCODE �Ǵ� ��ɾ�
void kHandleInvalidOpcodeFault();

//����̽� �̿��� �� ����
void kHandleNoDeviceFault ();

//���� ��Ʈ
void kHandleDoubleFaultAbort ();

//��ȿ���� ���� TSS(Task State Segment)
void kHandleInvalidTSSFault ();

//���׸�Ʈ �������� ����
void kHandleSegmentFault ();

//���� ��Ʈ
void kHandleStackFault();

//�Ϲ� ��ȣ ����(General Protection Fault)
void kHandleGeneralProtectionFault ();

//������ ��Ʈ
void kHandlePageFault ();

// Floating Point Unit (FPU) error
void kHandlefpu_fault ();

//alignment check
void kHandleAlignedCheckFault();

//machine check
void kHandleMachineCheckAbort ();

//Floating Point Unit (FPU) Single Instruction Multiple Data (SIMD) error
void kHandleSIMDFPUFault();

void HaltSystem(const char* errMsg);
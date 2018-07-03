!*****************************************************************************
!   SH7045�}�C�R���{�[�h�@�X�^�[�g�A�b�v���[�`��
!                                          (C) 30Nov2003 T.KOSAKA, A.YAMASHITA
!  
!  ���̃X�^�[�g�A�b�v�ōs�Ȃ��Ă��邱��
!  �i�P�j�X�^�b�N�|�C���^�̏�����
!  �i�Q�j�}���`�v���b�N�X�s���̏�����
!      �i�A�h���X�ƃ|�[�g���p�̃s�����A�h���X�Ƃ��Ďg����悤�ɂ���Ȃǁj
!  �i�R�j�ϐ��̈�ւ̏����l�̃R�s�[
!  �i�S�j�����l�������Ȃ��ϐ��̈�̃N���A�i����̓T�[�r�X�j
!  �i�T�jmain()�̌Ăяo���ƒ���ɖ������[�v�̐ݒu
!  ���̃X�^�[�g�A�b�v���[�`���t�@�C���ł̃T�[�r�X�֐�
!  �i�P�jvoid setIntMask(int mask)�@���荞�݃}�X�N�̐ݒ�֐�
!  �i�Q�jint getIntMask(void)�@���荞�݃}�X�N�̓ǂݏo���֐�
!  �i�R�j__main()�@�_�~�[�֐�
!      ������`���Ă����Ɨ\�����Ȃ����C�u�����֐����t������邱�Ƃ��Ȃ�
!*****************************************************************************
    .section    .text          ! �X�^�[�g�A�b�v���[�`����ROM�̈�ɏ�������
    .global     _start         ! �v���O�����J�n�ʒu���G�N�X�|�[�g
    .extern     _dataRAM_begin
    .extern     _dataRAM_end
    .extern     _dataROM_begin
    .extern     _bss_begin
    .extern     _bss_end

_start:                                 ! �v���O�����J�n�ʒu
!-----------------------------------------------------------------------------
! �X�^�b�N�|�C���^�ݒ�
!
! �ėp���W�X�^R0�`R15�̂���R15���n�[�h�E�F�A�X�^�b�N�|�C���^�Ɏw�肳��Ă��� 
! �A�h���X�� 0xFFFFF000�`0xFFFFFFFF �̓���RAM���g�p���邽�߁A 
! STACK_ROOT��0x0�����Ă��� 
!-----------------------------------------------------------------------------
    MOV.L    STACK_ROOT,    r15
!-----------------------------------------------------------------------------
! �e�|�[�g������
! �H�����̃}�C�R���L�b�g�ł͕W���ŊO��RAM����������Ă���Dmode2�ɃZ�b�g����
!-----------------------------------------------------------------------------
    MOV.L       BCR1,     r1         ! BCR1   -> r1
    MOV.W       D_BCR1,   r0         ! D_BCR1 -> r0
    MOV.W       r0,       @r1
    MOV.L       BCR2,     r1         ! BCR2   -> r1
    MOV.W       D_BCR2,   r0         ! D_BCR2 -> r0
    MOV.W       r0,       @r1
    MOV.L       WCR1,     r1         ! WCR1   -> r1
    MOV.W       D_WCR1,   r0         ! D_WCR1 -> r0
    MOV.W       r0,       @r1
    MOV.L       PACRH,    r1         ! PACRH   -> r1
    MOV.W       D_PACRH,  r0         ! D_PACRH -> r0
    MOV.W       r0,       @r1
    MOV.L       PACRL1,   r1         ! PACRL1   -> r1
    MOV.W       D_PACRL1, r0         ! D_PACRL1 -> r0
    MOV.W       r0,       @r1
    MOV.L       PBCR1,    r1         ! PBCR1   -> r1
    MOV.W       D_PBCR1,  r0         ! D_PBCR1 -> r0
    MOV.W       r0,       @r1
    MOV.L       PBCR2,    r1         ! PBCR2   -> r1
    MOV.W       D_PBCR2,  r0         ! D_PBCR2 -> r0
    MOV.W       r0,       @r1
    MOV.L       PCCR,     r1         ! PCCR   -> r1
    MOV.W       D_PCCR,   r0         ! D_PCCR -> r0
    MOV.W       r0,       @r1
    MOV.L       PDCRH1,   r1         ! PDCRH1   -> r1
    MOV.W       D_PDCRH1, r0         ! D_PDCRH1 -> r0
    MOV.W       r0,       @r1
    MOV.L       PDCRH2,   r1         ! PDCRH2   -> r1
    MOV.W       D_PDCRH2, r0         ! D_PDCRH2 -> r0
    MOV.W       r0,       @r1
    MOV.L       PDCRL,    r1         ! PDCRL   -> r1
    MOV.W       D_PDCRL,  r0         ! D_PDCRL -> r0
    MOV.W       r0,       @r1
    NOP

!-----------------------------------------------------------------------------
! ROM�Ɋi�[����Ă���f�[�^(�ϐ��Ȃ�)��RAM�̈�ɃR�s�[����
!   RAM_BGN  : �f�[�^���i�[����RAM�̈�̊J�n�A�h���X
!   RAM_END : �f�[�^���i�[����RAM�̈�̏I���A�h���X
!   ROM_BGN : ���ۂɃf�[�^���������܂�Ă���ROM�̈�̊J�n�A�h���X
!-----------------------------------------------------------------------------
    MOV.L       RAM_BGN,  r0        ! RAM_BGN -> r0
    MOV.L       RAM_END,  r1        ! RAM_END -> r1
    MOV.L       ROM_BGN,  r2        ! ROM_BGN -> r2
    BRA         LOOP11
    NOP
LOOP1:
    MOV.B       @r2,    r3          ! ROM�̈悩��f�[�^���擾
    MOV.B       r3,     @r0         ! RAM�̈�փf�[�^���R�s�[
    ADD         #1,     r0          ! RAM�̈���w���A�h���X���C���N�������g
    ADD         #1,     r2          ! ROM�̈���w���A�h���X���C���N�������g
LOOP11:
    CMP/eq      r0,     r1          ! RAM_BGN == RAM_END �Ȃ�� T=1
    BF          LOOP1               ! T=1 �Ȃ�� LOOP1��
    NOP
END_LOOP1:

!-----------------------------------------------------------------------------!
!   �����������̃O���[�o���ϐ���0�ŏ�����
!   BSS_BGN  : �����������̃O���[�o���ϐ����i�[����Ă���J�n�A�h���X
!   BSS_END : �����������̃O���[�o���ϐ����i�[����Ă���I���A�h���X
!-----------------------------------------------------------------------------
    MOV.L       BSS_BGN,  r0        ! BSS_BGN -> r0
    MOV.L       BSS_END,  r1        ! BSS_END -> r1
    BRA         LOOP21
    NOP
LOOP2:
    MOV         #0,     r3          ! r3=0
    MOV.B       r3,     @r0         ! �O���[�o���ϐ��̗̈��0����
    ADD         #1,     r0          ! �̈���w���A�h���X���C���N�������g
LOOP21:
    CMP/eq      r0,     r1          ! BSS_BGN == BSS_END �Ȃ�� T=1
    BF          LOOP2               ! T=1 �Ȃ�� LOOP2��
    NOP
END_LOOP2:

!-----------------------------------------------------------------------------
! main�֐��Ăяo��
!-----------------------------------------------------------------------------
START_MAIN:                         ! main��CALL
    MOV.L       MAIN_ADRS, r0       ! main�֐��̃A�h���X -> R0
    JSR         @r0                 ! main�ւ̃W�����v�T�u���[�`��
    OR          r0,     r0          ! JSR���߂͒x������̂���
                                    ! (�x������Ƃ�JSR����̖��߂��Ɏ��s)

! ������main���I�����Ė߂��Ă��Ă��������[�v�ɂ��Ē�~������ 
FOREVER:
    BRA         FOREVER
    OR          r0,     r0

.align  4                               ! 4Byte = 32Bit�Œ�
!----���s���ɎQ�Ƃ���鏉���l�L��f�[�^�̗̈�iRAM�̈�j----
RAM_BGN:            .long   _dataRAM_begin    ! �J�n�A�h���X 
RAM_END:            .long   _dataRAM_end      ! �I���A�h���X 
!----�����N���ɎQ�Ƃ���鏉���l�L��f�[�^�̏����l�̈�iROM�̈�j----
ROM_BGN:            .long   _dataROM_begin    ! �J�n�A�h���X 
!----���s���ɎQ�Ƃ���鏉���l�Ȃ��f�[�^�̗̈�iRAM�̈�j----
BSS_BGN:            .long   _bss_begin        ! �J�n�A�h���X 
BSS_END:            .long   _bss_end          ! �I���A�h���X 

STACK_ROOT:         .long   0x0               !�X�^�b�N�|�C���^�̏����l
MAIN_ADRS:          .long   _main             !main�֐�

    .align  4
BCR1:    .long 0xffff8620
BCR2:    .long 0xffff8622
WCR1:    .long 0xffff8624
PACRH:   .long 0xffff8388
PACRL1:  .long 0xffff838c
PBCR1:   .long 0xffff8398
PBCR2:   .long 0xffff839a
PCCR:    .long 0xffff839c
PDCRH1:  .long 0xffff83a8
PDCRH2:  .long 0xffff83aa
PDCRL:   .long 0xffff83ac

!----�O�t���J�E���^IC�̐ݒ�(CS0)�͂����ł��� ----
D_BCR1:    .short 0x203f ! CS2/CS3 are 16bit-bus,CS0,CS1 is 32 bit
                         ! 0010 0000 0011 1111
D_BCR2:    .short 0x5610 ! CS1/CS2/CS3 wait 1 idle,CS0 wait 2 idle
                         ! 0101 0110 0001 0000
D_WCR1:    .short 0x0002 ! CS0 is 2 wait, CS1-3 are 0 wait
                         ! 0000 0000 0000 0010
D_PACRH:   .short 0x5020 ! WRHH WRHL DRAK0 are enable
                         ! 0101 0000 0010 0000
D_PACRL1:  .short 0x5550 ! CK -RD -WRH -WRL -CS[1..0] are enable
                         ! 0101 0101 0101 0000
D_PBCR1:   .short 0x000a ! A21/A20 are enable
D_PBCR2:   .short 0xa005 ! A19/A18/A17/16 are enable
D_PCCR:    .short 0xffff ! A15-0 are enable
D_PDCRH1:  .short 0x5555 ! D31-24 are enable
D_PDCRH2:  .short 0x5555 ! D23-16 are enable
D_PDCRL:   .short 0xffff ! D15-0 are enable

! void setIntMask(int mask)   r4:mask r2:work r1:srreg
    .align  4
    .global	_setIntMask
_setIntMask:
    stc      sr,r1          ! srreg = __sr__
    mov.l    MASKVALUER,r2
    and      r2,r1          ! srreg &= 0xffffff0f
    shll2    r4             ! mask <<= 2
    shll2    r4             ! mask <<= 2
    mov.l    MASKVALUE,r2
    and      r2,r4          ! mask &= 0x00f0
    or       r4,r1          ! srreg |= mask
    ldc      r1,sr 
    rts	
    nop

! ���̊֐���IntMask��Ԃ��܂��B 
! int getIntMask(void)
    .align  4
    .global	_getIntMask
_getIntMask:
    stc      sr,r0
    mov.l    MASKVALUE,r2
    and      r2,r0
    shlr2    r0
    shlr2    r0
    rts	
    nop

    .align  4
MASKVALUE:    .long  0x000000f0
MASKVALUER:   .long  0xffffff0f

! ���̊֐��̓����J���f�t�H���g�֐��Q������̂�h���܂� 
    .align  4
    .global ___main
___main:
    rts	
    nop



.end
	
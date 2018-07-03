/*****************************************************************************
         �����J�X�N���v�g�@1Dec2003 T.kosaka A.Yamashita 

�@�B��̃v���O������f�[�^�̔z�u�i�ǂ��̃A�h���X�ɔz�u���邩�j�����߂܂�

�L�[���[�h
OUTPUT_ARCH �}�V���̃A�[�L�e�N�`��
ENTRY       �v���O�����̒��Ŏ��s����ŏ��̖��߂̈ʒu��ݒ�
MEMORY      �������u���b�N�̈ʒu�ƃT�C�Y���L�q
SECTIONS    �o�̓t�@�C���̃��������C�A�E�g���L�q

�Z�N�V�����̖��O
.vector  ���荞�݃x�N�^�e�[�u��
.text    �v���O�����@�B��R�[�h�C�萔�i�ÓI��������܂ށj
.data    �O���ϐ��Cstatic�ϐ��ŏ������i�錾���ɒl���������Ă���j�ς̕ϐ�
.bss     �O���ϐ��Cstatic�ϐ��ŏ������Ȃ��̕ϐ�
��
int aaa=1234;                   aaa��.data�Ɋm�ۂ����
int bbb;                        bbb��.bss�Ɋm�ۂ����
const int bbb1=5678;            bbb1��.text�Ɋm�ۂ����

main()
{
    int ccc;                    ccc�̓X�^�b�N�ɓ��I�Ɋm�ۂ����
      :
}

int func(void)
{
    static int ddd=0;           ddd��.data�Ɋm�ۂ����
    static int eee;             eee��.bss�Ɋm�ۂ����
    int fff;                    fff�̓X�^�b�N�ɓ��I�Ɋm�ۂ����
      :
    fff += 2345;                2345��.text�Ɋm�ۂ����
      :
}
******************************************************************************/
OUTPUT_ARCH(sh)
ENTRY("_start")
MEMORY
{
    /* ����ROM 0x00000000�`0x0003ffff */
    /* �O��RAM 0x00400000�`0x0041ffff */
    /* ����I/O 0xffff8000�`0xffff87ff */
    /* ����RAM 0xfffff000�`0xffffffff */

    vect(r)         : org = 0x00000000, len = 0x280
    dtc_vect(r)     : org = 0x00000400, len = 0x60
    rom(rx)         : org = 0x00000480, len = 256k-0x280
    ram2(rwx)       : org = 0x00400000, len = 1024k
    ram(rwx)        : org = 0xfffff000, len = 4k
}
SECTIONS
{
    .vector : {
        /* 0 */
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(0))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(0))
        /* 4 */
        LONG(DEFINED(_int_GeneralIllegalInstruction)?ABSOLUTE(_int_GeneralIllegalInstruction):ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(DEFINED(_int_SlotIllegalInstruction)?ABSOLUTE(_int_SlotIllegalInstruction):ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        /* 8 */
        LONG(ABSOLUTE(_start))
        LONG(DEFINED(_int_CPUAddressError)?ABSOLUTE(_int_CPUAddressError):ABSOLUTE(_start))
        LONG(DEFINED(_int_DMAAddressError)?ABSOLUTE(_int_DMAAddressError):ABSOLUTE(_start))
        LONG(DEFINED(_int_NMI)?ABSOLUTE(_int_NMI):ABSOLUTE(_start))
        /* 12 */
        LONG(DEFINED(_int_Userbrake)?ABSOLUTE(_int_Userbrake):ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        /* 16 */
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        /* 20 */
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        /* 24 */
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        /* 28 */
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        /* 32 */
        LONG(DEFINED(_int_trap0)?ABSOLUTE(_int_trap0):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap1)?ABSOLUTE(_int_trap1):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap2)?ABSOLUTE(_int_trap2):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap3)?ABSOLUTE(_int_trap3):ABSOLUTE(_start))
        /* 36 */
        LONG(DEFINED(_int_trap4)?ABSOLUTE(_int_trap4):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap5)?ABSOLUTE(_int_trap5):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap6)?ABSOLUTE(_int_trap6):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap7)?ABSOLUTE(_int_trap7):ABSOLUTE(_start))
        /* 40 */
        LONG(DEFINED(_int_trap8)?ABSOLUTE(_int_trap8):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap9)?ABSOLUTE(_int_trap9):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap10)?ABSOLUTE(_int_trap10):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap11)?ABSOLUTE(_int_trap11):ABSOLUTE(_start))
        /* 44 */
        LONG(DEFINED(_int_trap12)?ABSOLUTE(_int_trap12):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap13)?ABSOLUTE(_int_trap13):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap14)?ABSOLUTE(_int_trap14):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap15)?ABSOLUTE(_int_trap15):ABSOLUTE(_start))
        /* 48 */
        LONG(DEFINED(_int_trap16)?ABSOLUTE(_int_trap16):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap17)?ABSOLUTE(_int_trap17):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap18)?ABSOLUTE(_int_trap18):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap19)?ABSOLUTE(_int_trap19):ABSOLUTE(_start))
        /* 52 */
        LONG(DEFINED(_int_trap20)?ABSOLUTE(_int_trap20):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap21)?ABSOLUTE(_int_trap21):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap22)?ABSOLUTE(_int_trap22):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap23)?ABSOLUTE(_int_trap23):ABSOLUTE(_start))
        /* 56 */
        LONG(DEFINED(_int_trap24)?ABSOLUTE(_int_trap24):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap25)?ABSOLUTE(_int_trap25):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap26)?ABSOLUTE(_int_trap26):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap27)?ABSOLUTE(_int_trap27):ABSOLUTE(_start))
        /* 60 */
        LONG(DEFINED(_int_trap28)?ABSOLUTE(_int_trap28):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap29)?ABSOLUTE(_int_trap29):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap30)?ABSOLUTE(_int_trap30):ABSOLUTE(_start))
        LONG(DEFINED(_int_trap31)?ABSOLUTE(_int_trap31):ABSOLUTE(_start))
        /* 64 */
        LONG(DEFINED(_int_irq0)?ABSOLUTE(_int_irq0):ABSOLUTE(_start))
        LONG(DEFINED(_int_irq1)?ABSOLUTE(_int_irq1):ABSOLUTE(_start))
        LONG(DEFINED(_int_irq2)?ABSOLUTE(_int_irq2):ABSOLUTE(_start))
        LONG(DEFINED(_int_irq3)?ABSOLUTE(_int_irq3):ABSOLUTE(_start))
        /* 68 */
        LONG(DEFINED(_int_irq4)?ABSOLUTE(_int_irq4):ABSOLUTE(_start))
        LONG(DEFINED(_int_irq5)?ABSOLUTE(_int_irq5):ABSOLUTE(_start))
        LONG(DEFINED(_int_irq6)?ABSOLUTE(_int_irq6):ABSOLUTE(_start))
        LONG(DEFINED(_int_irq7)?ABSOLUTE(_int_irq7):ABSOLUTE(_start))
        /* 72 */
        LONG(DEFINED(_int_dei0)?ABSOLUTE(_int_dei0):ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        /* 76 */
        LONG(DEFINED(_int_dei1)?ABSOLUTE(_int_dei1):ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        /* 80 */
        LONG(DEFINED(_int_dei2)?ABSOLUTE(_int_dei2):ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        /* 84 */
        LONG(DEFINED(_int_dei3)?ABSOLUTE(_int_dei3):ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        /* 88 */
        LONG(DEFINED(_int_tgi0a)?ABSOLUTE(_int_tgi0a):ABSOLUTE(_start))
        LONG(DEFINED(_int_tgi0b)?ABSOLUTE(_int_tgi0b):ABSOLUTE(_start))
        LONG(DEFINED(_int_tgi0c)?ABSOLUTE(_int_tgi0c):ABSOLUTE(_start))
        LONG(DEFINED(_int_tgi0d)?ABSOLUTE(_int_tgi0d):ABSOLUTE(_start))
        /* 92 */
        LONG(DEFINED(_int_tci0v)?ABSOLUTE(_int_tci0v):ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        /* 96 */
        LONG(DEFINED(_int_tgi1a)?ABSOLUTE(_int_tgi1a):ABSOLUTE(_start))
        LONG(DEFINED(_int_tgi1b)?ABSOLUTE(_int_tgi1b):ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        /* 100 */
        LONG(DEFINED(_int_tci1v)?ABSOLUTE(_int_tci1v):ABSOLUTE(_start))
        LONG(DEFINED(_int_tci1u)?ABSOLUTE(_int_tci1u):ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        /* 104 */
        LONG(DEFINED(_int_tgi2a)?ABSOLUTE(_int_tgi2a):ABSOLUTE(_start))
        LONG(DEFINED(_int_tgi2b)?ABSOLUTE(_int_tgi2b):ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        /* 108 */
        LONG(DEFINED(_int_tci2v)?ABSOLUTE(_int_tci2v):ABSOLUTE(_start))
        LONG(DEFINED(_int_tci2u)?ABSOLUTE(_int_tci2u):ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        /* 112 */
        LONG(DEFINED(_int_tgi3a)?ABSOLUTE(_int_tgi3a):ABSOLUTE(_start))
        LONG(DEFINED(_int_tgi3b)?ABSOLUTE(_int_tgi3b):ABSOLUTE(_start))
        LONG(DEFINED(_int_tgi3c)?ABSOLUTE(_int_tgi3c):ABSOLUTE(_start))
        LONG(DEFINED(_int_tgi3d)?ABSOLUTE(_int_tgi3d):ABSOLUTE(_start))
        /* 116 */
        LONG(DEFINED(_int_tci3v)?ABSOLUTE(_int_tci3v):ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        /* 120 */
        LONG(DEFINED(_int_tgi4a)?ABSOLUTE(_int_tgi4a):ABSOLUTE(_start))
        LONG(DEFINED(_int_tgi4b)?ABSOLUTE(_int_tgi4b):ABSOLUTE(_start))
        LONG(DEFINED(_int_tgi4c)?ABSOLUTE(_int_tgi4c):ABSOLUTE(_start))
        LONG(DEFINED(_int_tgi4d)?ABSOLUTE(_int_tgi4d):ABSOLUTE(_start))
        /* 124 */
        LONG(DEFINED(_int_tci4v)?ABSOLUTE(_int_tci4v):ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        /* 128 */
        LONG(DEFINED(_int_eri0)?ABSOLUTE(_int_eri0):ABSOLUTE(_start))
        LONG(DEFINED(_int_rxi0)?ABSOLUTE(_int_rxi0):ABSOLUTE(_start))
        LONG(DEFINED(_int_txi0)?ABSOLUTE(_int_txi0):ABSOLUTE(_start))
        LONG(DEFINED(_int_tei0)?ABSOLUTE(_int_tei0):ABSOLUTE(_start))
        /* 132 */
        LONG(DEFINED(_int_eri1)?ABSOLUTE(_int_eri1):ABSOLUTE(_start))
        LONG(DEFINED(_int_rxi1)?ABSOLUTE(_int_rxi1):ABSOLUTE(_start))
        LONG(DEFINED(_int_txi1)?ABSOLUTE(_int_txi1):ABSOLUTE(_start))
        LONG(DEFINED(_int_tei1)?ABSOLUTE(_int_tei1):ABSOLUTE(_start))
        /* 136 */
        LONG(DEFINED(_int_adi0)?ABSOLUTE(_int_adi0):ABSOLUTE(_start))
        LONG(DEFINED(_int_adi1)?ABSOLUTE(_int_adi1):ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        /* 140 */
        LONG(DEFINED(_int_swdtce)?ABSOLUTE(_int_swdtce):ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        /* 144 */
        LONG(DEFINED(_int_cmi0)?ABSOLUTE(_int_cmi0):ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        /* 148 */
        LONG(DEFINED(_int_cmi1)?ABSOLUTE(_int_cmi1):ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        /* 152 */
        LONG(DEFINED(_int_iti)?ABSOLUTE(_int_iti):ABSOLUTE(_start))
        LONG(DEFINED(_int_cmi)?ABSOLUTE(_int_cmi):ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        LONG(ABSOLUTE(_start))
        /* 156 */
        LONG(DEFINED(_int_oei)?ABSOLUTE(_int_oei):ABSOLUTE(_start))
        FILL(0xff)
    } > vect                         /*�����܂ł�vect�̈�Ɋ���t����*/
    .dtc : {
	/* 0x400 - 0x460*/
	. += 0x003c;
        SHORT(DEFINED(_DTC1)?ABSOLUTE((_DTC1)&0xffff):ABSOLUTE(_start))
    } > dtc_vect
    .text : {
        CREATE_OBJECT_SYMBOLS
        *(.text)                     /*���ׂẴ\�[�X�t�@�C����.text�Z�N�V������������*/
        _dataROM_begin = .;          /*�ϐ�_dataROM_begin�ɃJ�����g�̃A�h���X����*/
    }  > rom                         /*�����܂ł�rom�̈�Ɋ���t����*/
    .data : AT (_dataROM_begin) {    /*AT �����l�f�[�^��_dataROM_begin�ɏ�������*/
        _dataRAM_begin = .;          /*�ϐ�_dataRAM_begin�ɃJ�����g�̃A�h���X����*/
        *(.data)                     /*���ׂẴ\�[�X�t�@�C����.data�Z�N�V������������*/
        _dataRAM_end = .;            /*�ϐ�_dataRAM_end�ɃJ�����g�̃A�h���X����*/
    } > ram2                         /*�����܂ł�ram2�̈�Ɋ���t����*/
    .bss : {
        _bss_begin = .;              /*�ϐ�_bss_begin�ɃJ�����g�̃A�h���X����*/
        *(.bss)                      /*���ׂẴ\�[�X�t�@�C����.bss�Z�N�V������������*/
        _bss_end = .;                /*�ϐ�_bss_end�ɃJ�����g�̃A�h���X����*/
    }  >ram2                         /*�����܂ł�ram2�̈�Ɋ���t����*/
}

/*******************************************************************************
�⑫����

�����N��Ƃɂ�莟�̊e�ϐ��i5�j�̒l����܂�܂��B�����̕ϐ��̓X�^�[�g�A�b�v��
�[�`���ŗ��p����܂��B
_dataROM_begin    .data�Z�N�V�����̃������C���[�W�ŏ����l�����Ԑ擪�A�h���X
                  ���̗̈悪���[�U�v���O�������s���ɎQ�Ƃ���邱�Ƃ͂Ȃ�
_dataRAM_begin    .data�Z�N�V�����̐擪�A�h���X
_dataRAM_end      .data�Z�N�V�����̖����A�h���X�{�P
_bss_begin        .bss�Z�N�V�����̐擪�A�h���X
_bss_end          .bss�Z�N�V�����̖����A�h���X�{�P

���������ꂽ�O���[�o���ϐ��i��̗�ł�int aaaa=1234;�j��RAM��ɔz�u�����ƁC�d
�����؂ꂽ��Ԃł͏����l1234���o���Ă������Ƃ��o���Ȃ��B�܂�ROM��ɔz�u�����ƁC
���[�U�v���O�����Œl��ύX���邱�Ƃ��o���Ȃ��B
�����ł��̂悤�ȕϐ��́C���s�v���O�����ł�RAM�̈�ɂ��̕ϐ�������悤�ɋ@�B��R
�[�h������Ă����C�����l��ROM�̈�ɏ������ނ悤�ɂȂ�B�����ϐ��̂߂��芄�蓖��
���d�����邽�߂ɁuAT�v�R�}���h���g����B
ROM�̈�ɏ������܂ꂽ�����l�����[�U�v���O�����������o���O��RAM�̈�ɃR�s�[���邱
�Ƃ̓X�^�[�g�A�b�v���[�`���̎d���̂P�ł��B
*******************************************************************************/
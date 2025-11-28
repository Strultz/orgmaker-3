
#define MAXNOTELENGTH	255//音符の最大長
#define MAXMELODY		8
#define MAXDRAM			8
#define MAXTRACK		16

#define KEYDUMMY		255//キーの変更は加えない音符
#define PANDUMMY		255//パンの変更は加えない音符
#define VOLDUMMY		255//ボリュームの変更は加えない音符
//曲情報をセットする時のフラグ
#define SETALL		0xffffffff//全てをセット
#define SETWAIT		0x00000001
#define SETGRID		0x00000002
#define SETALLOC	0x00000004
#define SETREPEAT	0x00000008
#define SETFREQ		0x00000010
#define SETWAVE		0x00000020
#define SETPIPI		0x00000040


#define ALLOCNOTE	4096
#define MODEPARADD	0
#define MODEPARSUB	1
#define MODEMULTIPLY 10 //2014.04.30 A
#define MODEDECAY   64  //2014.05.01 A

#define PF_MIX_PASTE  0x1
#define PF_PASTE_NOTE 0x2
#define PF_PASTE_VOL  0x4
#define PF_PASTE_PAN  0x8
#define PF_PASTE_ALL (PF_PASTE_NOTE | PF_PASTE_VOL | PF_PASTE_PAN)

#define OWM_UPDATESTATUS (WM_USER + 1)
#define OWM_TBCONTEXTMENU (WM_USER + 2)


//#define NUMDRAMITEM		39	//１２＋５＋９＋１(猫)＋１１	    // 2011.10.17 D
#define NUMDRAMITEM		42	//１２＋５＋９＋１(猫)＋１１＋３	// 2011.10.17 A
//#define NUMDRAMITEM		43	//１２＋５＋９＋１(猫)＋１１＋３＋１    // 2024.08.24 A

#define MAXHORZMEAS	999 // Increased from 256 to 999


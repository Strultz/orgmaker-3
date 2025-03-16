
#define MAXNOTELENGTH	255//‰¹•„‚ÌÅ‘å’·
#define MAXMELODY		8
#define MAXDRAM			8
#define MAXTRACK		16

#define KEYDUMMY		255//ƒL[‚Ì•ÏX‚Í‰Á‚¦‚È‚¢‰¹•„
#define PANDUMMY		255//ƒpƒ“‚Ì•ÏX‚Í‰Á‚¦‚È‚¢‰¹•„
#define VOLDUMMY		255//ƒ{ƒŠƒ…[ƒ€‚Ì•ÏX‚Í‰Á‚¦‚È‚¢‰¹•„
//‹Èî•ñ‚ğƒZƒbƒg‚·‚é‚Ìƒtƒ‰ƒO
#define SETALL		0xffffffff//‘S‚Ä‚ğƒZƒbƒg
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


//#define NUMDRAMITEM		39	//‚P‚Q{‚T{‚X{‚P(”L){‚P‚P	    // 2011.10.17 D
#define NUMDRAMITEM		42	//‚P‚Q{‚T{‚X{‚P(”L){‚P‚P{‚R	// 2011.10.17 A
//#define NUMDRAMITEM		43	//‚P‚Q{‚T{‚X{‚P(”L){‚P‚P{‚R{‚P    // 2024.08.24 A

#define MAXHORZMEAS	999 // Increased from 256 to 999



#define VER_VS(x) #x
#define VER_S(x) VER_VS(x)

#define VER_MAJOR 3
#define VER_MINOR 0
#define VER_PATCH 10
#define VER_PRE   -1 // set as -1 if not prerelease

#if defined(_DEBUG)
#define VER_STRING VER_S(VER_MAJOR) "." VER_S(VER_MINOR) "." VER_S(VER_PATCH) "-dev"
#define VER_IS_PRERELEASE true
#elif VER_PRE != -1
#define VER_STRING VER_S(VER_MAJOR) "." VER_S(VER_MINOR) "." VER_S(VER_PATCH) "-pre" VER_S(VER_PRE)
#define VER_IS_PRERELEASE true
#else
#define VER_STRING VER_S(VER_MAJOR) "." VER_S(VER_MINOR) "." VER_S(VER_PATCH)
#define VER_IS_PRERELEASE false
#endif

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

#define OWM_UPDATESTATUS (WM_USER + 1)


//#define NUMDRAMITEM		39	//‚P‚Q{‚T{‚X{‚P(”L){‚P‚P	    // 2011.10.17 D
#define NUMDRAMITEM		42	//‚P‚Q{‚T{‚X{‚P(”L){‚P‚P{‚R	// 2011.10.17 A
//#define NUMDRAMITEM		43	//‚P‚Q{‚T{‚X{‚P(”L){‚P‚P{‚R{‚P    // 2024.08.24 A

#define MAXHORZMEAS	999 // Increased from 256 to 999


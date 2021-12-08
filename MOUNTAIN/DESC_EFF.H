

 #define _FR_ 



#ifdef _FR_

#define T_TITR_FMVT "Flou de mouvement..."
#define T_TITR_NETB "Noir et Blanc"
#define T_TITR_SNTB "Noir et Blanc strict..."
#define T_TITR_INVV "Invers‚ Video"
#define T_TITR_MIRH "Miroir horizontal"
#define T_TITR_MIRV "Miroir vertical"
#define T_TITR_WOBV "Wobbler vertical..."
#define T_TITR_DIFF "Diffusion"
#define T_TITR_RVBC "Control RVB..."
#define T_TITR_ROTS "Rotation/Zoom..."
#define T_TITR_SPIR "Spirale..."

TRAN_DESC_PARAM desc_eparam_sntb =
{	1,0,{"Seuil de passage","",
	""}, 0, {1,0,0}, {255, 0, 0}, 
	{128,0,0}, 2, {"Couleur 1","Couleur 2"}, {{0,0,0},{255,255,255}},0,{"","","",""},
	0,0,{"",""},{{0,0},{0,0}}
};
TRAN_DESC_PARAM desc_eparam_fmvt =
{	1,0,{"Intensit‚","",
	""}, 0, {0,0,0}, {1024, 0, 0}, 
	{512,0,0}, 0, {"",""}, {{0,0,0},{255,255,255}},0,{"","","",""},
	0,0,{"",""},{{0,0},{0,0}}
};
TRAN_DESC_PARAM desc_eparam_wobv =
{	3,0,{"Courbe gauche","Courbe droite",
	"D‚calage"}, 0, {0,0,0}, {10000, 10000, 10000}, 
	{0,0,0}, 1, {"Fond",""}, {{0,0,0},{0,0,0}},0,{"","","",""},
	0,0,{"",""},{{0,0},{0,0}}
};
TRAN_DESC_PARAM desc_eparam_rvbc =
{	3,0,{"Rouge","Vert",
	"Bleu"}, 0, {0,0,0}, {1024, 1024, 1024}, 
	{512,512,512}, 0, {"",""}, {{0,0,0},{255,255,255}},0,{"","","",""},
	0,0,{"",""},{{0,0},{0,0}}
};
TRAN_DESC_PARAM desc_eparam_rots =
{	2,0,{"Angle en degr‚s","Zoom en pourcent",
	""}, 0, {-360,0,0}, {360, 500, 0}, 	
	{10,100,0}, 1, {"Fond",""}, {{0,0,0},{255,255,255}},2,{"Mode rapide","Mosa‹que","",""},
	1,1,{"Centre",""},{{500,500},{500,500}}
};
TRAN_DESC_PARAM desc_eparam_spir =
{	1,0,{"Angle en degr‚s","",
	""}, 0, {-360,0,0}, {360, 0, 0}, 	
	{100,0,0}, 0, {"",""}, {{0,0,0},{255,255,255}},0,{"","","",""},
	0,1,{"Centre",""},{{500,500},{500,500}}
};

#else		/* en anglais */

#define T_TITR_FMVT "Motion blur..."
#define T_TITR_NETB "Black and white"
#define T_TITR_SNTB "Strict Black and white..."
#define T_TITR_INVV "Video invertion"
#define T_TITR_MIRH "Horizontal mirror"
#define T_TITR_MIRV "Vertical mirror"
#define T_TITR_WOBV "Vertical Wobbler..."
#define T_TITR_DIFF "Diffusion"
#define T_TITR_RVBC "RGB Control..."
#define T_TITR_ROTS "Rotation/Zoom..."
#define T_TITR_SPIR "Spiral..."

TRAN_DESC_PARAM desc_eparam_sntb =
{	1,0,{"Pass value","",
	""}, 0, {1,0,0}, {255, 0, 0}, 
	{128,0,0}, 2, {"Color 1","Color 2"}, {{0,0,0},{255,255,255}},0,{"","","",""},
	0,0,{"",""},{{0,0},{0,0}}
};
TRAN_DESC_PARAM desc_eparam_fmvt =
{	1,0,{"Intensity","",
	""}, 0, {0,0,0}, {1024, 0, 0}, 
	{512,0,0}, 0, {"",""}, {{0,0,0},{255,255,255}},0,{"","","",""},
	0,0,{"",""},{{0,0},{0,0}}
};
TRAN_DESC_PARAM desc_eparam_wobv =
{	3,0,{"Left Curve","Right Curve",
	"Shift"}, 0, {0,0,0}, {10000, 10000, 10000}, 
	{0,0,0}, 1, {"Background",""}, {{0,0,0},{0,0,0}},0,{"","","",""},
	0,0,{"",""},{{0,0},{0,0}}
};
TRAN_DESC_PARAM desc_eparam_rvbc =
{	3,0,{"Red","Green",
	"Blue"}, 0, {0,0,0}, {1024, 1024, 1024}, 
	{512,512,512}, 0, {"",""}, {{0,0,0},{255,255,255}},0,{"","","",""},
	0,0,{"",""},{{0,0},{0,0}}
};
TRAN_DESC_PARAM desc_eparam_rots =
{	2,0,{"Angle in degree","Zoom in percent",
	""}, 0, {-360,0,0}, {360, 500, 0}, 
	{10,100,0}, 1, {"Background",""}, {{0,0,0},{255,255,255}},2,{"Fast mode","Pattern mode","",""},
	1,1,{"Center",""},{{500,500},{500,500}}
};
TRAN_DESC_PARAM desc_eparam_spir =
{	1,0,{"Angle in degree","",
	""}, 0, {-360,0,0}, {360, 0, 0}, 	
	{100,0,0}, 0, {"",""}, {{0,0,0},{255,255,255}},0,{"","","",""},
	0,1,{"Center",""},{{500,500},{500,500}}
};

#endif


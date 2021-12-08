typedef struct
{
	unsigned LONG add_RGB;       /* adresse du buffer image au format natif RGB 8bits 
															par couleur si -1L forcer … reconnaitre l'‚cran
															pas d'affichage retour des paramˆtres dans la structure
															et typepixel rempli (pr‚voir 15 octets) */
	unsigned WORD largeur_RGB;   /* largeur de l'image RGB */
	unsigned WORD hauteur_RGB;   /* hauteur de l'image RGB */
	unsigned LONG add_ECRAN;     /* adresse ‚cran ou m‚moire si NULL automatique ‚cran */
	unsigned WORD largeur_ECRAN; /* largeur de l'‚cran en pixels si 0 automatique*/
	unsigned WORD hauteur_ECRAN; /* hauteur de l'‚cran en pixels si 0 automatique*/
	LONG octets;  /* nombres d'octets de d‚calage en fin de ligne sur la plupart des 
								cartes = 0*/
	unsigned WORD  position_X;   /* position sur l'‚cran en pixels selon X de d‚part de recopie */
	unsigned WORD position_Y;	   /* position sur l'‚cran en pixels selon Y de d‚part de recopie */
	unsigned WORD X_voulu;       /* position dans l'image en pixels selon X de d‚part de recopie */
	unsigned WORD Y_voulu;       /* position dans l'image en pixels selon Y de d‚part de recopie */
	unsigned WORD largeur_voulue;/* largeur en pixel de recopie */
	unsigned WORD hauteur_voulue;/* hauteur en pixels de recopie */
	unsigned LONG add_palette_SP;/* adresse d'un tableau de 255 valeurs des correspondances
	  				de couleur pour le cas sans palette hard */
} videoinf;

typedef struct
{
	unsigned LONG chainepix;  /* adresse de la chaine de caractŠre d‚crivant un pixel */
	unsigned char retour1;    /* 0 oK, 0xFF ‚cran non support‚ */ 
	unsigned char retour2;    /* 0xFF routine g‚n‚rale lente */
	unsigned WORD version;    /* num‚ro de version */
	unsigned LONG palettecouleur; /* adresse palette couleur */
	unsigned LONG palettegris;    /* adresse palette grise */
} sortievideo;

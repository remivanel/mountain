/* Routine Fselect */

typedef struct 
{	char	chemin[110];	/* sans le masque */
	char	complet[125];	/* le tout */
	char	masque[15];		/* pour les extenstions */
} CHEMIN;

int Fselecteur(CHEMIN *ptr,char *pth,char *name,int *bouton,char *comment);


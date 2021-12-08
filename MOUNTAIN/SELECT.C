#include <aes.h>
#include <string.h>
#include "select.h"


/*------------------------------*/
/*NOM: Fselect                  */
/*OBJET: FacilitÇ la gestion du */
/* selecteur de fichier         */
/*                              */
/*PARAMETRES:                   */
/*En entrÇ:                     */
/* char *comment: titre du      */
/* selecteur.                   */
/*En sortie:                    */
/* CHEMIN *ptr:pointeur sur une */
/* stucture chemin qui sera     */
/* rempli par la fonction       */
/* int *bouton: bouton appuyÇ:	*/
/* 0 = Annule 1 = OK            */ 
/*En entrÇ-sortie:              */
/* char *name: nom du fichier   */
/* char *pth: chemin proposÇ,   */
/*  suivit du masque,           */
/* si pth = "", alors chemin par*/
/* defaut + *.*                 */
/* si pth = "\\*.xxx", chm par  */
/* defaut + *.xxx               */
/*                              */
/* VALEUR RENVOYêE:             */
/* 0 si erreur                  */
/* Auteur : VANEL RÇmi          */
/*------------------------------*/
int     Dgetdrv( void );
int     Dgetpath( char *path, int driveno );
long  Super( void *stack );


int Fselecteur(CHEMIN *ptr,char *pth,char *name,int *bouton,char *comment)
{	int retour;
	unsigned long len;
	char pth2[150],ext[14];
	int version_tos;
	char		**sysbase=(char **)0x4f2;		/* pointeur vers le debut du
											systeme d'exploitation	*/
	char		*systeme;												
	long old_stack;
	
	if(pth[0] == '\\')
	{	strcpy(ext,pth);
		pth[0] = Dgetdrv() + 'A';
		pth[1] = 0;		/* sale bug vicelare !! */
		strcat(pth,":");
		Dgetpath(pth2,0);
		strcat(pth,pth2);
		strcat(pth,ext);
	} else if (pth[0] == 0)
	{	pth[0] = Dgetdrv() + 'A';
		pth[1] = 0;		/* sale bug vicelare !! */
		strcat(pth,":");
		Dgetpath(pth2,0);
		strcat(pth,pth2);
		strcat(pth,"\\*.*");
	}
	
	if (Super((void *)1L)==0L)	/* si on est en mode utilisateur	*/
	{		
		old_stack=Super(0L);	/* on passe en mode superviseur	*/

	}
	systeme=*sysbase;			/* debut du systeme d'exploitation	*/

	if (old_stack)				/* si on etait en utilisateur au debut	*/
	{
		Super((void *)old_stack);	/*  on y retourne */
	}				

	version_tos = *(int *)(systeme+2);
	
	if(version_tos < 0x160)
		retour = fsel_input(pth,name,bouton);
	else
		retour = fsel_exinput(pth,name,bouton,comment);
	len = strlen(pth);
	do 
	{	--len;
	} while (pth[len] != '\\');
	strcpy(ptr->chemin,pth);
	ptr->chemin[len + 1] = '\0';
	
	strcpy(ptr->complet,ptr->chemin);
	strcat(ptr->complet,name);
	
	strcpy(ptr->masque,&pth[len + 1]);
	return retour;
}


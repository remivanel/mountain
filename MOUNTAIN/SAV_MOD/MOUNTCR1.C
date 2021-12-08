/* module Mountain pour version enregistree */

#include <stdio.h>
#include <stdlib.h>
#include <tos.h>
#include "..\windform.h"  /* et vdi.h et aes.h */
#include <string.h>
#include "..\some_fnc.h"
#include "..\modules.h"
#include "..\export.h"
#include "..\mountain.h"
#include "..\preview.h"
#include "..\projet.h"
#include "..\transit.h"
#include "..\geffets.h"
#include "..\pistes.h"
#include "..\effets1.h"
#include "..\txts.h"
#include "sav_mod.h"

void ext_exp_mplayer(EXT_MPL_EXP *ext_mpl_exp)
{	int x,y;
	char chemin_bat[200],cmdline[250];
	int format,qualite;
	FILE *fp_bat;
	
	qualite=ext_mpl_exp->qualite;
	format=ext_mpl_exp->format;

	ext_mpl_exp->mpl_para->format = -1;

	x=ext_mpl_exp->projet->x;
	y=ext_mpl_exp->projet->y;
	
	ext_mpl_exp->mpl_para->ptr_img = ext_mpl_exp->img[0];
	
	strcpy(chemin_bat,ext_mpl_exp->chemin_key);
	strcat(chemin_bat,"MOUNTAIN.BAT");
	
	fp_bat = fopen(chemin_bat,"w");
	if(fp_bat == NULL)
	{	(ext_mpl_exp->super_alert)(1,3,T_IMPOS_OPEN_BAT);
		Mfree(ext_mpl_exp->img[0]);
		Mfree(ext_mpl_exp->img[1]);
		return ;
	}
	fprintf(fp_bat,"m_player\n");
	fprintf(fp_bat,"w=%d\n",x);
	fprintf(fp_bat,"h=%d\n",y);
	if(format == 1)
		fprintf(fp_bat,"c=tga2\n",y);
	else
		fprintf(fp_bat,"c=ximg\n",y);
	fprintf(fp_bat,"b=%ld\n",(long)x*y*2L+18L);
	fprintf(fp_bat,"t=%d\n",200/ext_mpl_exp->projet->speed);
	if(ext_mpl_exp->fic_son)
		fprintf(fp_bat,"s=%s.MOV\n",ext_mpl_exp->fic_son);
	fprintf(fp_bat,"k=%d\n",ext_mpl_exp->freq_key);
	fprintf(fp_bat,"q=%d\n",qualite);
	fprintf(fp_bat,"f=%ld\n",ext_mpl_exp->nb_img);
	if(format == 3)
		fprintf(fp_bat,"o=#%s.AVI\n",ext_mpl_exp->nom_fic);
	else
		fprintf(fp_bat,"o=%s.MOV\n",ext_mpl_exp->nom_fic);
	fprintf(fp_bat,"m=%ld\n",ext_mpl_exp->mpl_para);
	fprintf(fp_bat,"data\ndum0000.tga\n.rept %ld\n.incr\n.disp\n.endr\n.stop\n",ext_mpl_exp->nb_img-1);
	fclose(fp_bat);

	
	/* Dans le dossier de M player ... */

	Dsetdrv(ext_mpl_exp->chemin_mplayer[0]-'A');
	strcpy(cmdline,ext_mpl_exp->chemin_mplayer);
	(ext_mpl_exp->find_path)(cmdline);
	Dsetpath(cmdline);

	sprintf(cmdline,".-d +a %s",chemin_bat);
	
	/* ... Se trouve M Player */
	(ext_mpl_exp->Save_stack)();
	Pexec(0,ext_mpl_exp->chemin_mplayer,cmdline,"");
	graf_mouse(ARROW,0);
}

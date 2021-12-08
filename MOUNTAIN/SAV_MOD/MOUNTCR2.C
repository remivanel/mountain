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

void ext_mod_export(EXT_EXP_MOD *ext_exp_mod)
{	
	Dsetdrv(ext_exp_mod->projet->chemin[0] - 'A');
	strcpy(ext_exp_mod->chemin,ext_exp_mod->projet->chemin);
	(ext_exp_mod->find_path)(ext_exp_mod->chemin);
	Dsetpath(ext_exp_mod->chemin);
	
	
	ext_exp_mod->exp_init_param->name=ext_exp_mod->nom_anim;
	ext_exp_mod->exp_init_param->x=ext_exp_mod->projet->x;
	ext_exp_mod->exp_init_param->y=ext_exp_mod->projet->y;
	ext_exp_mod->exp_init_param->fps=ext_exp_mod->projet->speed;
	ext_exp_mod->exp_init_param->nb_img=ext_exp_mod->nb_img;		
	ext_exp_mod->exp_init_param->size_buf=0;
	ext_exp_mod->exp_init_param->param_adr=ext_exp_mod->adr_param;
	ext_exp_mod->exp_init_param->work_adr=ext_exp_mod->work_adr;

}
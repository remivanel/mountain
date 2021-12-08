/********************MOUTAIN********************/
/*****        GESTION des Animations        ****/
/*****             Version 0.1              ****/
/*****            Par Vanel R‚mi            ****/
/***********************************************/


#include <tos.h>
#include "windform.h"
#include <string.h>
#include <stdio.h>
#include "flh_lib.h"
#include "gest_flh.h"
#include "gest_fli.h"
#include "mountain.h"
#include "objets.h"
#include "anim.h"
#include "modules.h"
#include "some_fnc.h"
#include "txts.h"

ANIM_PAR anim_par[MAX_ANIM_PAR];
int nb_canal;
static int *buffer;
static long taille_buffer;
static int reserved_buf=0;		/* il y a un buffer resrver de taille_buffer */

extern OBJET objet[MAX_OBJET];
extern int *pal;
extern IMP_MOD imp_mod[10];

int open_anim(int num_objet)
{	int i,canal=-1,mod;
	long taille;
	INIT_PARAM para;
	
	/* trouve un canal libre */
	for(i=0;i<MAX_ANIM_PAR;i++)
	{	if(anim_par[i].used == 0)
		{	canal = i;
			goto suite;
		}
	}
	
	suite:;

	if(canal == -1)
	{	super_alert(1,1,T_NO_MORE_CANAL);
		return -1;
	}
	
	
	if(objet[num_objet].type == OBJ_FLH || objet[num_objet].type == OBJ_FLI || objet[num_objet].type == OBJ_FLC)
	{	
		anim_par[canal].handle = (int)Fopen(objet[num_objet].chemin,FO_READ);
		if(anim_par[canal].handle < 0)
		{	super_alert(1,3,T_FILE_ERROR);
			return -1;
		}
		
		anim_par[canal].hnd_key = (int)Fopen(objet[num_objet].chemin_key,FO_READ);
		if(anim_par[canal].hnd_key < 0)
		{	super_alert(1,3,T_FILE_ERROR);
			Fclose(anim_par[canal].handle);
			return -1;
		}
		
		Fread(anim_par[canal].hnd_key,SIZE_HKEY,&anim_par[canal].key);
		anim_par[canal].offset_flh = Malloc(anim_par[canal].key.nb_img_in_flh*4);
		if(anim_par[canal].offset_flh == 0)
		{	super_alert(1,3,T_NO_MEMORY);
			Fclose(anim_par[canal].hnd_key);
			Fclose(anim_par[canal].handle);
			return -1;
		}
		
		Fread(anim_par[canal].hnd_key,anim_par[canal].key.nb_img_in_flh*4,anim_par[canal].offset_flh);
		Fseek(anim_par[canal].key.table_offset,anim_par[canal].hnd_key,0);
		
		if(anim_par[canal].key.nb_img_in_key)		/* au moin 1 key */
		{	anim_par[canal].offset_key = Malloc(anim_par[canal].key.nb_img_in_key*4);
			if(anim_par[canal].offset_key == 0)
			{	super_alert(1,3,T_NO_MEMORY);
				Mfree(anim_par[canal].offset_flh);
				Fclose(anim_par[canal].hnd_key);
				Fclose(anim_par[canal].handle);
				return -1;
			}
		
			Fread(anim_par[canal].hnd_key,anim_par[canal].key.nb_img_in_key*4,anim_par[canal].offset_key);
		}
		
		/* reserve un buffer au moins aussi grand que l'image */
		if(new_buffer((long)objet[num_objet].x * objet[num_objet].y * 6L)/2)
		{	super_alert(1,3,T_NO_MEMORY);
			Mfree(anim_par[canal].offset_flh);
			Fclose(anim_par[canal].hnd_key);
			Fclose(anim_par[canal].handle);
			return -1;
		}

	}else if(objet[num_objet].type == OBJ_MOD)
	{	anim_par[canal].handle = (int)Fopen(objet[num_objet].chemin,FO_READ);
		if(anim_par[canal].handle < 0)
		{	super_alert(1,3,T_FILE_ERROR);
			return -1;
		}
		/* reserve son espace donnees */
		mod = find_module(objet[num_objet].module_id);
		if(mod == -1)
		{	super_alert(1,3,T_NO_MATCH_MOD);
			Fclose(anim_par[canal].handle);
			return -1;
		}
		
		anim_par[canal].mod = mod;		/* une fois pour tt */
		
		if(imp_mod[mod].size_buf_para)
		{	anim_par[canal].work_adr = Malloc(imp_mod[mod].size_buf_para);
			if(!anim_par[canal].work_adr)
			{	super_alert(1,3,T_NO_MEM_OPEN_ANIM);
				Fclose(anim_par[canal].handle);
				return -1;
			}
		}
		taille = Fseek(0,anim_par[canal].handle,2);
		Fseek(0,anim_par[canal].handle,0);
		para.size_fic = taille;
		para.handle = anim_par[canal].handle;
		para.work_adr = anim_par[canal].work_adr;
		
		(imp_mod[mod].init_imp)(&para);
		if(para.retour == -1)
		{	super_alert(1,3,T_OPEN_ERROR);
			Mfree(anim_par[canal].work_adr);
			Fclose(anim_par[canal].handle);
			return -1;
		}

		/* reserve un buffer au moins aussi grand que ce que le module demande */
		if(new_buffer(para.size_buf))
		{	super_alert(1,3,T_NO_MEM_OPEN_ANIM);
			Mfree(anim_par[canal].work_adr);
			Fclose(anim_par[canal].handle);
			return -1;
		}
	}else if(objet[num_objet].type == OBJ_SUIT)
	{	anim_par[canal].mod = find_module(objet[num_objet].module_id);
	}
	
	nb_canal++;

	anim_par[canal].used = 1;
	anim_par[canal].objet = num_objet;

	form_mem(UPDT_MEM);
	
	return canal;
}

int get_frame(int canal,int frame,int old_frame, int *img, int x_img)
{	IMG_PARAM para;
	INIT_PARAM ini_para;
	char fic[160];
	int mod,num_objet;
	long taille,frame2;
	
	frame = max(0,frame);	/* pour ne pas avoir de frame negative */
	frame = min(objet[anim_par[canal].objet].nb_img-1,frame);	/* pour ne pas avoir de frame > nb_img */
	old_frame = min(objet[anim_par[canal].objet].nb_img-1,old_frame);	/* pour ne pas avoir de frame > nb_img */

	if(old_frame == frame)		/* optimisation de feu */
		return 1;

	if(objet[anim_par[canal].objet].type == OBJ_FLH)
	{	return  get_in_flh(frame,old_frame, anim_par[canal].handle, anim_par[canal].hnd_key, buffer, img,
							 x_img, &anim_par[canal].key, anim_par[canal].offset_flh,
                   	anim_par[canal].offset_key, taille_buffer);
	}else if(objet[anim_par[canal].objet].type == OBJ_FLI || objet[anim_par[canal].objet].type == OBJ_FLC)
	{	/* installe la bonne palette */
		pal = anim_par[canal].palette; 
		return  get_in_fli(frame,old_frame, anim_par[canal].handle, anim_par[canal].hnd_key, buffer, img,
							 x_img, &anim_par[canal].key, anim_par[canal].offset_flh,
                   	anim_par[canal].offset_key, taille_buffer);
	}
	else if(objet[anim_par[canal].objet].type == OBJ_MOD)
	{	para.handle = anim_par[canal].handle;
		para.old_frame = old_frame;
		para.frame = frame;
		para.buf = buffer;
		para.img = img;
		para.x_dest = x_img;
		para.work_adr = anim_par[canal].work_adr;
		
		para.retour = 0;			/* pour les modules qui ne gerent pas ca */
		(imp_mod[anim_par[canal].mod].get_frame)(&para);
		if(para.retour == -1)
			return 0;
		return 1;
	}
	else if(objet[anim_par[canal].objet].type == OBJ_SUIT)
	{	num_objet = anim_par[canal].objet;
		strcpy(fic,objet[num_objet].chemin);
		strcat(fic,objet[num_objet].tab_noms + frame*14);
		
		/* OUVERTURE PAR LE MODULE */
		
		anim_par[canal].handle = (int)Fopen(fic,FO_READ);
		if(anim_par[canal].handle < 0)
		{	super_alert(1,3,T_FILE_ERROR);
			return -1;
		}

		mod = anim_par[canal].mod;		/* une fois pour tt */

		/* reserve son espace donnees */
		if(mod == -1)
		{	super_alert(1,3,T_NO_MATCH_MOD);
			Fclose(anim_par[canal].handle);
			return -1;
		}
		
		if(imp_mod[mod].size_buf_para)
		{	anim_par[canal].work_adr = Malloc(imp_mod[mod].size_buf_para);
			if(!anim_par[canal].work_adr)
			{	super_alert(1,3,T_NO_MEM_OPEN_ANIM);
				Fclose(anim_par[canal].handle);
				return -1;
			}
		}
		taille = Fseek(0,anim_par[canal].handle,2);
		Fseek(0,anim_par[canal].handle,0);
		ini_para.size_fic = taille;
		ini_para.handle = anim_par[canal].handle;
		ini_para.work_adr = anim_par[canal].work_adr;
		
		(imp_mod[mod].init_imp)(&ini_para);
		if(ini_para.retour == -1)
		{	super_alert(1,3,T_OPEN_ERROR);
			Mfree(anim_par[canal].work_adr);
			Fclose(anim_par[canal].handle);
			return -1;
		}
		
		if(ini_para.x != objet[num_objet].x || ini_para.y != objet[num_objet].y)
		{	super_alert(1,3,T_ALL_SAME_FORM);
			Mfree(anim_par[canal].work_adr);
			Fclose(anim_par[canal].handle);
			return -1;
		}
		
		/* reserve un buffer au moins aussi grand que ce que le module demande */
		if(new_buffer(ini_para.size_buf))
		{	super_alert(1,3,T_READ_NO_MEM);
			Mfree(anim_par[canal].work_adr);
			Fclose(anim_par[canal].handle);
			return -1;
		}
		
		/* LECTURE PAR LE MODULE */
		para.handle = anim_par[canal].handle;
		para.old_frame = -1;
				/* num de frame */
		frame2 = min(objet[num_objet].num_img,ini_para.nb_img);
		frame2 = max(frame2 , 0);
		para.frame = frame2;
		para.buf = buffer;
		para.img = img;
		para.x_dest = x_img;
		para.work_adr = anim_par[canal].work_adr;
		
		para.retour = 0;			/* pour les modules qui ne gerent pas ca */
		(imp_mod[anim_par[canal].mod].get_frame)(&para);
		
		/* FERMETURE DU MODULE */	
	/*	(imp_mod[anim_par[canal].mod].close_anim); */

		Fclose(anim_par[canal].handle);
		Mfree(anim_par[canal].work_adr);
		
		if(para.retour == -1)
			return 0;
		
		return 1;
	}
		
   return 0;
}


void close_anim(int canal)
{	CLOSE_PARAM close_param;

	if(anim_par[canal].used)
	{	
		anim_par[canal].used = 0;
		
		if(objet[anim_par[canal].objet].type == OBJ_FLH || objet[anim_par[canal].objet].type == OBJ_FLI || objet[anim_par[canal].objet].type == OBJ_FLC)
		{	Mfree(anim_par[canal].offset_key);
			Mfree(anim_par[canal].offset_flh);
			Fclose(anim_par[canal].handle);
			Fclose(anim_par[canal].hnd_key);
		}else
		if(objet[anim_par[canal].objet].type == OBJ_MOD)
		{	if(imp_mod[anim_par[canal].mod].flags & MI_CLOSABLE)
			{	close_param.handle = anim_par[canal].handle;
				close_param.work_adr = anim_par[canal].work_adr;
				/* On ferme le module */
				(imp_mod[anim_par[canal].mod].close_anim)(&close_param);
			}
			Fclose(anim_par[canal].handle);
			Mfree(anim_par[canal].work_adr);
		}
				
		nb_canal--;
		if(nb_canal == 0)
			libere_buffer();
	}
	form_mem(UPDT_MEM);
}

int new_buffer(long taille)
{	
	taille = max(10000L,taille);			/* enleve des pb */

	if(reserved_buf)
	{	if(taille > taille_buffer)		/* il y a deja un buf, mais trop petit */
		{	Mfree(buffer);
			buffer = Malloc(taille);
			if(buffer)
			{	taille_buffer = taille;
				return 0;
			}else
			{	taille_buffer = 0;
				reserved_buf =0;
				return -1;
			}
		}
		return 0;
	}
	/* aucun buffer deja reserve */
	buffer = Malloc(taille);
	if(buffer)
	{	taille_buffer = taille;
		reserved_buf=1;
		return 0;
	}else
	{	taille_buffer = 0;
		return -1;
	}
}

void libere_buffer(void)
{	if(reserved_buf)
	{	Mfree(buffer);
		reserved_buf = 0;
	}
		
}

int get_one_frame(int num_objet, int frame, int *img, int x_img)
{	int retour;
	int canal;
	
	frame = max(0,frame);	/* pour ne pas avoir de frame negative */
	frame = min(objet[num_objet].nb_img-1,frame);	/* pour ne pas avoir de frame > nb_img */
	
	canal = open_anim(num_objet);
	if(canal == -1)
	{	super_alert(1,3,T_IMPOS_OPEN_ANIM);
		return 0;
	}
	
	retour = get_frame(canal,frame, -1, img, x_img);

	close_anim(canal);
	
	if(retour == 0)
		return 0;
	
	/* que c'est simple ! */

	return 1;
	
}
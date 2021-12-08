/********************MOUTAIN********************/
/*****             GESTION FLH              ****/
/*****             Version 0.1              ****/
/*****            Par Vanel R‚mi            ****/
/***********************************************/


#include <tos.h>
#include "windform.h"  /* et vdi.h et aes.h */
#include <string.h>
#include <stdio.h>
#include "flh_lib.h"
#include "cp_flh.h"
#include "gest_flh.h"
#include "mountain.h"
#include "objets.h"
#include "anim.h"
#include "txts.h"
#include "some_fnc.h"

extern OBJET objet[MAX_OBJET];

void find_name(char *str, char *dest);


int do_key_frame(char *nom_flh, char *nom_key, int frequence, OBJECT *adr_key)
{	int hnd_flh, hnd_key;
	int nb_pict;
	int buffer[64];
	long dum;
	char nom[10];
	int x,y;
	int *img,*buf;
	unsigned int frame,type;
	long *table_offset, offset=0;
	size_t taille_table_offset;
	long offset_table_offset;
	size_t size_frame, size_chunk, taille_mem, position;
	KEY_HEAD key;
	int head[10] = {0,0,0,0,0,0,0,0,0,0};
	
	hnd_flh = (int)Fopen(nom_flh,FO_READ);
	if(hnd_flh < 0)
		return 0;
	
	Fread(hnd_flh,128,buffer);
	
	if(!(buffer[2] == 0x44af ))
	{	super_alert(1,3,T_FILE_ERROR);
		Fclose(hnd_flh);
		return 0;
	}
	
	nb_pict = intel(buffer[3]);
	nb_pict = max(nb_pict,1);		/* certains flh ont 0 en nb_pict mais ont 1 image */
	x = intel(buffer[4]);
	y = intel(buffer[5]);
	
	key.head='FKEY';
	key.x=x;
	key.y=y;
	key.nb_img_in_flh = nb_pict;
	key.nb_img_in_key = (nb_pict-1)/frequence;
	key.frequence=frequence;
	find_name(nom_flh,nom);
	strcpy(key.nom_anim,nom);
	
	img = Malloc((size_t)x * (size_t)(y+1) * 2);
	if(img==0)
	{	super_alert(1,3,T_NO_MEMORY);
		Fclose(hnd_flh);
		return 0;
	}
	
	taille_table_offset = key.nb_img_in_key * 4;
	if(taille_table_offset)
	{	table_offset = Malloc(taille_table_offset);
		if(table_offset==0)
		{	super_alert(1,3,T_NO_MEMORY);
			Fclose(hnd_flh);
			Mfree(img);
			return 0;
		}
	}

	taille_mem = (size_t)Malloc(-1) - 64000L;
	buf = Malloc(taille_mem);
	if(buf==0)
	{	super_alert(1,3,T_NO_MEMORY);
		Fclose(hnd_flh);
		Mfree(table_offset);
		Mfree(img);
		return 0;
	}
		
	hnd_key = (int)Fcreate(nom_key,0);
	if(hnd_key < 0)
	{	
		super_alert(1,1,T_IMPOS_CREAT_KEYF);
		Mfree(img);
		Mfree(table_offset);
		Mfree(buf);
		Fclose(hnd_flh);
		return 0;
	}
		
	Fwrite(hnd_key,SIZE_HKEY,&key);
	
	/* Rechrche des saut des pointeur pour chaque images */
	position = 128;
	for(frame=0;frame<nb_pict;frame++)
	{	Fwrite(hnd_key,4,&position);
	
		Fread(hnd_flh,4,buf);
		size_frame = intell(*(long *)(buf));
		Fseek(size_frame-4,hnd_flh,1);
		position += size_frame;
	}
	
	/* realisation des key frames */
	
	Fseek(128,hnd_flh,0);
		
	for(frame=0;frame<nb_pict;frame++)
	{	sprintf(adr_key[KEY_IMG].ob_spec.tedinfo->te_ptext,T_IMAGE_DD,frame,nb_pict);
		objc_draw (adr_key, KEY_IMG, MAX_DEPTH, x_desk, y_desk, w_desk, h_desk);
		
		Fread(hnd_flh,4,buf);
		size_frame = intell(*(long *)(buf));
		if(size_frame > taille_mem)
		{	super_alert(1,3,T_NO_MEMORY);
			Fclose(hnd_flh);
			Fclose(hnd_key);
			Mfree(table_offset);
			Mfree(img);
			Mfree(buf);
			Fdelete(nom_key);
			return 0;
		}
		
		if(Fread(hnd_flh,size_frame-4,buf+2) != size_frame-4)
		{	super_alert(1,1,T_READ_ERROR);
			Fclose(hnd_flh);
			Fclose(hnd_key);
			Mfree(table_offset);
			Mfree(img);
			Mfree(buf);
			Fdelete(nom_key);
			return 0;
		}
		decomp(x,y,x,buf,img);		/* on decomp la frame */
		
		if( ( ((frame) % frequence) == 0) && frame) 	/* on tombe sur une frame multiple de freq */
		{	/* avant de sauver la key, on prend l'offset ou on va la sauver */
			table_offset[offset++] = Fseek(0,hnd_key,1);
			
			size_chunk = cp_brun_tc(img,buf, 0xFFFF,x,y,x); 
			type = intel(25);		/* FLI BRUN TC */
			size_chunk += 6;							/* pour l'entˆte */
			
						/* CREATION FRAME */
		
			size_frame = intell(size_chunk + 16);
			Fwrite(hnd_key,4,&size_frame);
			head[0] = 0xfaf1;							/* signature frame */
			Fwrite(hnd_key,2,head);
			head[0] = 0x100;							/* 1 chunk */
			Fwrite(hnd_key,2,head);
			Fwrite(hnd_key,8,&head[1]);			/* 8 oct 0 */
			
					/* CREATION CHUNK */
		
			dum = intell(size_chunk);
			Fwrite(hnd_key,4,&dum);
			Fwrite(hnd_key,2,&type);
			if( Fwrite(hnd_key,size_chunk - 6,buf) != size_chunk - 6)
			{	super_alert(1,3,T_WRITE_ERROR);
				Fclose(hnd_flh);
				Fclose(hnd_key);
				Mfree(table_offset);
				Mfree(img);
				Mfree(buf);
				Fdelete(nom_key);
				return 0;
			}
		}/* fin sav brun */
		
		
	}/* fin du for */	
	sprintf(adr_key[KEY_IMG].ob_spec.tedinfo->te_ptext,T_IMAGE_DD,nb_pict,nb_pict);
	objc_draw (adr_key, KEY_IMG, MAX_DEPTH, x_desk, y_desk, w_desk, h_desk);
		
	/* inscription de la table offset key */
	offset_table_offset = Fseek(0,hnd_key,1);
	Fwrite(hnd_key,taille_table_offset,table_offset);
	/* ou trouver cette table */	
	Fseek(POS_TABLE_O,hnd_key,0);		/* position key.table_offset */
	Fwrite(hnd_key,4,&offset_table_offset);
	
	Mfree(img);
	Mfree(buf);
	Mfree(table_offset);
	Fclose(hnd_flh);
	Fclose(hnd_key);
	return 1;
}

int get_flh_info(char *fic, char *dest,int *x, int *y, int *nb_pict)
{	int hnd_fli;
	int buffer[64];
	
	hnd_fli = (int)Fopen(fic,FO_READ);
	if(hnd_fli < 0)
		return 0;
	
	Fread(hnd_fli,128,buffer);
	
	Fclose(hnd_fli);
	
	if(!(buffer[2] == 0x44af) )
	{	
		super_alert(1,3,T_ISNT_A_FLH);
		return 0;
	}
	
	*nb_pict = intel(buffer[3]);
	*nb_pict = max(*nb_pict,1);		/* certains flh ont 0 en nb_pict mais ont 1 image */
	*x = intel(buffer[4]);
	*y = intel(buffer[5]);
	
	sprintf(dest,T_FLH_DDD_IMG,*x,*y,*nb_pict);

	return 1;
}

/* fonction qui prend n'importe quelle image dans un flh muni d'un key */
int get_in_flh(int frame_to_get, int preced_frame, int hnd_flh, int hnd_key,
               int *buffer, int *img, int x_img, KEY_HEAD *key, long *flh_offset,
               long *key_offset, size_t taille_buffer)
{
	size_t size_frame;
	int plus_proche_in_key, frame;
	int frame_plus_proche_in_key;
	char alert[100];
	
	if(preced_frame > frame_to_get)
		preced_frame = -1;
	
	/* rien a faire si :*/
	if(frame_to_get == preced_frame)
		return 1;
	
	/* juste une image si */
	if(frame_to_get == preced_frame+1 || frame_to_get == 0)
	{	Fseek(flh_offset[frame_to_get],hnd_flh,0);
		Fread(hnd_flh,4,buffer);
		size_frame = intell(*(long *)(buffer));
		if(size_frame > taille_buffer)
		{	sprintf(alert,T_GET_FLH1,size_frame,taille_buffer);
			super_alert(1,3,alert);
			return 0;
		}

		if(Fread(hnd_flh,size_frame-4,buffer+2) != size_frame-4)
		{	super_alert(1,3,T_READ_ERROR);
			return 0;
		}
	
		decomp(key->x,key->y,x_img,buffer,img);		/* on decomp la frame */
		
		return 1; /* c'est ok */
	}
	
	plus_proche_in_key = frame_to_get / key->frequence;
	frame_plus_proche_in_key = plus_proche_in_key * key->frequence;
	
	/* si notre position dans le flh est plus pres que la precedante key frame, on fait qq flh delta */
	if((frame_to_get - frame_plus_proche_in_key >=  frame_to_get-preced_frame) || (frame_plus_proche_in_key == 0))
	{		
		for(frame = preced_frame; frame < frame_to_get;frame++)
		{	Fseek(flh_offset[frame+1],hnd_flh,0);
			Fread(hnd_flh,4,buffer);
			size_frame = intell(*(long *)(buffer));
			if(size_frame > taille_buffer)
			{	sprintf(alert,T_GET_FLH2,size_frame,taille_buffer);
				super_alert(1,3,alert);
				return 0;
			}
			
			if(Fread(hnd_flh,size_frame-4,buffer+2) != size_frame-4)
			{	super_alert(1,3,T_READ_ERROR);
				return 0;
			}
			decomp(key->x,key->y,x_img,buffer,img);		/* on decomp la frame */
		}
		return 1;
	}
	
	/* methode key frame : */
	/* prise ds la key */
	Fseek(key_offset[plus_proche_in_key-1],hnd_key,0);
	Fread(hnd_key,4,buffer);
	size_frame = intell(*(long *)(buffer));

	if(Fread(hnd_key,size_frame-4,buffer+2) != size_frame-4)
	{	super_alert(1,3,T_READ_ERROR);
		return 0;
	}
	decomp(key->x,key->y,x_img,buffer,img);		/* on decomp la frame */
	
	/* prise ds le flh */
	for(frame = frame_plus_proche_in_key; frame < frame_to_get;frame++)
	{	Fseek(flh_offset[frame+1],hnd_flh,0);
		Fread(hnd_flh,4,buffer);
		size_frame = intell(*(long *)(buffer));
		if(size_frame > taille_buffer)
		{	sprintf(alert,T_GET_FLH3,size_frame,taille_buffer);
			super_alert(1,3,alert);
			return 0;
		}
		
		if(Fread(hnd_flh,size_frame-4,buffer+2) != size_frame-4)
		{	super_alert(1,3,T_READ_ERROR);
			return 0;
		}
		decomp(key->x,key->y,x_img,buffer,img);		/* on decomp la frame */
	}
	return 1;
}


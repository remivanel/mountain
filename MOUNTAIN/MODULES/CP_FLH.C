#include "FLH_LIB.H"
typedef unsigned long   size_t;


size_t cp_brun_tc(int *img,int *flh,int mask_pack,int x_ani,int y_ani ,int x_img )
{	size_t count = 0,pos = 0,bcl,old;
	char *dest = (char *)flh, compt, *lin;
	int pixel,pixel_maske,no_egal,ligne, *deb_ligne, egal;
	char *adr_packet, nb_packet;
	
	deb_ligne = img;
	for(ligne = 0; ligne < y_ani; ligne++)
	{	adr_packet = &dest[count++];		/* on note l'adr du nb de packet pour plus tard */
		nb_packet = 0;					/* ben tiend justement */
		
		pos = 0;
		while(pos < x_ani)
		{	nb_packet++;		/* un de plus */
			
			pixel = deb_ligne[pos++];
			pixel_maske = pixel & mask_pack;
			no_egal = 0;
			egal = 0;
			old = pos;
			while( (deb_ligne[pos] & mask_pack) != pixel_maske && no_egal < 126 && pos < x_ani)
			{	pixel = deb_ligne[pos];
				pixel_maske = pixel & mask_pack;
				no_egal++;	pos++; 
			}
			if(no_egal)			/* pas compact -> copy */
			{	if(pos == 320 || no_egal == 126)			/* optimisation, kan tu ns tiends ... */
					no_egal++;
				else
					pos--;
				compt = (signed char) -no_egal;
				dest[count++] = compt;
				lin = (char *)&deb_ligne[--old];
				no_egal = no_egal * 2;						/* on va copier en octets */
				for(bcl = 0; bcl < no_egal; bcl++)
					dest[count++] = lin[bcl];
			
			}else						/* cool... rle pack */
			{	while( (deb_ligne[pos] & mask_pack) == pixel_maske && egal < 126 && pos < x_ani)
				{	egal++;	pos++; }
				egal++;						/* paseke on a d‚j… pris le pixel de r‚f‚rence */
				compt = (signed char) egal;
				dest[count++] = compt;
				dest[count++] = (pixel >> 8) & 0xff;
				dest[count++] = pixel & 0xff;
			}
		}
		*adr_packet = nb_packet;		/* on peut mettre le nb de packet cr‚‚ */
		deb_ligne += x_img; 
	}
	return count;
	
}

size_t cp_delta_tc(int *old, int *img,int *flh,int mask_pack,int x_ani,int y_ani ,int x_img)
{	size_t count = 2,pos = 0,bcl,old_v;
	char *dest = (char *)flh, compt, *lin , *adr_pack;
	int pixel,no_egal,ligne, *deb_img,*deb_old, egal, ln_saut = 0,saut,ln_comp = 0;
	int packet,no_saut,acomp, *dd_img, *dd_old;
	int pixel_maske;
	
	/* on laisse de la place libre pour le nb de ligne comp */
	
	dd_img = img;
	dd_old = old;
	for(ligne = 0; ligne < y_ani; ligne++)
	{	
		for(pos = 0;pos < x_ani;pos++)
			if((dd_old[pos] & mask_pack) != (dd_img[pos] & mask_pack) )
				break;								/* teste si ligne egal */
		if(pos == x_ani)
		{	ln_saut++;
			goto end_lin;
		}
		else if(ln_saut)
		{	ln_saut = -ln_saut;
			dest[count++] = ln_saut & 0xff;
			dest[count++] = (ln_saut >> 8) & 0xff;
			ln_saut = 0;
		}
		
		for(pos = x_ani - 1;pos > 1;pos--)
			if((dd_old[pos]  & mask_pack) != (dd_img[pos] & mask_pack))
				break;
		acomp = (int)++pos;
		
		pos = 0;
		packet = 0;
		adr_pack = &dest[count];
		count += 2;
		ln_comp++;
		deb_img = dd_img;
		deb_old = dd_old;
		while(pos < acomp)
		{	packet++;
			saut = 0;
			no_egal = 1;
			no_saut = 0;		/* on compt le octets pareil */
			while((deb_old[saut] & mask_pack) == (deb_img[saut] & mask_pack) && saut < 255 && pos < x_ani)
			{	saut++; pos++; }
			dest[count++] = saut;
			deb_old += saut;
			deb_img += saut;
			if(pos == x_ani || saut == 255)
			{	dest[count++] = 0;
				goto end_pak;
			}
			old_v = pos;
			while( (deb_old[no_saut] & mask_pack) != (deb_img[no_saut] & mask_pack) && no_saut < 126 && pos < x_ani)
			{	no_saut++; pos++;  }
							/* on a no_saut pixel a compacter */
			
			pixel = deb_img[0];
			pixel_maske = pixel & mask_pack;
			while((deb_img[no_egal] & mask_pack) != (deb_img[no_egal - 1] & mask_pack) && no_egal < no_saut)
				no_egal += 2;
			
			if(no_egal>1)
			{	if(no_egal != no_saut)
					no_egal--;
				dest[count++] = (signed char)no_egal;
				egal = no_egal * 2;						/* on va copier en octets */
				lin = (char *)deb_img;
				for(bcl = 0; bcl < egal; bcl++)
					dest[count++] = lin[bcl];
				no_saut = no_egal;
			}else
			{	egal = 1;
				while((deb_img[egal] & mask_pack) == pixel_maske && egal < (acomp - old_v) && egal < 126)
					egal++;
				compt = (signed char) -egal;
				dest[count++] = compt;
				dest[count++] = (pixel >> 8) & 0xff;
				dest[count++] = pixel & 0xff;
				no_saut = egal;
			}
			pos = old_v + no_saut;
			deb_img += no_saut;
			deb_old += no_saut;
			
			end_pak:;
		}
		adr_pack[0] = packet & 0xff;
		adr_pack[1] = (packet >> 8) & 0xff;
		end_lin:;
		dd_old += x_img; 
		dd_img += x_img; 
	}
	flh[0] = intel(ln_comp);
	return count;
	
}

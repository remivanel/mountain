void export_mplayer(void)
{	int x,y;
	char chemin_bat[200],cmdline[250];
	int format,qualite;
	FILE *fp_bat;

	/* uniquement ici */
	actu_img=0;
	compt_bar=0;
	nb_img=end-start+1;
	mpl_para.rout_mpl = MPL_calc;
	/***/
		
	qualite = get_popup(mpl_var.adr_form,MPL_POP_Q,POP_QUALITE);
	format = get_popup(mpl_var.adr_form,MPL_FORMAT,POP_FORMAT);
	
	mpl_para.format = -1;

	x=projet.x;
	y=projet.y;
	
	img[0]=Malloc((long)x*y*2L+20);
	if(img[0]==0)
		return ;
	img[1]=Malloc((long)x*y*2L+20);	/* 0x2b */
	if(img[1]==0)
	{	Mfree(img[0]);
		return ;
	}
	mpl_para.ptr_img = img[0];
	
	strcpy(chemin_bat,chemin_key);
	strcat(chemin_bat,"MOUNTAIN.BAT");
	
	fp_bat = fopen(chemin_bat,"w");
	if(fp_bat == NULL)
	{	super_alert(1,3,T_IMPOS_OPEN_BAT);
		Mfree(img[0]);
		Mfree(img[1]);
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
	fprintf(fp_bat,"t=%d\n",200/projet.speed);
	fprintf(fp_bat,"k=%d\n",key_tab[key_choose]);
	fprintf(fp_bat,"q=%d\n",qualite);
	fprintf(fp_bat,"f=%ld\n",nb_img);
	if(format == 3)
		fprintf(fp_bat,"o=#%s.AVI\n",fic_exp.complet);
	else
		fprintf(fp_bat,"o=%s.MOV\n",fic_exp.complet);
	#ifdef _REG_
	fprintf(fp_bat,"m=%ld\n",&mpl_para);
	#endif
	fprintf(fp_bat,"data\ndum0000.tga\n.rept %ld\n.incr\n.disp\n.endr\n.stop\n",nb_img-1);
	fclose(fp_bat);

	
	/* Dans le dossier de M player ... */

	Dsetdrv(chemin_mplayer[0]-'A');
	strcpy(cmdline,chemin_mplayer);
	find_path(cmdline);
	Dsetpath(cmdline);

	sprintf(cmdline,".-d +a %s",chemin_bat);
	
	#ifdef _REG_
	/* ... Se trouve M Player */
	Save_stack();
	Pexec(0,chemin_mplayer,cmdline,"");
/*	Put_stack();*/
	graf_mouse(ARROW,0);
	#else
	super_alert(1,3,T_REGISTER_ONLY);
	#endif
	Mfree(img[0]);	Mfree(img[1]);

	end_preview();
}

int export_anim(void)
{	int x,y, *img[2],dummy,key;
	char chemin[200];
	long nb_img=end-start+1,frame;
	void *work_adr,*buf,*adr_param;
	int draw_preview = exp_var.adr_form[CALC_AFF].ob_state & SELECTED;
	
	
	if(projet.size_param <= 4)
		adr_param = &(long)projet.adr_param;
	else
		adr_param = projet.adr_param;
	
	actu_img=0;
	compt_bar=0;
	
	no_draw=FALSE;		/* pour n'afficher la prev k pdt le calc */
	
	if(!actu_projet || nb_exp_mod==0)
		return 0;

	Dsetdrv(projet.chemin[0] - 'A');
	strcpy(chemin,projet.chemin);
	find_path(chemin);
	Dsetpath(chemin);
	
	x=projet.x;
	y=projet.y;
	
	img[0]=Malloc((long)x*y*2L);
	if(img[0]==0)
		return 0;
	img[1]=Malloc((long)x*y*2L);
	if(img[1]==0)
	{	Mfree(img[0]);
		return 0;
	}
	
	exp_init_param.name=nom_anim;
	exp_init_param.x=x;
	exp_init_param.y=y;
	exp_init_param.fps=projet.speed;
	exp_init_param.nb_img=nb_img;		
	exp_init_param.size_buf=0;
	exp_init_param.param_adr=adr_param;

	if(exp_mod[num_mod].size_buf_perm)
	{	work_adr=Malloc(exp_mod[num_mod].size_buf_perm);
		if(work_adr==0)
		{	Mfree(img[0]);	Mfree(img[1]);
			return 0;
		}
	}
	exp_init_param.work_adr=work_adr;
	
	/* INITIALISATION */
	#ifdef _REG_
	if(nom_anim[0])
	{	(exp_mod[num_mod].init_exp)(&exp_init_param);	
	
		if(exp_init_param.retour == -1)
		{	Mfree(img[0]);	Mfree(img[1]);
			Mfree(work_adr);
			return 0;
		}
	}
	#else
	super_alert(1,3,T_REGISTER_ONLY);
	draw_preview=1;
	exp_var.adr_form[CALC_AFF].ob_state = SELECTED;
	wf_draw(&exp_var,0);
	#endif

	/* init ok */
	if(exp_init_param.size_buf)
	{	buf=Malloc(exp_init_param.size_buf);
		if(buf==0)
		{	Mfree(img[0]);	Mfree(img[1]);
			Mfree(work_adr);
			return 0;
		}
	}
	
	form_mem(UPDT_MEM);
	
	exp_img_param.buf=buf;
	exp_img_param.work_adr=work_adr;
	exp_img_param.param_adr=adr_param;
	
	for(frame=start;frame<=end;frame++)
	{	set_bar(++compt_bar,nb_img*2, &exp_var, CALC_FON, CALC_BAR);
		
		exp_img_param.img=img[actu_img];
		exp_img_param.old_img=img[1-actu_img];
	
		exp_img_param.frame=frame-start;
	
		if(calc_frame((int)frame, img[actu_img], x, y, x) == -1)
		{	super_alert(1,3,T_ERR_HAPEN);
			goto err_sav;
		}

		/* Preview */
		if(draw_preview)
		{	zoom(img[actu_img],calc_pre,x,y,x,calc_pre_x,calc_pre_y,calc_pre_x_mod16,1024);
			objc_draw(exp_var.adr_form,CALC_PREV,MAX_DEPTH,x_desk,y_desk,w_desk,h_desk);
		}
		set_bar(++compt_bar,nb_img*2,&exp_var, CALC_FON, CALC_BAR);
	
		#ifdef _REG_
		if(nom_anim[0])
		{	(exp_mod[num_mod].put_frame)(&exp_img_param);
			if(exp_img_param.retour == -1)
			{	super_alert(1,3,T_ERR_SAV);
				goto err_sav;
			}
		}
		#endif
		actu_img = 1-actu_img;
		
		graf_mkstate(&dummy, &dummy, &dummy,&key);
		if(key == (K_LSHIFT|K_RSHIFT))
			goto err_sav;
		
	}


	err_sav:;
	Mfree(img[0]);	Mfree(img[1]);
	Mfree(work_adr);
	Mfree(buf);

	end_preview();
	
	return 1;
}
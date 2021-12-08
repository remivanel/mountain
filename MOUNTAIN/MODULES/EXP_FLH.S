* MODULE d'EXPORT FLH

MACRO imovew p1, p2
	move.w	p1,p2
	rol.w		#8,p2
	ENDM

MACRO imovel p1, p2
	move.l	p1,p2
	rol.w	#8,p2
	swap	p2
	rol.w	#8,p2
	ENDM


IMPORT cp_brun_tc
IMPORT cp_delta_tc


	dc.b	"EFLH"
	dc.b	"1.00"
	dc.l	40				; size buff
	dc.l	4				; size param
	dc.l	$21			; flags
	bra.w	init.w
	bra.w	write.w
	bra.w	param.w
	bra.w	param_def.w
	
*buffer permanant
*0  W x
*2  W y
*4  L nb img
*8  W handle
*10 ? nom fic
*30 L adr prem image

*zone de paramettrage
*0  W qualit‚ 1..5
*2  W masque de qualit‚


********* ROUTINE D'INIT

init:
	movem.l	d0-d7/a0-a5,-(sp)
	
	move.l	20(a0),a1		; buffer permanant
	
	move.w	#0,4(a0)			;ok
*calc taille buffer
	moveq		#0,d0
	move.w	6(a0),d0			;x
	move.w	8(a0),d1			;y
	mulu.w	#6,d0				;d0=d0*6
	mulu.w	d1,d0				;d0=x*y
	lsr		d0					;/2  en fait on major par 3/2 pour + de secu
	move.l	d0,16(a0)		;buffer demand‚
*prend les infos
	move.w	6(a0),(a1)		;x
	move.w	8(a0),2(a1)		;y
	move.l	12(a0),4(a1)	;num
	move.l	a1,a2				; sav
	move.l	a0,a3

*reserve un buffer pour la prem image
	move.l	d0,-(sp)
	move.w	#72,-(sp)		;Malloc
	trap		#1
	lea		6(sp),sp
	move.l	d0,30(a2)		;sav adr
	bne 		ok_malloc
	
	move.w	#$ffff,4(a3)		; erreur d'ouverture
	movem.l	(sp)+,d0-d7/a0-a5
	rts	
	
ok_malloc:
*creation du fichier
		
	lea		fic(pc),a0			; chaine nom
	clr.b		(a0)
	
	move.l	(a3),a1			; a1-> nom sans ext
	bsr 		cat				; copie nom
	
	lea		ext(pc),a1		; extention
	bsr 		cat				; copie ext

*sav le nom
	lea		10(a2),a1
	bsr		cpy	
		
*Fcreate
	move.w	#0,-(sp) ; archive
	move.l	a0,-(sp)
	move.w	#60,-(sp)
	trap		#1
	lea		8(sp),sp
	move.w	d0,d7				;handle
	bpl		ok_create
	
	move.l	30(a2),-(sp)
	move.w	#73,-(sp)		;Mfree le buf
	trap		#1
	lea		6(sp),sp
	
	move.w	#$ffff,4(a3)		; erreur d'ouverture
	movem.l	(sp)+,d0-d7/a0-a5
	rts	
	
ok_create:
	move.w	d7,8(a2)	;handle

* construction de l'entete
	lea		head(pc),a0
	move.l	12(a3),d0	; nb img
	rol.w		#8,d0			;intel
	move.w	d0,6(a0)
	
	imovew	6(a3),d0		; x
	move.w	d0,8(a0)
	imovew	8(a3),d0		; y
	move.w	d0,10(a0)
	
	move.w	#1000,d0		;1 sec
	move.w	10(a3),d1	;fps
	beq		div0
	divu.w	d1,d0			;en millieme / image
	rol.w		#8,d0			;intel
div0:
	move.w	d0,16(a0)	;speed
	
;Fwrite l'entete 
	pea		head(pc)			;dest
	move.l	#128,-(sp)		;la taille
	move.w	d7,-(sp)			;d7=handle
	move.w	#64,-(sp)
	trap		#1
	lea		12(sp),sp
	
	
	
	movem.l	(sp)+,d0-d7/a0-a5
	rts

;*********** STRCAT a1 dans a0 ***********
cat:
	movem.l	a0-a1,-(sp)
bcl1:
	tst.b		(a0)+
	bne		bcl1
	subq		#1,a0
bcl2:
	move.b	(a1)+,(a0)+
	bne		bcl2
		
	movem.l	(sp)+,a0-a1
	rts

;*********** STRCPY a0 dans a1 ***********
cpy:
	movem.l	a0-a1,-(sp)
bclc:
	move.b	(a0)+,(a1)+
	bne		bclc
		
	movem.l	(sp)+,a0-a1
	rts
	

;********************* ROUTINE D'ECRITURE ******************

write:
	movem.l	d0-d7/a0-a5,-(sp)

	move.l	a0,a4				; sauvegarde

	move.l	18(a4),a5			; buffer de travail
	move.l	22(a4),a0			; buffer de param
	lea		qualite(pc),a1
	move.w	2(a0),(a1)			; le masque

	move.w	8(a5),d7			; handle fic
	move.l	4(a5),d0			; nb d'img
	move.l	(a4),d1			; index actuel
	
	bne		no_first
* la on traite la premiere
	move.l	12(a4),a0		; adr img
	move.l	30(a5),a1
	move.w	(a5),d0			; x
	move.w	2(a5),d1			; y
	mulu		d1,d0
copy:								; on sauve la premiere ds notre buffer
	move.w	(a0)+,(a1)+
	subq.l	#1,d0
	bne.s		copy
	
	move.l	12(a4),a0		; adr img
	move.l	4(a4),a1			; buffer
	move.w	qualite(pc),d0	; masque
	move.w	(a5),d1			; x
	move.w	2(a5),d2			; y
	
	bsr		cp_brun_tc
	addq.l	#6,d0
	move.l	d0,d6				; size chunk
*FRAME
	add.l		#16,d0				; size frame
	rol.w		#8,d0
	swap		d0					;intell
	rol.w		#8,d0
	lea		fic(pc),a0			; un petit buffer
	move.l	d0,(a0)			; size frame
	move.w	#$faf1,4(a0)		; type
	move.w	#$100,6(a0)		; nb chunk
	move.l	#0,8(a0)
	move.l	#0,12(a0)
*fwrite head frame
	move.l	a0,-(sp)			;dest
	move.l	#16,-(sp)		;la taille
	move.w	d7,-(sp)			;d7=handle
	move.w	#64,-(sp)
	trap		#1
	lea		12(sp),sp
*CHUNK
	move.l	d6,d0
	rol.w		#8,d0
	swap		d0					;intell
	rol.w		#8,d0
	lea		fic(pc),a0			; un petit buffer
	move.l	d0,(a0)			; size frame
	move.w	#$1900,4(a0)	; TYPE FLH_BRUN
*fwrite head chunk
	move.l	a0,-(sp)			;dest
	move.l	#6,-(sp)			;la taille
	move.w	d7,-(sp)			;d7=handle
	move.w	#64,-(sp)
	trap		#1
	lea		12(sp),sp
*fwrite chunk lui meme
	move.l	d6,d0
	subq.l	#6,d0
	move.l	4(a4),-(sp)			;dest
	move.l	d0,-(sp)			;la taille
	move.w	d7,-(sp)			;d7=handle
	move.w	#64,-(sp)
	trap		#1
	lea		12(sp),sp
	
	bra suite
	
no_first:
* image intermediaire
	move.l	8(a4),a0			; adr old img
	move.l	12(a4),a1		; adr img
	move.l	4(a4),-(sp)		; buffer
	move.w	qualite(pc),d0	; masque
	move.w	(a5),d1			; x
	move.w	2(a5),d2			; y
	
	bsr		cp_delta_tc
	addq		#4,sp
	
	tst.l		d0
	beq		frame_vide
	
	addq.l	#6,d0
	move.l	d0,d6				; size chunk
*FRAME
	add.l		#16,d0				; size frame
	rol.w		#8,d0
	swap		d0					;intell
	rol.w		#8,d0
	lea		fic(pc),a0			; un petit buffer
	move.l	d0,(a0)			; size frame
	move.w	#$faf1,4(a0)		; type
	move.w	#$100,6(a0)		; nb chunk
	move.l	#0,8(a0)
	move.l	#0,12(a0)
*fwrite head frame
	move.l	a0,-(sp)			;dest
	move.l	#16,-(sp)		;la taille
	move.w	d7,-(sp)			;d7=handle
	move.w	#64,-(sp)
	trap		#1
	lea		12(sp),sp
*CHUNK
	move.l	d6,d0
	rol.w		#8,d0
	swap		d0					;intell
	rol.w		#8,d0
	lea		fic(pc),a0			; un petit buffer
	move.l	d0,(a0)			; size frame
	move.w	#$1b00,4(a0)	; TYPE FLH_DELTA
*fwrite head chunk
	move.l	a0,-(sp)			;dest
	move.l	#6,-(sp)			;la taille
	move.w	d7,-(sp)			;d7=handle
	move.w	#64,-(sp)
	trap		#1
	lea		12(sp),sp
*fwrite chunk lui meme
	move.l	d6,d0
	subq.l	#6,d0
	move.l	4(a4),-(sp)			;dest
	move.l	d0,-(sp)			;la taille
	move.w	d7,-(sp)			;d7=handle
	move.w	#64,-(sp)
	trap		#1
	lea		12(sp),sp
	
	bra		suite
	
frame_vide:
	lea		fic(pc),a0			; un petit buffer
	clr.l		(a0)			; size frame
	move.w	#$faf1,4(a0)		; type
	clr.w		6(a0)			; nb chunk
	clr.l		8(a0)
	clr.l		12(a0)
*fwrite head frame
	move.l	a0,-(sp)			;dest
	move.l	#16,-(sp)		;la taille
	move.w	d7,-(sp)			;d7=handle
	move.w	#64,-(sp)
	trap		#1
	lea		12(sp),sp

suite:
	move.l	4(a5),d0			; nb d'img
	move.l	(a4),d1			; index actuel
	subq.l	#1,d0
	cmp.l		d0,d1				; derniere image ?
	bne		fin

* la on met l'image de transition vers la premiere (pour le loop)
	move.l	12(a4),a0		; adr old img 
	move.l	30(a5),a1			; adr img (la premiere )
	move.l	4(a4),-(sp)		; buffer
	move.w	qualite(pc),d0	; masque
	move.w	(a5),d1			; x
	move.w	2(a5),d2			; y
	
	bsr		cp_delta_tc
	addq		#4,sp
	
	tst.l		d0
	beq		frame0_vide
	
	addq.l	#6,d0
	move.l	d0,d6				; size chunk
*FRAME
	add.l		#16,d0				; size frame
	rol.w		#8,d0
	swap		d0					;intell
	rol.w		#8,d0
	lea		fic(pc),a0			; un petit buffer
	move.l	d0,(a0)			; size frame
	move.w	#$faf1,4(a0)		; type
	move.w	#$100,6(a0)		; nb chunk
	clr.l		8(a0)
	clr.l		12(a0)
*fwrite head frame
	move.l	a0,-(sp)			;dest
	move.l	#16,-(sp)		;la taille
	move.w	d7,-(sp)			;d7=handle
	move.w	#64,-(sp)
	trap		#1
	lea		12(sp),sp
*CHUNK
	move.l	d6,d0
	rol.w		#8,d0
	swap		d0					;intell
	rol.w		#8,d0
	lea		fic(pc),a0			; un petit buffer
	move.l	d0,(a0)			; size frame
	move.w	#$1b00,4(a0)	; TYPE FLH_DELTA
*fwrite head chunk
	move.l	a0,-(sp)			;dest
	move.l	#6,-(sp)			;la taille
	move.w	d7,-(sp)			;d7=handle
	move.w	#64,-(sp)
	trap		#1
	lea		12(sp),sp
*fwrite chunk lui meme
	move.l	d6,d0
	subq.l	#6,d0
	move.l	4(a4),-(sp)			;dest
	move.l	d0,-(sp)			;la taille
	move.w	d7,-(sp)			;d7=handle
	move.w	#64,-(sp)
	trap		#1
	lea		12(sp),sp
	
	bra		suite0
	
frame0_vide:
	lea		fic(pc),a0			; un petit buffer
	clr.l		(a0)			; size frame
	move.w	#$faf1,4(a0)		; type
	clr.w		6(a0)			; nb chunk
	move.l	#0,8(a0)
	move.l	#0,12(a0)
*fwrite head frame
	move.l	a0,-(sp)			;dest
	move.l	#16,-(sp)		;la taille
	move.w	d7,-(sp)			;d7=handle
	move.w	#64,-(sp)
	trap		#1
	lea		12(sp),sp

suite0:
;Fclose le fic
	move.w	d7,-(sp)			;d7=handle
	move.w	#62,-(sp)
	trap		#1
	lea		4(sp),sp

* libere le buf de prem image
	move.l	30(a5),-(sp)
	move.w	#73,-(sp)		;Mfree le buf
	trap		#1
	lea		6(sp),sp

* la on va prendre la taille du fichier et la mettre ds les 4 premiers octets
;Fopen le fic
	move.w	#1,-(sp)				; WRITE
	pea		10(a5)		;nom fic
	move.w	#61,-(sp)
	trap		#1
	lea		8(sp),sp
	move.w	d0,d7
	
;	Fseek au debut
	move.w	#0,-(sp)			;mode absolu
	move.w	d7,-(sp)			;handle
	move.l	#0,-(sp)			;debut
	move.w	#66,-(sp)
	trap		#1
	lea		10(sp),sp

;	Fseek a la fin
	move.w	#2,-(sp)			;mode relatif a la fin
	move.w	d7,-(sp)			;handle
	move.l	#0,-(sp)			;fin
	move.w	#66,-(sp)
	trap		#1
	lea		10(sp),sp
	
	rol.w		#8,d0
	swap		d0
	rol.w		#8,d0
	
	lea		fic(pc),a0
	move.l	d0,(a0)

;	Fseek au debut
	move.w	#0,-(sp)			;mode absolu
	move.w	d7,-(sp)			;handle
	move.l	#0,-(sp)			;debut
	move.w	#66,-(sp)
	trap		#1
	lea		10(sp),sp

*fwrite taille fic
	pea		fic(pc)
	move.l	#4,-(sp)			;la taille
	move.w	d7,-(sp)			;d7=handle
	move.w	#64,-(sp)
	trap		#1
	lea		12(sp),sp

	
;Fclose le fic
	move.w	d7,-(sp)			;d7=handle
	move.w	#62,-(sp)
	trap		#1
	lea		4(sp),sp

fin:
	move.w	#0,16(a4)		; retour ok
	
	movem.l	(sp)+,d0-d7/a0-a5
	rts

******* ROUTINE DE PARAMETTRAGE

param:
	movem.l	d0-d7/a0-a5,-(sp)
	
	move.l	a0,a5		; sav
	move.w	(a0),d7	; le num qualite 1..7
	
; init aes :
	lea		params(pc),a0
	lea		control(pc),a1
	move.l	a1,(a0)
	lea		global(pc),a1
	move.l	a1,4(a0)
	lea		int_in(pc),a1
	move.l	a1,8(a0)
	lea		int_out(pc),a1
	move.l	a1,12(a0)
	lea		addr_in(pc),a1
	move.l	a1,16(a0)
	lea		addr_out(pc),a1
	move.l	a1,20(a0)

param_bcl:
	cmp.b		#5,d7
	bne		no5
	lea		alert5(pc),a0
	moveq		#1,d0				; defaut
	bsr 		form_alert
	cmp.w		#1,d0
	beq		fin_p				; ok ?
	moveq		#4,d7				; sinon, qualite moindre
	bra		param_bcl

no5:
	cmp.b		#4,d7
	bne		no4
	lea		alert4(pc),a0
	moveq		#2,d0				; defaut
	bsr 		form_alert
	cmp.w		#2,d0
	beq		fin_p				; ok ?
	cmp.w		#1,d0
	bne		s5
	moveq		#5,d7				; non, alors qualite plus
	bra		param_bcl
s5:moveq		#3,d7				; sinon, qualite moindre
	bra		param_bcl

no4:
	cmp.b		#3,d7
	bne		no3
	lea		alert3(pc),a0
	moveq		#2,d0				; defaut
	bsr 		form_alert
	cmp.w		#2,d0
	beq		fin_p				; ok ?
	cmp.w		#1,d0
	bne		s4
	moveq		#4,d7				; non, alors qualite plus
	bra		param_bcl
s4:moveq		#2,d7				; sinon, qualite moindre
	bra		param_bcl

no3:
	cmp.b		#2,d7
	bne		no2
	lea		alert2(pc),a0
	moveq		#2,d0				; defaut
	bsr 		form_alert
	cmp.w		#2,d0
	beq		fin_p				; ok ?
	cmp.w		#1,d0
	bne		s3
	moveq		#3,d7				; non, alors qualite plus
	bra		param_bcl
s3:moveq		#1,d7				; sinon, qualite moindre
	bra		param_bcl

no2:
	lea		alert1(pc),a0
	moveq		#2,d0				; defaut
	bsr 		form_alert
	cmp.w		#2,d0
	beq		fin_p				; ok ?
	moveq		#2,d7				; non, alors qualite plus
	
	bra		param_bcl

fin_p:
	move.w	d7,(a5)+			; sav qualite
	lea		tab_q(pc),a0
	subq		#1,d7
	move.w	(d7.w*2,a0),(a5)	; sav masque
	movem.l	(sp)+,d0-d7/a0-a5
	rts

************** PARAMETTRES PAR DEFAUTS
param_def:
	move.l	#$0005ffff,(a0)
	rts
	

	
; appel l'alert en A0, defaut en d0, retour en D0
form_alert:
	movem.l	d1/a1,-(sp)		; save
	lea		control(pc),a1
	move.w	#52,(a1)
	move.w	#1,2(a1)
	move.w	#1,4(a1)
	move.w	#1,6(a1)
	move.w	#0,8(a1)
	lea		int_in(pc),a1
	move.w	d0,(a1)
	lea		addr_in(pc),a1
	move.l	a0,(a1)
;appel aes
	lea		params(pc),a1
	move.l	a1,d1
	move.w	#200,d0
	trap		#2					; AES
	lea		int_out(pc),a1
	move.w	(a1),d0			;prend retour
	movem.l	(sp)+,d1/a1
	rts


fic:	dc.b  "                       ",0
ext:	dc.b	".FLH",0,0
qualite:
		dc.w	$ffff
tab_q:
		dc.w	$8410, $C618, $E71C, $F79E, $FFFF
		
alert5:
		dc.b  "[2][ Choix de la qualit‚| Qualit‚ actuelle :| 5 - Meilleur][ OK | Moins ]",0
alert4:
		dc.b  "[2][ Choix de la qualit‚| Qualit‚ actuelle :| 4 - Passable][ Plus | OK | Moins ]",0
alert3:
		dc.b  "[2][ Choix de la qualit‚| Qualit‚ actuelle :| 3 - Moyenne ][ Plus | OK | Moins ]",0
alert2:
		dc.b  "[2][ Choix de la qualit‚| Qualit‚ actuelle :| 2 - Mediocre][ Plus | OK | Moins ]",0
alert1:
		dc.b  "[2][ Choix de la qualit‚| Qualit‚ actuelle :| 1 - Horrible][ Plus | OK ]",0
even
head:	dc.w	0,0,$44af,0,0,0,$1000,$300
		dc.w	0,0,0,$d2c1,$812f,'MN','T ',$d2c1
		dc.w	$812f,'TN','T ',$100,$100,0,0,0
		dc.w	0,0,0,0,0,0,0,0
		dc.w 	0,0,0,0,0,0,0,0
		dc.w	$8000,0,$9292,0,0,0,0,0
		dc.w	0,0,0,0,0,0,0,0	
		dc.w	0,0,0,0,0,0,0,0

params:	dc.l	0,0,0,0,0,0
control:	dc.w	0,0,0,0,0
global:	dc.w	0,0,0,0,0,0,0,0,0,0,0,0,0,0
int_in:	dc.w	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
int_out:	dc.w	0,0,0,0,0,0,0
addr_in:	dc.l	0,0,0
addr_out:	dc.l	0

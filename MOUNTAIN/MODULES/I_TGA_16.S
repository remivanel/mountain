* CHARGEMENT TGA


	dc.b	"TGAI"
	dc.b	"1.00"
	dc.l	20
	dc.l	$21			;flags
	bra.w	init.w
	bra.w	read.w

; Buffer :
;0 W -> X
;2 W -> Y
;4 b -> reso (16bits)
;5 b -> inverse haut-bas
;6 L -> taille fic
;10 W -> comp 0->non 1 -> oui
;12 W -> offset debut image

init:
	movem.l	d0-d7/a0-a5,-(sp)
	
	move.l	a0,a4				; sauvegarde
	
	move.l	20(a4),a5			; buffer de travail
	
;	Fseek au debut
	move.w	#0,-(sp)				;mode absolu
	move.w	(a4),-(sp)			;(a4)=handle
	move.l	#0,-(sp)				;debut
	move.w	#66,-(sp)
	trap		#1
	lea		10(sp),sp
	
; Fread 4 oct ds le buffer demande
	move.l	a5,-(sp)
	move.l	#4,-(sp)
	move.w	(a4),-(sp)
	move.w	#63,-(sp)
	trap		#1
	lea		12(sp),sp
	
	move.w	#0,10(a5)    ; par defaut, non compacte
	
	move.l	(a5),d0
	
	swap		d0
	rol.w		#8,d0				; prend le byte d'offset
	add.w		#18,d0
	move.w	d0,12(a5)		; stoque l'offset
	swap		d0
	
	and.w		#$ff00,d0
	cmp.w		#$0200,d0
	beq		good
	cmp.w		#$0A00,d0
	beq		good_cp					; c'est compacte
	move.w	#$FFFF,6(a4)			; c'est pas bon
	movem.l	(sp)+,d0-d7/a0-a5
	rts

good_cp:
	move.w	#1,10(a5)    ; compact‚
good:
	move.w	#0,6(a4)			; ok
	
;	Fseek + 8
	move.w	#1,-(sp)				;mode relatif
	move.w	(a4),-(sp)			;(a4)=handle
	move.l	#8,-(sp)				;+8
	move.w	#66,-(sp)
	trap		#1
	lea		10(sp),sp

; Fread taille x et y ( 4o )
	move.l	a5,-(sp)
	move.l	#4,-(sp)
	move.w	(a4),-(sp)
	move.w	#63,-(sp)
	trap		#1
	lea		12(sp),sp
	
	move.w	(a5),d0
	rol.w		#8,d0
	move.w	d0,(a5)
	move.w	2(a5),d0
	rol.w		#8,d0
	move.w	d0,2(a5)			;intellise
	
; Fread profondeur en bits et retournement
	lea		4(a5),a0
	move.l	a0,-(sp)
	move.l	#2,-(sp)
	move.w	(a4),-(sp)
	move.w	#63,-(sp)
	trap		#1
	lea		12(sp),sp
	
	move.b	4(a5),d0
	
	cmp.b		#$10,d0		; 16 bits ?
	beq		good2
	move.w	#$FFFF,6(a4)			; c'est pas bon
	movem.l	(sp)+,d0-d7/a0-a5
	rts
	
good2:
	
;calcul taille du fichier (juste l'image)

	moveq		#0,d6
	moveq		#0,d7
	move.w	(a5),d6			; taille x
	move.w	2(a5),d7			; taille y
	mulu.w	d7,d6
	lsl.l		#1,d6				; 16 bits/pixels
	move.l	d6,6(a5)			; on la stoque

	tst.w		10(a5)			; compact‚e ?
	beq		good3				; non
	
taille_cp:
	move.l	2(a4),d7			; en fait on ne met que la taille du fichier compacte
	moveq		#0,d0
	move.w	12(a5),d0
	sub.l		d0,d7				; eleve la taille entete
	move.l	d7,6(a5)			; on la stoque
	add.l		d7,d6				; pour les img comp, on prevoit un buff de decom
	
good3:	
; on a tout ce qu'il faut

	move.w	(a5),8(a4)			; taille x
	move.w	2(a5),10(a4)		; taille y
	move.l	#1,12(a4)			; nb img
	move.l	d6,16(a4)			; buffer de lecture
	lea		info(PC),a0
	move.l	a0,24(a4)			; la desc

	movem.l	(sp)+,d0-d7/a0-a5
	rts

;********************* ROUTINE DE LECTURE ******************

read:
	movem.l	d0-d7/a0-a5,-(sp)

	move.l	a0,a4				; sauvegarde

	move.l	20(a4),a5			; buffer de travail

	moveq		#0,d6
	move.w	12(a5),d6			;offset debut img
		
;	Fseek au debut de l'img
	move.w	#0,-(sp)				;mode absolu
	move.w	(a4),-(sp)			;(a4)=handle
	move.l	d6,-(sp)				;debut
	move.w	#66,-(sp)
	trap		#1
	lea		10(sp),sp

	move.l	6(a5),d6			; taille du fic

	move.l	10(a4),a3		; buffer de destination
	
; Fread l'img
	move.l	a3,-(sp)			;dest
	move.l	d6,-(sp)			;la taille
	move.w	(a4),-(sp)		;(a4)=handle
	move.w	#63,-(sp)
	trap		#1
	lea		12(sp),sp

	cmp.l		d0,d6
	beq		no_err
	
	move.w	#$ffff,24(a4)			; c pas bon
	movem.l	(sp)+,d0-d7/a0-a5
	rts

no_err:	
	move.w	#0,24(a4)

;on copie l'image dans l'ecran de dest en tenant compte de la largeur de dest

	move.w	2(a5),d7			; taille y
	subq		#1,d7				; pour le dbf
	move.w	18(a4),d5		; largeur ecran dest
	move.w	(a5),d6			; taille x
	sub.w		d6,d5				; le nb de pixels a saute a chaque fin de ligne
	lsl.w		#1,d5				; 16 bits/pixels
	move.l	14(a4),a0		; ecran dest
	
	move.b	5(a5),d4			;byte d'inversement
	and.b		#%100000,d4
	beq		inverse

	tst.w		10(a5)			;compact‚ ??
	bne		comp
	
boucley:
	move.w	(a5),d6			; taille x
	subq		#1,d6				; pour le dbf
bouclex:
	move.w	(a3)+,d4
	ror		#5,d4
	lsr.b		#1,d4
	roxr		#2,d4	
	move.w	d4,(a0)+
	
	dbf		d6,bouclex
	add.w		d5,a0				; le saut de pixel
	dbf		d7,boucley

	movem.l	(sp)+,d0-d7/a0-a5
	rts

comp:
	move.w	18(a4),d5		; larg dest
	cmp.w		(a5),d5
	beq		opt_cp
	move.l	a3,a0				; si la largeur de l'img different de la hauteur, on decomp ds un buffer tempo
	add.l		6(a5),a0			;apres taille fic
opt_cp:
	move.w	(a5),d5			;xdest
	move.w	2(a5),d6			;ydest
	mulu.w	d6,d5				; taille img
	lsl.l		d5					;*2
	move.l	a0,a1				; sav la dest
	add.l		d5,a1				; a1 = valeur atteindre
	
bcl_c:
	moveq		#0,d1
	move.b	(a3)+,d1			; le byte de compte
	cmp.w		#127,d1
	bmi		no_cmp
	sub.b		#128,d1			; c compacte
	move.w	(a3)+,d2			; le pixel a n copier
	ror		#5,d2
	lsr.b		#1,d2
	roxr		#2,d2	
bcl_cp:
	move.w	d2,(a0)+	
	dbf		d1,bcl_cp
	bra		suiv

no_cmp:							; pas cp
	move.w	(a3)+,d2			; les pixel a n copier
	ror		#5,d2
	lsr.b		#1,d2
	roxr		#2,d2	
	move.w	d2,(a0)+	
	dbf		d1,no_cmp
	
suiv:
	cmp.l		a1,a0
	bmi		bcl_c				; on n'a pas fini la ligne...

	move.w	18(a4),d5		; larg dest
	cmp.w		(a5),d5
	beq		opt_cp2
;la on doit recopier le buffer tempo vers l'ecran en tenant compte de la largeur differente	
	move.l	10(a4),a0		; buffer de destination
	add.l		6(a5),a0			; apres taille fic
	move.l	14(a4),a1		; ecran dest
	move.w	2(a5),d7			; taille y
	subq		#1,d7				; pour le dbf
	move.w	(a5),d6			; taille x
	sub.w		d6,d5
	lsl.w		#1,d5
	
cbcly:
	move.w	(a5),d6			; taille x
	subq		#1,d6				; pour le dbf
cbclx:
	move.w	(a0)+,(a1)+
	
	dbf		d6,cbclx
	add.w		d5,a1				; le saut de pixel
	dbf		d7,cbcly
	
opt_cp2:

	movem.l	(sp)+,d0-d7/a0-a5
	rts


inverse:
	tst.w		10(a5)			;compact‚ ??
	bne		icomp

	move.l	6(a5),d6			; taille du fic
	add.l		d6,a3	
	move.w	(a5),d0			; taille x
	lsl.w		#1,d0
	sub.w		d0,a3		
	lsl.w		#1,d0
	
iboucley:
	move.w	(a5),d6			; taille x
	subq		#1,d6				; pour le dbf
ibouclex:
	move.w	(a3)+,d4
	ror		#5,d4
	lsr.b		#1,d4
	roxr		#2,d4	
	move.w	d4,(a0)+
	
	dbf		d6,ibouclex
	add.w		d5,a0				; le saut de pixel
	sub.w		d0,a3				;ligne precedente src
	dbf		d7,iboucley

	movem.l	(sp)+,d0-d7/a0-a5
	rts

icomp:
	move.w	2(a5),d0			; taille y
	mulu.w	18(a4),d0		; *larg dest = taille ecran dest
	lsl.l		#1,d0				; 16 bits/pixels
	add.l		d0,a0				; on se met a la fin
	move.w	18(a4),d5		; larg dest
	lsl.w		d5					;*2
	move.l	a0,a1				; sav la dest			
	move.l	a1,a2				; a2= adr de fin de ligne
	add.w		(a5),a2
	add.w		(a5),a2
	sub.w		d5,a2				;  preced ligne
	
ic_ligne:
	sub.w		d5,a1				; une ligne en arriere
	move.l	a1,a0				; a0=adr de travail
bcl_ic:
	moveq		#0,d1
	move.b	(a3)+,d1			; le byte de compte
	cmp.w		#127,d1
	bmi		ino_cmp
	sub.b		#128,d1			; c compacte
	move.w	(a3)+,d2			; le pixel a n copier
	ror		#5,d2
	lsr.b		#1,d2
	roxr		#2,d2	
ibcl_cp:
	move.w	d2,(a0)+	
	dbf		d1,ibcl_cp
	bra		i_suiv

ino_cmp:							; pas cp
	move.w	(a3)+,d2			; les pixel a n copier
	ror		#5,d2
	lsr.b		#1,d2
	roxr		#2,d2	
	move.w	d2,(a0)+	
	dbf		d1,ino_cmp
	
i_suiv:
	cmp.l		a2,a0
	bmi		bcl_ic			; on n'a pas fini la ligne...
	sub.w		d5,a2				;  preced ligne
	
	dbf		d7,ic_ligne

	movem.l	(sp)+,d0-d7/a0-a5
	rts

info:	dc.b  "Image TGA 16 bits",0

* CHARGEMENT FRACLAND


	dc.b	"FRAC"
	dc.b	"1.00"
	dc.l	60
	dc.l	1			; flags
	bra.w	init.w
	bra.w	read.w
	
init:
	movem.l	d0-d7/a0-a6,-(sp)
	
	move.l	a0,a4				; sauvegarde
	
	move.l	20(a4),a5			; buffer de travail
	
;	Fseek au debut
	move.w	#0,-(sp)				;mode absolu
	move.w	(a4),-(sp)			;(a4)=handle
	move.l	#0,-(sp)				;debut
	move.w	#66,-(sp)
	trap		#1
	lea		10(sp),sp
	
; Fread 8 oct ds le buffer demande
	move.l	a5,-(sp)
	move.l	#8,-(sp)
	move.w	(a4),-(sp)
	move.w	#63,-(sp)
	trap		#1
	lea		12(sp),sp
	
	move.l	(a5),d0
	cmp.l		#'FRAC',d0
	beq		good
	
	move.w	#$FFFF,6(a4)			; c'est pas bon
	movem.l	(sp)+,d0-d7/a0-a6
	rts

good:
	move.l	4(a5),d0
	cmp.l		#'A   ',d0
	beq		good2
	move.w	#$FFFF,6(a4)			; c'est pas bon
	movem.l	(sp)+,d0-d7/a0-a6
	rts

good2:
	move.w	#0,6(a4)			; ok
	
; Fread taille reso et nb img  ( 4o )
	move.l	a5,-(sp)
	move.l	#4,-(sp)
	move.w	(a4),-(sp)
	move.w	#63,-(sp)
	trap		#1
	lea		12(sp),sp

; Fread pallette
	lea		4(a5),a0
	move.l	a0,-(sp)
	move.l	#32,-(sp)
	move.w	(a4),-(sp)
	move.w	#63,-(sp)
	trap		#1
	lea		12(sp),sp

;conveti palette
	moveq		#31,d0
	lea		4(a5),a0
pal:
	move.w	(a0),d1				; le pix en mode STF (0777)
	bfextu	d1{21:3},d2			; extrait le rouge
	lsl.w		#2,d2					; sur 5 bits
	move.w	d2,d3
	lsl.w		#6,d3
	bfextu	d1{25:3},d2			; extrait le vert
	lsl.w		#3,d2					; sur 6 bits
	or.w		d2,d3
	lsl.w		#5,d3
	bfextu	d1{29:3},d2			; extrait le blue
	lsl.w		#2,d2					; sur 5 bits
	or.w		d2,d3
	move.w	d3,(a0)+
	dbf		d0,pal

; on a tout ce qu'il faut
	
	move.w	#320,8(a4)			; taille x
	move.w	#200,10(a4)			; taille y

	tst.w		(a5)
	beq		st_bass
	move.w	#640,8(a4)			; taille x
	move.w	#400,10(a4)			; taille y
st_bass:

	move.l	#0,12(a4)			; nb img
	move.w	2(a5),14(a4)		; nb img'
	move.l	#32000,16(a4)			; buffer de lecture
	lea		info(PC),a0
	move.l	a0,24(a4)			; la desc

	movem.l	(sp)+,d0-d7/a0-a6
	rts

;********************* ROUTINE DE LECTURE ******************

read:
	movem.l	d0-d7/a0-a6,-(sp)

	move.l	a0,a4				; sauvegarde

	move.l	20(a4),a5			; buffer de travail
	
	move.l	6(a4),d0			; num img
	cmp.w		2(a4),d0
	bne		pas_pareil
	movem.l	(sp)+,d0-d7/a0-a6		; si meme img, on quitte
	rts
	
pas_pareil:
	mulu.w	#32000,d0			; cherche debut image
	add.l		#44,d0				; saute entete

;	Fseek au debut de l'img
	move.w	#0,-(sp)				;mode absolu
	move.w	(a4),-(sp)			;(a4)=handle
	move.l	d0,-(sp)				;debut
	move.w	#66,-(sp)
	trap		#1
	lea		10(sp),sp

	move.l		10(a4),a3		; buffer de lecture
	
; Fread l'img
	move.l	a3,-(sp)			;dest
	move.l	#32000,-(sp)	;la taille
	move.w	(a4),-(sp)		;(a4)=handle
	move.w	#63,-(sp)
	trap		#1
	lea		12(sp),sp
	
	cmp.l		#32000,d0
	beq		no_err
	
	move.w	#$ffff,24(a4)		; erreur
	movem.l	(sp)+,d0-d7/a0-a6
	rts

	move.w	#0,24(a4)		; no erreur
no_err:
;on copie l'image dans l'ecran de dest en tenant compte de la largeur de dest

	move.w	#199,d7			; taille y
	move.w	#319,d6			; taille x
	tst.w		(a5)
	beq		st2_bass
	move.w	#399,d7			; taille y
	move.w	#639,d6			; taille x
st2_bass:
	
	move.w	18(a4),d5		; largeur ecran dest
	sub.w		d6,d5				; le nb de pixels a saute a chaque fin de ligne
	subq		#1,d5
	lsl.w		#1,d5				; 16 bits/pixels
	move.l	14(a4),a0		; ecran dest
	lea	4(a5),a6			; la palette

	tst.w		(a5)
	bne		bcly1
	
boucley:
	move.w	#19,d4			; 20 paquets de 16
bouclex:
	movem.w	(a3)+,d0-d3		; les premiers 16 pixels
	move.w	#15,d6			; 16 pix
pix_4:
	and.l		#$ffff,d3
	lsl.l		d3					; voir STM115
	swap		d3
	lsl		d2
	roxl		d3
	lsl		d1
	roxl		d3
	lsl		d0
	roxl		d3
	move.w	(d3*2,a6),(a0)+
	swap		d3

	dbf		d6,pix_4
	
	dbf		d4,bouclex
	add.w		d5,a0				; le saut de pixel
	dbf		d7,boucley

	movem.l	(sp)+,d0-d7/a0-a6
	rts

bcly1:
	move.w	#39,d4			; 40 paquets de 16
bclx1:
	move.w	(a3)+,d0			; les premiers 16 pixels
	move.w	#15,d6			; 16 pix
pix_1:
	lsl		d0
	moveq		#0,d1
	roxl		d1
	beq		suite
	move.w	#$ffff,d1
suite:
	move.w	d1,(a0)+

	dbf		d6,pix_1
	
	dbf		d4,bclx1
	add.w		d5,a0				; le saut de pixel
	dbf		d7,bcly1

	movem.l	(sp)+,d0-d7/a0-a6
	rts



info:	dc.b  "Anim Fracland",0
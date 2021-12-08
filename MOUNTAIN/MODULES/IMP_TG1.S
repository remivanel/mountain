* CHARGEMENT TG1

;il a deux mode de fonctionnement, un mode optimis‚ o— il charge
;tout le fichier d'un coup dans le cas o— la largeur de l'image
;de destination egale celle du fichier.
;Un mode 'moins optims‚' dans le cas o— l'image a une largeur inf‚rieur
;a celle de dest (genre image 50*50  … charger dans un buffer 64*64),
;dans ce cas, elle est charg‚e puis recopi‚e ligne par ligne.


	dc.b	"TG1 "
	dc.b	"1.00"
	dc.l	10
	dc.l	$21			;flags
	bra.w	init.w
	bra.w	read.w
	
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
	
	move.l	(a5),d0
	cmp.l		#'COKE',d0
	beq		good
	
	move.w	#$FFFF,6(a4)			; c'est pas bon
	movem.l	(sp)+,d0-d7/a0-a5
	rts

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

; Fread position de l'img
	move.l	a5,d0
	addq		#4,d0
	move.l	d0,-(sp)
	move.l	#2,-(sp)
	move.w	(a4),-(sp)
	move.w	#63,-(sp)
	trap		#1
	lea		12(sp),sp

;calcul taille du fichier (juste l'image)
	moveq		#0,d6
	moveq		#0,d7
	move.w	(a5),d6			; taille x
	move.w	2(a5),d7			; taille y
	mulu.w	d7,d6
	lsl.l		#1,d6				; 16 bits/pixels
	move.l	d6,6(a5)			; on la stoque
	
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
	
	moveq		#0,d0
	move.w	4(a5),d0				; debut de l'image ds le fichier

;	Fseek au debut de l'img
	move.w	#0,-(sp)				;mode absolu
	move.w	(a4),-(sp)			;(a4)=handle
	move.l	d0,-(sp)				;debut
	move.w	#66,-(sp)
	trap		#1
	lea		10(sp),sp

	move.l	6(a5),d6			; taille du fic

	move.w	(a5),d0			;taille x
	move.w	18(a4),d1		;taille x ecran
	cmp.w		d1,d0
	beq		optimisation
	
	move.l	10(a4),a3		; buffer de destination
	
; Fread l'img
	move.l	a3,-(sp)			;dest
	move.l	d6,-(sp)			;la taillz
	move.w	(a4),-(sp)		;(a4)=handle
	move.w	#63,-(sp)
	trap		#1
	lea		12(sp),sp
	
	cmp.l		d0,d6
	beq		no_err1
	
	move.w	#$ffff,24(a4)
	movem.l	(sp)+,d0-d7/a0-a5
	rts

no_err1:	
	move.w	#0,24(a4)
;on copie l'image dans l'ecran de dest en tenant compte de la largeur de dest

	move.w	2(a5),d7			; taille y
	subq		#1,d7				; pour le dbf
	move.w	18(a4),d5		; largeur ecran dest
	move.w	(a5),d6			; taille x
	sub.w		d6,d5				; le nb de pixels a saute a chaque fin de ligne
	lsl.w		#1,d5				; 16 bits/pixels
	move.l	14(a4),a0		; ecran dest
boucley:
	move.w	(a5),d6			; taille x
	subq		#1,d6				; pour le dbf
bouclex:
	move.w	(a3)+,(a0)+
	dbf		d6,bouclex
	add.w		d5,a0				; le saut de pixel
	dbf		d7,boucley

	movem.l	(sp)+,d0-d7/a0-a5
	rts


optimisation:
; Fread l'img
	move.l	14(a4),-(sp)			;dest
	move.l	d6,-(sp)			;la taille
	move.w	(a4),-(sp)		;(a4)=handle
	move.w	#63,-(sp)
	trap		#1
	lea		12(sp),sp

	move.w	#0,24(a4)

	cmp.l		d0,d6
	beq		no_err2
	move.w	#$ffff,24(a4)		; erreur
no_err2:	

	movem.l	(sp)+,d0-d7/a0-a5
	rts


info:	dc.b  "Image TG1 COKE",0
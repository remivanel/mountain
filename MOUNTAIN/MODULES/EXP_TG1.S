* MODULE d'EXPORT TG1

IMPORT itoa
EXPORT errno

	dc.b	"ETG1"
	dc.b	"1.00"
	dc.l	30				; size buff
	dc.l	0				; size buff param
	dc.l	1				; flags
	bra.w	init.w
	bra.w	write.w
	bra.w	param.w
	bra.w	param.w
	
init:
	movem.l	d0-d7/a0-a5,-(sp)
	
	move.l	20(a0),a1		; buffer permanant
	
	move.w	#0,4(a0)			;ok
	move.l	#0,16(a0)		;pas de buffer
*prend les infos	
	move.w	6(a0),(a1)+		;x
	move.w	8(a0),(a1)+		;y
	move.l	12(a0),(a1)+	;num

	move.l	a1,a2
	move.l	(a0),a1			;nom src
	move.l	a2,a0				;copie dans le buffer permanent
	move.b	#0,(a0)
	bsr		cat
	
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
	

;********************* ROUTINE D'ECRITURE ******************

write:
	movem.l	d0-d7/a0-a5,-(sp)

	move.l	a0,a4				; sauvegarde

	move.l	18(a4),a5			; buffer de travail
	
	move.l	4(a5),d0			; nb d'img
	cmp.l		#1,d0				; 1 img : nom complet
	beq		nom_comp

;converti le num en texte
	move.l	(a4),d0			; num
	move.w	#10,d1			; radix
	lea		dum(PC),a0		; chaine tempo
	bsr		itoa

	lea		8(a5),a1			; le nom etait ds le buffer
	move.b	#0,4(a1)			; on tronque le nom
	lea		fic(pc),a0
	move.b	#0,(a0)
	bsr		cat				; copy le nom
	
	lea		dum(PC),a1		; chaine tempo (le num)
	bsr		cat				; copy le num
	bra		suite
	
nom_comp:
	lea		fic(pc),a0
	move.b	#0,(a0)
	lea		8(a5),a1		; le nom etait ds le buffer
	bsr.w		cat			; copy le nom
suite:
	lea		ext(pc),a1
	bsr.w		cat			; et l'ext
	
; Fcreate le fich
	move.w	#0,-(sp) ; archive
	move.l	a0,-(sp)
	move.w	#60,-(sp)
	trap		#1
	lea		8(sp),sp
	move.w	d0,d7				;handle
	bpl		ok_create
	
	move.w	#$ffff,16(a4)		; erreur d'ouverture
	movem.l	(sp)+,d0-d7/a0-a5
	rts	
	
ok_create:
	lea		tete(pc),a0
	move.w	(a5),12(a0)		;x
	move.w	2(a5),14(a0)	;y
	move.w	#18,16(a0)		;offset
	
;Fwrite l'entete
	move.l	a0,-(sp)			;dest
	move.l	#18,-(sp)		;la taille
	move.w	d7,-(sp)			;d7=handle
	move.w	#64,-(sp)
	trap		#1
	lea		12(sp),sp

;Taille de l'image
	move.w	(a5),d0		;x
	move.w	2(a5),d1		;y
	mulu.w	d1,d0			;d0<-x*y
	lsl.l		#1,d0			;16 bits
	move.l	d0,d6			; sauvegarde de la taille pour comparaison
	
;Fwrite l'image
	move.l	12(a4),-(sp)	;dest
	move.l	d0,-(sp)			;la taille
	move.w	d7,-(sp)			;d7=handle
	move.w	#64,-(sp)
	trap		#1
	lea		12(sp),sp
	cmp.l		d0,d6
	beq		ok_sauve

	move.w	#$ffff,16(a4)		; erreur de sauvegarde
	movem.l	(sp)+,d0-d7/a0-a5
	rts	
	
	
ok_sauve:

;Fclose le fic
	move.w	d7,-(sp)			;d7=handle
	move.w	#62,-(sp)
	trap		#1
	lea		4(sp),sp
	
	move.w	#0,16(a4)		; retour ok
	
	movem.l	(sp)+,d0-d7/a0-a5
	rts

param:
	rts


tete:		dc.b	"COKE format.",0,0,0,0,0,0
ext:		dc.b	".TG1",0,0
fic:		dc.b  "                "
dum:		dc.b  "                "
errno:	dc.l	0
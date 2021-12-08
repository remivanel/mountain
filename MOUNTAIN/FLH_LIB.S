MODE68000 = 1

	EXPORT intel
	EXPORT intell
	EXPORT copy
	EXPORT flh_delta
	EXPORT flh_brun
	EXPORT decomp
	EXPORT copy_tc
	EXPORT copy_tc2
	
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

intel:
	rol.w		#8,d0
	rts
	
intell:
	rol.w		#8,d0
	swap		d0
	rol.w		#8,d0
	rts


* (adr_source,adr_dest,lenght en word)

copy:
	move.w	(a0)+,(a1)+
	subq.l	#1,d0
	bne.s		copy
	
	rts

* (src,dst,nb_lignes,x, x to skip)
copy_tc:
	subq.w	#1,d0		; -1 pour dbf
	subq.w	#1,d1		; -1 pour dbf
	move.w	d1,-(sp)
	add.w		d2,d2
	
cpy:
	move.w	(a0)+,(a1)+
	dbf		d1,cpy

	add.w		d2,a1		;  ligne suivante
	move.w	(sp),d1
	
	dbf		d0,cpy
	
	addq.l	#2,sp
	
	rts


* (src,dst,nb_lignes,x, x to skip,x to skip sur src)
copy_tc2:
	move.l	d3,-(sp)		 ; save d3
	move.w	8(sp),d3		 ; dernier para
	subq.w	#1,d0		; -1 pour dbf
	subq.w	#1,d1		; -1 pour dbf
	move.w	d1,-(sp)
	add.w		d2,d2
	add.w		d3,d3
	
cpy2:
	move.w	(a0)+,(a1)+
	dbf		d1,cpy2

	add.w		d2,a1		;  ligne suivante
	add.w		d3,a0		;  ligne suivante
	move.w	(sp),d1
	
	dbf		d0,cpy2
	
	addq.l	#2,sp
	move.l	(sp)+,d3  ; restore
	rts
		

; FLH DELTA
; D0 -> x : largeur d'un image
; D1 -> y : h d'une image  ##inutilise##
; D2 -> x : largeur de l'ecran dest
; A0 -> ptr : pointeur sur le flh
; A1 -> img : destination
;*************************
; D1 -> lignes a decompac‚es
; D2 -> nb de packets
; D5 -> nb pix to skip for each line
; D6 -> x : largeur de l'ecran dest sav

flh_delta:
	movem.l	d0-d7/a0-a2,-(sp)	; save context	
	
	move.w	d2,d6
	move.w	d2,d5
	sub.w		d0,d5			* nb pix to skip
	add.w		d5,d5
	add.w		d6,d6

	and.l		#$0000ffff,d0
	add.w		d0,d0			* x *2 une fois pour toutes
	imovew	(a0)+,d1  	* nb de lignes a decomp
	beq		end_ln_delta * ca peut arriver...
	subq.w	#1,d1			* pour le dbf
	
	
loop_lignes:	
	imovew	(a0)+,d2
	bmi		ln_saut
	move.l	a1,a2			* sauve les ptr image

	* on va decompacter les d2 packets
	subq.w	#1,d2			* pour le dbf

loop_packets:
	moveq		#0,d3
	move.b	(a0)+,d3
	add.w		d3,d3			* nb de pixels a sauter
	add.l		d3,a1			* saut
	
	moveq		#0,d3
	move.b	(a0)+,d3		* compteur rle
	bmi		rle_pack
* now on a d3 pixel a copier direct
	beq		packet_suivant
	subq.w	#1,d3			* pour le dbf

nopackpix:
	move.w	(a0)+,(a1)+
	dbf		d3,nopackpix
	jmp		packet_suivant

rle_pack:
	moveq		#0,d4
	sub.b		d3,d4
;	beq		packet_suivant.w			* si flh mal pck
	subq.w	#1,d4			* pour le dbf
IFEQ MODE68000
	move.w	(a0)+,d3
unpackpix:
	move.w	d3,(a1)+
ELSE
	move.b	(a0)+,d3
	move.b	(a0)+,d7
unpackpix:
	move.b	d3,(a1)+
	move.b	d7,(a1)+
ENDIF

	dbf		d4,unpackpix.w

packet_suivant:
	dbf		d2,loop_packets.w
	
	add.l		d0,a2
	move.l	a2,a1		* recale sur la ligne suivante
	
	jmp		ligne_suivante
ln_saut:
	addq.w	#1,d1		* on a pas vraiment decompact‚ des lignes
	moveq		#0,d3		
	sub.w		d2,d3		* enleve le signe
	mulu.w	d6,d3		* packets * x = octets a sauter
	add.l		d3,a1		* saut
	dbf		d1,loop_lignes

ligne_suivante:
	add.w		d5,a1
	dbf		d1,loop_lignes

end_ln_delta:
	movem.l	(sp)+,d0-d7/a0-a2
	rts
	
; FLH BRUN
; D0 -> x : largeur d'un image
; D1 -> y : hauteur d'un image
; D2 -> x : largeur de l'ecran dest
; A0 -> ptr : pointeur sur le flh
; A1 -> img : destination
;*************************
; D2 -> nb de packets
; A2 -> dest + 1 ligne

flh_brun:
	movem.l	d0-d6/a0-a2,-(sp)	; save context	
	
	and.l		#$0000ffff,d0

	move.w	d2,d5
	sub.w		d0,d5			* nb pix to skip
	add.w		d5,d5
	
	add.w		d0,d0			* x *2 une fois pour toutes
	subq.w	#1,d1			* pour le dbf

loop_ligney:
; on dervrait faire :
;	moveq		#0,d2
;	move.b	(a0)+,d2		* nb_de packets
;	bne		no_apex
	addq.l	#1,a0

	* ON CONSIDERE QUE LES ANIM APEX, AVEC TOUJOURS 0 PACKETS
	
	move.l	a1,a2			
	add.w		d0,a2			* a2 contiend la fin de la ligne
	
loop_packety:
	moveq		#0,d3
	move.b	(a0)+,d3		* compteur rle
	bpl		rle_packy
* now on a -d3 pixel a copier direct

	moveq		#0,d4
	sub.b		d3,d4			* d4 = -d3
	subq.w	#1,d4			* pour le dbf
	
nopackpiy:
	move.w	(a0)+,(a1)+
	dbf		d4,nopackpiy
	jmp		packet_suivanty

rle_packy:
	subq.w	#1,d3			* pour le dbf
IFEQ MODE68000
	move.w	(a0)+,d4
unpackpiy:
	move.w	d4,(a1)+
ELSE
	move.b	(a0)+,d4
	move.b	(a0)+,d6
unpackpiy:
	move.b	d4,(a1)+
	move.b	d6,(a1)+
ENDIF
	dbf		d3,unpackpiy

packet_suivanty:
	cmp.l		a2,a1			* on a fini la ligne ??
	bmi		loop_packety

	move.l	a2,a1		* recale sur la ligne suivante
	
;no_apex:
	add.w		d5,a1
	dbf		d1,loop_ligney
	
	movem.l	(sp)+,d0-d6/a0-a2
	rts
	
	
;DECOMP une frame
; D0-> x
; D1-> y
; D2-> x : largeur de l'ecran dest
;------
; A0-> ptr FLH
; A1-> img

decomp:
	movem.l	d0-d7/a0-a6,-(sp)	; save context	
	
	and.l		#$0000ffff,d0
	and.l		#$0000ffff,d1
	and.l		#$0000ffff,d2
	
;	move.l	a0,a5				* save adr
	imovel	(a0)+,d3			* size frame
;	add.l		d3,a5				* fait point‚ la sauvegarde sur la next frame
	move.w	(a0)+,d6			* magic
	cmp.w		#$faf1,d6
	bne      end_decomp		* maivaise frame

	imovew	(a0)+,d6			* nombre de chunks
	beq		end_decomp		* !!! bug du 0 chunk, assez dangereux
	subq.w	#1,d6				* pour le dbf
	addq.l	#8,a0				* octets reserv‚s
	
loop_chunk:
	move.l	a0,a6				* sav adr chunk
	imovel	(a0)+,d4			* sizechunk
	add.l		d4,a6				* fait point‚ la sauvegarde sur le next chunk
	imovew	(a0)+,d5			* type chunk
	
;	move.l	a1,a4				* sav adr img
	
	cmp.w		#25,d5			* flh brun
	bne		decomp_delta
	jsr 		flh_brun
	jmp		end_chunk
	
decomp_delta:
	cmp.w		#27,d5			* flh delta
	bne		end_chunk
	jsr 		flh_delta
	
end_chunk:
	; restore les parms
	moveq		#0,d0				* possible de l'enlever
;	move.w	2(a2),d0			* x
	move.l	a6,a0				* adr next chunk
;	move.l	a4,a1				* adr ecran

	dbf		d6,loop_chunk
	
	
end_decomp:
;	move.l		a5,a0
	movem.l	(sp)+,d0-d7/a0-a6
	rts
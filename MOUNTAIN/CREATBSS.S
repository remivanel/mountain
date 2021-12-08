EXPORT create_bss

*A0 -> adr du module

create_bss:
	movem.l	d0-d6/a0-a3,-(sp)	; save context	
	move.l	a0,a3
	LEA 228(A3),A0			;pointeur entˆte (file header) de 28 octets
 	MOVE.L A3,(A3)			;pointeur d‚but page de base
 	CLR.L 4(A3)				;pointeur de fin de m‚moire libre
 	LEA 256(A3),A1
 	MOVE.L A1,8(A3)		;pointeur d‚but de programme
 	MOVE.L 2(A0),D0
 	MOVE.L D0,12(A3)		;taille du segment TEXT
 	ADD.L A1,D0
 	MOVE.L D0,16(A3)		;pointeur du d‚but de la zone de donn‚es
 	MOVE.L 6(A0),20(A3)	;taille du segment DATA
 	ADD.L 6(A0),D0
 	MOVE.L D0,24(A3)		;pointeur du d‚but de la zone BSS
 	MOVE.L 10(A0),28(A3)	;taille du segment BSS
 	MOVE.L 14(A0),D0		;longueur tableau symbolique
 	LEA 128(A3),A1
 	MOVE.L A1,32(A3)		;pointeur du tampon DTA
 	LEA 36(A3),A1
 	LEA 256(A3),A2
clr_base:
	CLR (A1)+				;efface la fin de la page de base
 	CMP.L A2,A1
 	BCS.S clr_base
 	MOVE.L A3,A0
 	bsr RELOGE_ECO
 	BSR INIT_BSS

	movem.l	(sp)+,d0-d6/a0-a3
	rts

INIT_BSS:

 	MOVEM.L 	D0/A0-A2,-(SP)
 	MOVE.L 	24(A0),A1		;pointeur du d‚but de la zone BSS
 	MOVE.L 	28(A0),D0		;taille du segment BSS
 	beq		no_bss
 	LEA 		(A1,D0.L),A2
clr_bss:
	CLR 		(A1)+				;efface la zone BSS
 	CMP.L 	A2,A1
 	BCS.S 	clr_bss
	MOVEM.L 	(SP)+,D0/A0-A2
no_bss:
 	RTS

RELOGE_ECO:					;A0:adresse page de base du module, D0:longueur tableau symbolique

 	MOVEM.L D0/A0-A1,-(SP)
 	ADD.L 12(A0),D0		;taille du segment TEXT
 	ADD.L 20(A0),D0		;taille du segment DATA
 	LEA 256(A0),A0			;d‚but du programme
 	MOVE.L A0,D1
 	LEA (A0,D0.L),A1		;table de relocation
 	ADD.L (A1)+,A0
 	ADD.L D1,(A0)
 	MOVEQ #0,D0
reloge_bcl:
	MOVE.B (A1)+,D0
  	BEQ.S fin_bcl					;fin
  	CMP.B #1,D0
  	BEQ.S jnsp_bcl
  	ADD D0,A0
  	ADD.L D1,(A0)
  	BRA.S reloge_bcl
jnsp_bcl:
	LEA 254(A0),A0
 	BRA.S reloge_bcl
fin_bcl:
	MOVEM.L (SP)+,D0/A0-A1
 	RTS

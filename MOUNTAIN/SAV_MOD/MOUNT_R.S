IMPORT	ext_exp_mplayer
IMPORT	ext_mod_export
EXPORT	errno	
EXPORT 	_FilSysVec	
IMPORT 	mount_r2
IMPORT 	mount_r3

	dc.l		'MNT1'
	dc.l		0				* checksum
	bra.w		ext_exp_mplayer.w
	bra.w		ext_mod_export.w
	move.l	mount_r2,d0
	move.l	mount_r3,d0

even
errno:		dc.w	0	
_FilSysVec:	dc.l	0
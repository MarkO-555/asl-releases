	cpu	ez8
	page	0

	org	1000h

	ADC	r5,r7		; 12 57
	ADC	r6,@r8		; 13 68
	ADC	34h,55h		; 14 55 34
	ADC	35h,@0AAh	; 15 AA 35
	ADC	36h,#31h	; 16 36 31
	ADC	@37h,#32h	; 17 37 32
	ADCX	351h,456h	; 18 45 63 51
	ADCX	364h,#35h	; 19 35 03 64
	ADD	r5,r7		; 02 57
	ADD	r6,@r8		; 03 68
	ADD	34h,55h		; 04 55 34
	ADD	35h,@0AAh	; 05 AA 35
	ADD	36h,#31h	; 06 36 31
	ADD	@37h,#32h	; 07 37 32
	ADDX	351h,456h	; 08 45 63 51
	ADDX	364h,#35h	; 09 35 03 64
	AND	r5,r7		; 52 57
	AND	r6,@r8		; 53 68
	AND	34h,55h		; 54 55 34
	AND	35h,@0AAh	; 55 AA 35
	AND	36h,#31h	; 56 36 31
	AND	@37h,#32h	; 57 37 32
	ANDX	351h,456h	; 58 45 63 51
	ANDX	364h,#35h	; 59 35 03 64
	BCLR	3,r5		; E2 35
	BIT	0,3,r5		; E2 35
	BIT	1,3,r5		; E2 B5
	BRK			; 00
	BSET	3,r5		; E2 B5
	BSWAP	54h		; D5 54
LABEL1:
	BTJ	0,2,r7,LABEL1	; F6 27 FD
	BTJ	1,3,r6,LABEL1	; F6 B6 FA
	BTJ	0,2,@r7,LABEL1	; F7 27 F7
	BTJ	1,3,@r6,LABEL1	; F7 B6 F4
	BTJNZ	3,@r6,LABEL1	; F7 B6 F1
	BTJNZ	3,r6,LABEL1	; F6 B6 EE
	BTJZ	2,r7,LABEL1	; F6 27 EB
	BTJZ	2,@r7,LABEL1	; F7 27 E8
	CALL	@34h		; D4 34
	CALL	3456h		; D6 34 56
	CCF			; EF
	CLR	98h		; B0 98
	CLR	@35h		; B1 35
	COM	78h		; 60 78
	COM	@54h		; 61 54
	CP	r5,r7		; A2 57
	CP	r6,@r8		; A3 68
	CP	34h,55h		; A4 55 34
	CP	35h,@0AAh	; A5 AA 35
	CP	36h,#31h	; A6 36 31
	CP	@37h,#32h	; A7 37 32
	CPC	r5,r7		; 1F A2 57
	CPC	r6,@r8		; 1F A3 68
	CPC	34h,55h		; 1F A4 55 34
	CPC	35h,@0AAh	; 1F A5 AA 35
	CPC	36h,#31h	; 1F A6 36 31
	CPC	@37h,#32h	; 1F A7 37 32
	CPCX	351h,456h	; 1F A8 45 63 51
	CPCX	364h,#35h	; 1F A9 35 03 64
	CPX	351h,456h	; A8 45 63 51
	CPX	364h,#35h	; A9 35 03 64
	DA	34h		; 40 34
	DA	@43h		; 41 43
	DEC	56h		; 30 56
	DEC	@41h		; 31 41
	DECW	34h		; 80 34
	DECW	@44h		; 81 44
	DI			; 8F
LABEL2:
	DJNZ	r0,LABEL2	; 0A FE
	DJNZ	r1,LABEL2	; 1A FC
	DJNZ	r2,LABEL2	; 2A FA
	DJNZ	r3,LABEL2	; 3A F8
	DJNZ	r4,LABEL2	; 4A F6
	DJNZ	r5,LABEL2	; 5A F4
	DJNZ	r6,LABEL2	; 6A F2
	DJNZ	r7,LABEL2	; 7A F0
	DJNZ	r8,LABEL2	; 8A EE
	DJNZ	r9,LABEL2	; 9A EC
	DJNZ	r10,LABEL2	; AA EA
	DJNZ	r11,LABEL2	; BA E8
	DJNZ	r12,LABEL2	; CA E6
	DJNZ	r13,LABEL2	; DA E4
	DJNZ	r14,LABEL2	; EA E2
	DJNZ	r15,LABEL2	; FA E0
	EI			; 9F
	HALT			; 7F
	INC	46h		; 20 46
	INC	@34h		; 21 34
	INC	r0		; 0E
	INC	r1		; 1E
	INC	r2		; 2E
	INC	r3		; 3E
	INC	r4		; 4E
	INC	r5		; 5E
	INC	r6		; 6E
	INC	r7		; 7E
	INC	r8		; 8E
	INC	r9		; 9E
	INC	r10		; AE
	INC	r11		; BE
	INC	r12		; CE
	INC	r13		; DE
	INC	r14		; EE
	INC	r15		; FE
	INCW	34h		; A0 34
	INCW	@48h		; A1 48
	IRET			; BF
	JP	@rr4		; C4 E4
	JP	0F818h		; 8D F8 18
	JP	F,0F010h	; 0D F0 10
	JP	LT,0F111h	; 1D F1 11
	JP	LE,0F212h	; 2D F2 12
	JP	ULE,0F313h	; 3D F3 13
	JP	OV,0F414h	; 4D F4 14
	JP	MI,0F515h	; 5D F5 15
	JP	Z,0F616h	; 6D F6 16
	JP	C,0F717h	; 7D F7 17
	JP	T,0F818h	; 8D F8 18
	JP	GE,0F919h	; 9D F9 19
	JP	GT,0FA1Ah	; AD FA 1A
	JP	UGT,0FB1Bh	; BD FB 1B
	JP	NOV,0FC1Ch	; CD FC 1C
	JP	PL,0FD1Dh	; DD FD 1D
	JP	NZ,0FE1Eh	; ED FE 1E
	JP	NC,0FF1Fh	; FD FF 1F
	JR	LABEL3		; 8B 20
	JR	F,LABEL3	; 0B 1E
	JR	LT,LABEL3	; 1B 1C
	JR	LE,LABEL3	; 2B 1A
	JR	ULE,LABEL3	; 3B 18
	JR	OV,LABEL3	; 4B 16
	JR	MI,LABEL3	; 5B 14
	JR	Z,LABEL3	; 6B 12
	JR	C,LABEL3	; 7B 10
	JR	T,LABEL3	; 8B 0E
	JR	GE,LABEL3	; 9B 0C
	JR	GT,LABEL3	; AB 0A
	JR	UGT,LABEL3	; BB 08
	JR	NOV,LABEL3	; CB 06
	JR	PL,LABEL3	; DB 04
	JR	NZ,LABEL3	; EB 02
	JR	NC,LABEL3	; FB 00
LABEL3:
	LD	r0,#30h		; 0C 30
	LD	r1,#31h		; 1C 31
	LD	r2,#32h		; 2C 32
	LD	r3,#33h		; 3C 33
	LD	r4,#34h		; 4C 34
	LD	r5,#35h		; 5C 35
	LD	r6,#36h		; 6C 36
	LD	r7,#37h		; 7C 37
	LD	r8,#38h		; 8C 38
	LD	r9,#39h		; 9C 39
	LD	r10,#3Ah	; AC 3A
	LD	r11,#3Bh	; BC 3B
	LD	r12,#3Ch	; CC 3C
	LD	r13,#3Dh	; DC 3D
	LD	r14,#3Eh	; EC 3E
	LD	r15,#3Fh	; FC 3F
	LD	r3,3h(r6)	; C7 36 03
	LD	5h(r4),r7	; D7 74 05
	LD	r5,@r7		; E3 57
	LD	34h,55h		; E4 55 34
	LD	35h,@0AAh	; E5 AA 35
	LD	36h,#31h	; E6 36 31
	LD	@37h,#32h	; E7 37 32
	LD	@r7,r0		; F3 70
	LD	@25h,71h	; F5 71 25
	LDC	r4,@rr6		; C2 46
	LDC	@r5,@rr6	; C5 56
	LDC	@rr6,r4		; D2 46
	LDCI	@r7,@rr8	; C3 78
	LDCI	@rr6,@r8	; D3 86
	LDE	r5,@rr8		; 82 58
	LDE	@rr2,r5		; 92 52
	LDEI	@r6,@rr10	; 83 6A
	LDEI	@rr14,@r3	; 93 3E
	LD	r1,0E3h(r6)	; C7 16 E3
	LD	10h(r8),r6	; D7 68 10
	LDX	r3,876h		; E8 87 6E E3
	LDX	@r4,564h	; 85 45 64
	LDX	34h,@56h	; 86 56 34
	LDX	@12h,@.RR(09h)	; 87 E8 12 <-> 87 09 12 ?!
	LDX	r4,21h(rr2)	; 88 42 21
	LDX	92h(rr14),r0	; 89 E0 92
	LDX	345h,r6		; 94 63 45
	LDX	347h,@r6	; 95 63 47
	LDX	@rr10,r1	; 96 E1 EA
	LDX	@.RR(13h),@0B4h	; 97 B4 E2 <-> 97 B4 13 ?!
	LDX	351h,456h	; E8 45 63 51
	LDX	364h,#35h	; E9 35 03 64
	LEA	r3,0F4h(r4)	; 98 34 F4
	LEA	rr2,10h(rr4)	; 99 24 10
	MULT	0CCh		; F4 CC
	NOP			; 0F
	OR	r5,r7		; 42 57
	OR	r6,@r8		; 43 68
	OR	34h,55h		; 44 55 34
	OR	35h,@0AAh	; 45 AA 35
	OR	36h,#31h	; 46 36 31
	OR	@37h,#32h	; 47 37 32
	ORX	351h,456h	; 48 45 63 51
	ORX	364h,#35h	; 49 35 03 64
	POP	46h		; 50 46
	POP	@35h		; 51 35
	POPX	543h		; D8 54 30
	PUSH	54h		; 70 54
	PUSH	@34h		; 71 34
	PUSHX	345h		; C8 34 50
	RCF			; CF
	RET			; AF
	RL	35h		; 90 35
	RL	@44h		; 91 44
	RLC	35h		; 10 35
	RLC	@44h		; 11 44
	RR	20h		; E0 20
	RR	@46h		; E1 46
	RRC	20h		; C0 20
	RRC	@46h		; C1 46
	SBC	r5,r7		; 32 57
	SBC	r6,@r8		; 33 68
	SBC	34h,55h		; 34 55 34
	SBC	35h,@0AAh	; 35 AA 35
	SBC	36h,#31h	; 36 36 31
	SBC	@37h,#32h	; 37 37 32
	SBCX	351h,456h	; 38 45 63 51
	SBCX	364h,#35h	; 39 35 03 64
	SCF			; DF
	SRA	43h		; D0 43
	SRA	@67h		; D1 67
	SRL	41h		; 1F C0 41
	SRL	@67h		; 1F C1 67
	SRP	#35h		; 01 35
	STOP			; 6F
	SUB	r5,r7		; 22 57
	SUB	r6,@r8		; 23 68
	SUB	34h,55h		; 24 55 34
	SUB	35h,@0AAh	; 25 AA 35
	SUB	36h,#31h	; 26 36 31
	SUB	@37h,#32h	; 27 37 32
	SUBX	351h,456h	; 28 45 63 51
	SUBX	364h,#35h	; 29 35 03 64
	SWAP	56h		; F0 56
	SWAP	@89h		; F1 89
	TCM	r5,r7		; 62 57
	TCM	r6,@r8		; 63 68
	TCM	34h,55h		; 64 55 34
	TCM	35h,@0AAh	; 65 AA 35
	TCM	36h,#31h	; 66 36 31
	TCM	@37h,#32h	; 67 37 32
	TCMX	351h,456h	; 68 45 63 51
	TCMX	364h,#35h	; 69 35 03 64
	TM	r5,r7		; 72 57
	TM	r6,@r8		; 73 68
	TM	34h,55h		; 74 55 34
	TM	35h,@0AAh	; 75 AA 35
	TM	36h,#31h	; 76 36 31
	TM	@37h,#32h	; 77 37 32
	TMX	351h,456h	; 78 45 63 51
	TMX	364h,#35h	; 79 35 03 64
	TRAP	#35h		; F2 35
	WDT			; 5F
	XOR	r5,r7		; B2 57
	XOR	r6,@r8		; B3 68
	XOR	34h,55h		; B4 55 34
	XOR	35h,@0AAh	; B5 AA 35
	XOR	36h,#31h	; B6 36 31
	XOR	@37h,#32h	; B7 37 32
	XORX	351h,456h	; B8 45 63 51
	XORX	364h,#35h	; B9 35 03 64
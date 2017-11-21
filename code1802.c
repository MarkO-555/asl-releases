/* code1802.c */
/****************************************************************************/
/* AS                                                                       */
/* Code Generator for Intersil 1802                                         */
/*                                                                          */
/* History: 27. 1.2001 begun                                                */
/*           3. 2.2001 added 1805 instructions                              */
/*                                                                          */
/****************************************************************************/
/* $Id: code1802.c,v 1.9 2017/01/29 20:52:51 alfred Exp $                   */
/****************************************************************************
 * $Log: code1802.c,v $
 * Revision 1.9  2017/01/29 20:52:51  alfred
 * - clean up 18xx CPU types
 *
 * Revision 1.8  2017/01/08 12:36:02  alfred
 * - add 1804
 *
 * Revision 1.7  2017/01/08 10:45:28  alfred
 * - differentiate 1805 and 1805A instructions
 *
 * Revision 1.6  2017/01/08 10:27:07  alfred
 * - correct SCAL instruction
 *
 * Revision 1.5  2014/10/03 17:33:49  alfred
 * - adapt to current stype
 *
 * Revision 1.4  2007/11/27 11:24:39  alfred
 * - some instruction fixes
 *
 * Revision 1.3  2005/09/08 16:53:39  alfred
 * - use common PInstTable
 *
 * Revision 1.2  2004/05/29 11:33:00  alfred
 * - relocated DecodeIntelPseudo() into own module
 *
 ****************************************************************************/

#include "stdinc.h"
#include <string.h>

#include "bpemu.h"
#include "asmdef.h"
#include "asmsub.h"
#include "asmpars.h"
#include "asmitree.h"
#include "codepseudo.h"
#include "codevars.h"
#include "intpseudo.h"
#include "headids.h"
#include "errmsg.h"

#include "code1802.h"

/*-------------------------------------------------------------------------*/
/* Variables */

#define FixedOrderCnt 55
#define RegOrderCnt 14
#define RegNoZeroOrderCnt 1
#define RegImm16OrderCnt 1
#define RegLBranchOrderCnt 2
#define ImmOrderCnt 14
#define SBranchOrderCnt 22
#define LBranchOrderCnt 8
#define IOOrderCnt 2

typedef struct
{
  Word Code;
  CPUVar MinCPU;
} tOrder;

static tOrder *FixedOrders, *RegOrders, *RegNoZeroOrders,
              *RegImm16Orders, *RegLBranchOrders, *ImmOrders,
              *SBranchOrders, *LBranchOrders, *IOOrders;

static CPUVar CPU1802,
              CPU1804, CPU1805, CPU1806,
              CPU1804A, CPU1805A, CPU1806A;

/*-------------------------------------------------------------------------*/
/* Subroutines */

static Boolean PutCode(const tOrder *pOrder)
{
  if (ChkMinCPU(pOrder->MinCPU))
  {
    if (Hi(pOrder->Code))
      BAsmCode[CodeLen++] = Hi(pOrder->Code);
    BAsmCode[CodeLen++] = Lo(pOrder->Code);
    return True;
  }
  else
    return False;
}

/*-------------------------------------------------------------------------*/
/* Coders */

static void DecodePORT(Word Opcode)
{
  UNUSED(Opcode);

  CodeEquate(SegIO, 1, 0x7);
}

static void DecodeFixed(Word Index)
{
  const tOrder *pOrder = FixedOrders + Index;

  if (ChkArgCnt(0, 0))
    PutCode(pOrder);
}

static void DecodeReg(Word Index)
{
  const tOrder *pOrder = RegOrders + Index;
  Boolean OK;

  if (ChkArgCnt(1, 1)
   && PutCode(pOrder))
  {
    BAsmCode[CodeLen - 1] |= EvalIntExpression(ArgStr[1], UInt4, &OK);
    if (!OK)
      CodeLen = 0;
  }
}

static void DecodeRegNoZero(Word Index)
{
  const tOrder *pOrder = RegNoZeroOrders + Index;

  if (ChkArgCnt(1, 1)
   && PutCode(pOrder))
  {
    Boolean OK;
    Byte Reg;

    FirstPassUnknown = FALSE;
    Reg = EvalIntExpression(ArgStr[1], UInt4, &OK);
    if (!OK) CodeLen = 0;
    else if ((!FirstPassUnknown) && (0 == Reg))
    {
      WrXError(1445, ArgStr[1]);
      CodeLen = 0;
    }
    else
      BAsmCode[CodeLen - 1] |= Reg;
  }
}

static void DecodeRegImm16(Word Index)
{
  const tOrder *pOrder = RegImm16Orders + Index;
  Boolean OK;

  if (ChkArgCnt(2, 2)
   && PutCode(pOrder))
  {
    BAsmCode[CodeLen - 1] |= EvalIntExpression(ArgStr[1], UInt4, &OK);
    if (!OK) CodeLen = 0;
    else
    {
      Word Arg = EvalIntExpression(ArgStr[2], Int16, &OK);

      if (!OK) CodeLen = 0;
      else
      {
        BAsmCode[CodeLen++] = Hi(Arg);
        BAsmCode[CodeLen++] = Lo(Arg);
      }
    }
  }
}

static void DecodeRegLBranch(Word Index)
{
  const tOrder *pOrder = RegLBranchOrders + Index;
  Boolean OK;
  Word Addr;

  if (ChkArgCnt(2, 2)
   && PutCode(pOrder))
  {
    BAsmCode[CodeLen - 1] |= EvalIntExpression(ArgStr[1], UInt4, &OK);
    if (!OK) CodeLen = 0;
    else
    {
      Addr = EvalIntExpression(ArgStr[2], UInt16, &OK);
      if (!OK)
        CodeLen = 0;
      else
      {
        ChkSpace(SegCode);
        BAsmCode[CodeLen++] = Hi(Addr);
        BAsmCode[CodeLen++] = Lo(Addr);
      }
    }
  }
}

static void DecodeImm(Word Index)
{
  const tOrder *pOrder = ImmOrders + Index;
  Boolean OK;

  if (ChkArgCnt(1, 1)
   && PutCode(pOrder))
  {
    BAsmCode[CodeLen++] = EvalIntExpression(ArgStr[1], Int8, &OK);
    if (!OK) CodeLen = 0;
  }
}

static void DecodeSBranch(Word Index)
{
  const tOrder *pOrder = SBranchOrders + Index;
  Word Addr;
  Boolean OK;

  if (ChkArgCnt(1, 1)
   && PutCode(pOrder))
  {
    Addr = EvalIntExpression(ArgStr[1], UInt16, &OK);
    if (!OK) CodeLen = 0;
    else
    {
      if ((Hi(EProgCounter() + 1) != Hi(Addr)) && (!SymbolQuestionable))
      {
        WrError(1910);
        CodeLen = 0;
      }
      else
      {
        ChkSpace(SegCode);
        BAsmCode[CodeLen++] = Lo(Addr);
      }
    }
  }
}

static void DecodeLBranch(Word Index)
{
  const tOrder *pOrder = LBranchOrders + Index;
  Word Addr;
  Boolean OK;

  if (ChkArgCnt(1, 1)
   && PutCode(pOrder))
  {
    Addr = EvalIntExpression(ArgStr[1], UInt16, &OK);
    if (!OK) CodeLen = 0;
    {
      ChkSpace(SegCode);
      BAsmCode[CodeLen++] = Hi(Addr);
      BAsmCode[CodeLen++] = Lo(Addr);
    }
  }
}

static void DecodeIO(Word Index)
{
  const tOrder *pOrder = IOOrders + Index;
  Boolean OK;

  if (ChkArgCnt(1, 1))
  {
    FirstPassUnknown = False;
    BAsmCode[0] = EvalIntExpression(ArgStr[1], UInt3, &OK);
    if (OK)
    {
      if (FirstPassUnknown) BAsmCode[0] = 1;
      if (BAsmCode[0] == 0) WrError(1315);
      else
      {
        BAsmCode[0] |= pOrder->Code;
        CodeLen = 1;
      }
    }
  }
}

/*-------------------------------------------------------------------------*/
/* Instruction Hash Table Handling */

static void AddFixed(const char *pName, Word Code, CPUVar MinCPU)
{
  if (InstrZ >= FixedOrderCnt) exit(255);
  FixedOrders[InstrZ].Code = Code;
  FixedOrders[InstrZ].MinCPU = MinCPU;
  AddInstTable(InstTable, pName, InstrZ++, DecodeFixed);
}

static void AddReg(const char *pName, Word Code, CPUVar MinCPU)
{
  if (InstrZ >= RegOrderCnt) exit(255);
  RegOrders[InstrZ].Code = Code;
  RegOrders[InstrZ].MinCPU = MinCPU;
  AddInstTable(InstTable, pName, InstrZ++, DecodeReg);
}

static void AddRegNoZero(const char *pName, Word Code, CPUVar MinCPU)
{
  if (InstrZ >= RegNoZeroOrderCnt) exit(255);
  RegNoZeroOrders[InstrZ].Code = Code;
  RegNoZeroOrders[InstrZ].MinCPU = MinCPU;
  AddInstTable(InstTable, pName, InstrZ++, DecodeRegNoZero);
}

static void AddRegImm16(const char *pName, Word Code, CPUVar MinCPU)
{
  if (InstrZ >= RegImm16OrderCnt) exit(255);
  RegImm16Orders[InstrZ].Code = Code;
  RegImm16Orders[InstrZ].MinCPU = MinCPU;
  AddInstTable(InstTable, pName, InstrZ++, DecodeRegImm16);
}

static void AddImm(const char *pName, Word Code, CPUVar MinCPU)
{
  if (InstrZ >= ImmOrderCnt) exit(255);
  ImmOrders[InstrZ].Code = Code;
  ImmOrders[InstrZ].MinCPU = MinCPU;
  AddInstTable(InstTable, pName, InstrZ++, DecodeImm);
}

static void AddRegLBranch(const char *pName, Word Code, CPUVar MinCPU)
{
  if (InstrZ >= RegLBranchOrderCnt) exit(255);
  RegLBranchOrders[InstrZ].Code = Code;
  RegLBranchOrders[InstrZ].MinCPU = MinCPU;
  AddInstTable(InstTable, pName, InstrZ++, DecodeRegLBranch);
}

static void AddLBranch(const char *pName, Word Code, CPUVar MinCPU)
{
  if (InstrZ >= LBranchOrderCnt) exit(255);
  LBranchOrders[InstrZ].Code = Code;
  LBranchOrders[InstrZ].MinCPU = MinCPU;
  AddInstTable(InstTable, pName, InstrZ++, DecodeLBranch);
}

static void AddSBranch(const char *pName, Word Code, CPUVar MinCPU)
{
  if (InstrZ >= SBranchOrderCnt) exit(255);
  SBranchOrders[InstrZ].Code = Code;
  SBranchOrders[InstrZ].MinCPU = MinCPU;
  AddInstTable(InstTable, pName, InstrZ++, DecodeSBranch);
}

static void AddIO(const char *pName, Word Code, CPUVar MinCPU)
{
  if (InstrZ >= IOOrderCnt) exit(255);
  IOOrders[InstrZ].Code = Code;
  IOOrders[InstrZ].MinCPU = MinCPU;
  AddInstTable(InstTable, pName, InstrZ++, DecodeIO);
}

static void InitFields(void)
{
  InstTable = CreateInstTable(203);

  AddInstTable(InstTable, "PORT", 0, DecodePORT);

  FixedOrders = (tOrder*)calloc(FixedOrderCnt, sizeof(*FixedOrders)); InstrZ = 0;
  AddFixed("LDX"  , 0x00f0, CPU1802);
  AddFixed("LDXA" , 0x0072, CPU1802);
  AddFixed("STXD" , 0x0073, CPU1802);
  AddFixed("IRX"  , 0x0060, CPU1802);
  AddFixed("OR"   , 0x00f1, CPU1802);
  AddFixed("XOR"  , 0x00f3, CPU1802);
  AddFixed("AND"  , 0x00f2, CPU1802);
  AddFixed("SHR"  , 0x00f6, CPU1802);
  AddFixed("SHRC" , 0x0076, CPU1802);
  AddFixed("RSHR" , 0x0076, CPU1802);
  AddFixed("SHL"  , 0x00fe, CPU1802);
  AddFixed("SHLC" , 0x007e, CPU1802);
  AddFixed("RSHL" , 0x007e, CPU1802);
  AddFixed("ADD"  , 0x00f4, CPU1802);
  AddFixed("ADC"  , 0x0074, CPU1802);
  AddFixed("SD"   , 0x00f5, CPU1802);
  AddFixed("SDB"  , 0x0075, CPU1802);
  AddFixed("SM"   , 0x00f7, CPU1802);
  AddFixed("SMB"  , 0x0077, CPU1802);
  AddFixed("DADD" , 0x68f4, CPU1804A);
  AddFixed("DADC" , 0x6874, CPU1804A);
  AddFixed("DSM"  , 0x68f7, CPU1804A);
  AddFixed("DSMB" , 0x6877, CPU1804A);
  AddFixed("SKP"  , 0x0038, CPU1802);
  AddFixed("LSKP" , 0x00c8, CPU1802);
  AddFixed("LSZ"  , 0x00ce, CPU1802);
  AddFixed("LSNZ" , 0x00c6, CPU1802);
  AddFixed("LSDF" , 0x00cf, CPU1802);
  AddFixed("LSNF" , 0x00c7, CPU1802);
  AddFixed("LSQ"  , 0x00cd, CPU1802);
  AddFixed("LSNQ" , 0x00c5, CPU1802);
  AddFixed("LSIE" , 0x00cc, CPU1802);
  AddFixed("IDL"  , 0x0000, CPU1802);
  AddFixed("NOP"  , 0x00c4, CPU1802);
  AddFixed("SEQ"  , 0x007b, CPU1802);
  AddFixed("REQ"  , 0x007a, CPU1802);
  AddFixed("SAV"  , 0x0078, CPU1802);
  AddFixed("MARK" , 0x0079, CPU1802);
  AddFixed("RET"  , 0x0070, CPU1802);
  AddFixed("DIS"  , 0x0071, CPU1802);
  AddFixed("LDC"  , 0x6806, CPU1804);
  AddFixed("GEC"  , 0x6808, CPU1804);
  AddFixed("STPC" , 0x6800, CPU1804);
  AddFixed("DTC"  , 0x6801, CPU1804);
  AddFixed("STM"  , 0x6807, CPU1804);
  AddFixed("SCM1" , 0x6805, CPU1804);
  AddFixed("SCM2" , 0x6803, CPU1804);
  AddFixed("SPM1" , 0x6804, CPU1804);
  AddFixed("SPM2" , 0x6802, CPU1804);
  AddFixed("ETQ"  , 0x6809, CPU1804);
  AddFixed("XIE"  , 0x680a, CPU1804);
  AddFixed("XID"  , 0x680b, CPU1804);
  AddFixed("CIE"  , 0x680c, CPU1804);
  AddFixed("CID"  , 0x680d, CPU1804);
  AddFixed("DSAV" , 0x6876, CPU1804A);

  RegOrders = (tOrder*)calloc(RegOrderCnt, sizeof(*RegOrders)); InstrZ = 0;
  AddReg("LDA"  , 0x0040, CPU1802);
  AddReg("STR"  , 0x0050, CPU1802);
  AddReg("RLXA" , 0x6860, CPU1804);
  AddReg("RSXD" , 0x68a0, CPU1804);
  AddReg("INC"  , 0x0010, CPU1802);
  AddReg("DEC"  , 0x0020, CPU1802);
  AddReg("GLO"  , 0x0080, CPU1802);
  AddReg("PLO"  , 0x00a0, CPU1802);
  AddReg("GHI"  , 0x0090, CPU1802);
  AddReg("PHI"  , 0x00b0, CPU1802);
  AddReg("RNX"  , 0x68b0, CPU1804);
  AddReg("SEP"  , 0x00d0, CPU1802);
  AddReg("SEX"  , 0x00e0, CPU1802);
  AddReg("SRET" , 0x6890, CPU1804);

  RegNoZeroOrders = (tOrder*)calloc(RegNoZeroOrderCnt, sizeof(*RegNoZeroOrders)); InstrZ = 0;
  AddRegNoZero("LDN"  , 0x0000, CPU1802);

  RegImm16Orders = (tOrder*)calloc(RegImm16OrderCnt, sizeof(*RegImm16Orders)); InstrZ = 0;
  AddRegImm16("RLDI" , 0x68c0, CPU1804);

  ImmOrders = (tOrder*)calloc(ImmOrderCnt, sizeof(*ImmOrders)); InstrZ = 0;
  AddImm("LDI"  , 0x00f8, CPU1802);
  AddImm("ORI"  , 0x00f9, CPU1802);
  AddImm("XRI"  , 0x00fb, CPU1802);
  AddImm("ANI"  , 0x00fa, CPU1802);
  AddImm("ADI"  , 0x00fc, CPU1802);  
  AddImm("ADCI" , 0x007c, CPU1802);
  AddImm("SDI"  , 0x00fd, CPU1802);
  AddImm("SDBI" , 0x007d, CPU1802);  
  AddImm("SMI"  , 0x00ff, CPU1802);  
  AddImm("SMBI" , 0x007f, CPU1802);
  AddImm("DADI" , 0x68fc, CPU1804A);
  AddImm("DACI" , 0x687c, CPU1804A);
  AddImm("DSMI" , 0x68ff, CPU1804A);
  AddImm("DSBI" , 0x687f, CPU1804A);

  RegLBranchOrders = (tOrder*)calloc(RegLBranchOrderCnt, sizeof(*RegLBranchOrders)); InstrZ = 0;
  AddRegLBranch("DBNZ" , 0x6820, CPU1804A);
  AddRegLBranch("SCAL" , 0x6880, CPU1804);

  LBranchOrders = (tOrder*)calloc(LBranchOrderCnt, sizeof(*LBranchOrders)); InstrZ = 0;
  AddLBranch("LBR"  , 0x00c0, CPU1802);
  AddLBranch("NLBR" , 0x00c8, CPU1802);
  AddLBranch("LBZ"  , 0x00c2, CPU1802);
  AddLBranch("LBNZ" , 0x00ca, CPU1802);
  AddLBranch("LBDF" , 0x00c3, CPU1802);
  AddLBranch("LBNF" , 0x00cb, CPU1802);
  AddLBranch("LBQ"  , 0x00c1, CPU1802);
  AddLBranch("LBNQ" , 0x00c9, CPU1802);

  SBranchOrders = (tOrder*)calloc(SBranchOrderCnt, sizeof(*SBranchOrders)); InstrZ = 0;
  AddSBranch("BR"   , 0x0030, CPU1802);
  AddSBranch("NBR"  , 0x0038, CPU1802);
  AddSBranch("BZ"   , 0x0032, CPU1802);
  AddSBranch("BNZ"  , 0x003a, CPU1802);
  AddSBranch("BDF"  , 0x0033, CPU1802);
  AddSBranch("BPZ"  , 0x0033, CPU1802);
  AddSBranch("BGE"  , 0x0033, CPU1802);
  AddSBranch("BNF"  , 0x003b, CPU1802);
  AddSBranch("BM"   , 0x003b, CPU1802);
  AddSBranch("BL"   , 0x003b, CPU1802);
  AddSBranch("BQ"   , 0x0031, CPU1802);
  AddSBranch("BNQ"  , 0x0039, CPU1802);
  AddSBranch("B1"   , 0x0034, CPU1802);
  AddSBranch("BN1"  , 0x003c, CPU1802);
  AddSBranch("B2"   , 0x0035, CPU1802);
  AddSBranch("BN2"  , 0x003d, CPU1802);
  AddSBranch("B3"   , 0x0036, CPU1802);
  AddSBranch("BN3"  , 0x003e, CPU1802);
  AddSBranch("B4"   , 0x0037, CPU1802);
  AddSBranch("BN4"  , 0x003f, CPU1802);
  AddSBranch("BCI"  , 0x683e, CPU1804);
  AddSBranch("BXI"  , 0x683f, CPU1804);

  IOOrders = (tOrder*)calloc(IOOrderCnt, sizeof(*IOOrders)); InstrZ = 0;
  AddIO("OUT"  , 0x0060, CPU1802);
  AddIO("INP"  , 0x0068, CPU1802);
}

static void DeinitFields(void)
{
  free(FixedOrders);
  free(RegOrders);
  free(RegNoZeroOrders);
  free(RegImm16Orders);
  free(RegLBranchOrders);
  free(ImmOrders);
  free(SBranchOrders);
  free(LBranchOrders);
  free(IOOrders);

  DestroyInstTable(InstTable);
}

/*-------------------------------------------------------------------------*/
/* Interface to Upper Level */

static void MakeCode_1802(void)
{
  CodeLen = 0; DontPrint = False;

  /* to be ignored */

  if (*OpPart == '\0') return;

  /* Pseudo Instructions */

  if (DecodeIntelPseudo(True)) return;

  /* search */

  if (!LookupInstTable(InstTable, OpPart))
    WrXError(1200, OpPart);
}

static Boolean IsDef_1802(void)
{
  return Memo("PORT");
}

static void SwitchFrom_1802(void)
{
  DeinitFields();
}

static void SwitchTo_1802(void)
{
  PFamilyDescr FoundDescr;

  FoundDescr = FindFamilyByName("1802");

  TurnWords = FALSE;
  ConstMode = ConstModeIntel;
  SetIsOccupied = False;

  PCSymbol = "$";
  HeaderID = FoundDescr->Id;
  NOPCode = 0xc4;
  DivideChars = ",";
  HasAttrs = False;

  ValidSegs = (1 << SegCode) | (1 << SegIO);
  Grans[SegCode ] = 1; ListGrans[SegCode ] = 1; SegInits[SegCode ] = 0;
  SegLimits[SegCode ] = 0xffff;
  Grans[SegIO   ] = 1; ListGrans[SegIO   ] = 1; SegInits[SegIO   ] = 1;
  SegLimits[SegIO   ] = 0x7;

  MakeCode = MakeCode_1802;
  IsDef = IsDef_1802;

  InitFields();
  SwitchFrom = SwitchFrom_1802;
}

/*-------------------------------------------------------------------------*/
/* Module Initialization */

void code1802_init(void)
{
  CPU1802  = AddCPU("1802", SwitchTo_1802);
  CPU1804  = AddCPU("1804", SwitchTo_1802);
  CPU1805  = AddCPU("1805", SwitchTo_1802);
  CPU1806  = AddCPU("1806", SwitchTo_1802);
  CPU1804A = AddCPU("1804A", SwitchTo_1802);
  CPU1805A = AddCPU("1805A", SwitchTo_1802);
  CPU1806A = AddCPU("1806A", SwitchTo_1802);
}

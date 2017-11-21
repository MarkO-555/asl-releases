/* code9331.c */
/*****************************************************************************/
/* AS-Portierung                                                             */
/*                                                                           */
/* Code Generator Toshiba TC9331                                             */
/*                                                                           */
/*****************************************************************************/
/* $Id: code9331.c,v 1.6 2016/09/11 15:28:51 alfred Exp $                    *
 *****************************************************************************
 * $Log: code9331.c,v $
 * Revision 1.6  2016/09/11 15:28:51  alfred
 * - silence warning about unused parameter
 *
 * Revision 1.5  2016/08/30 14:06:04  alfred
 * - regard ( ) comments
 * - correct machine codes for JC/JNC
 *
 * Revision 1.4  2016/08/30 12:00:49  alfred
 * - correct handling of Zs in MAIN instructions
 *
 * Revision 1.3  2016/08/30 10:36:45  alfred
 * - add some weird extras for B data area
 *
 * Revision 1.2  2016/08/30 10:14:46  alfred
 * - completed instructions
 *
 * Revision 1.1  2016/08/29 21:10:43  alfred
 * - begun with TC9331
 *
 *****************************************************************************/

#include "stdinc.h"
#include <ctype.h>
#include <string.h>

#include "bpemu.h"
#include "strutil.h" 
#include "asmdef.h"
#include "asmsub.h"
#include "asmpars.h"
#include "asmitree.h"
#include "headids.h"
#include "codevars.h"
#include "errmsg.h"

#include "code9331.h"

static CPUVar CPU9331;

static const char *pFNames[] =
{
  "ZF", "SF", "V0F", "V1F",
  "LRF", "GF0", "GF1", "GF2",
  "GF3", "IFF0", "IFF1", "IFF2",
  NULL
};

/* ------------------------------------------------------------------------- */
/* Common Subroutines */

static void StripComment(char *pArg)
{
  char *pStart, *pEnd;

  while (True)
  {
    pStart = QuotPos(pArg, '(');
    if (!pStart)
      return;
    pEnd = QuotPos(pStart + 1, ')');
    if (!pEnd)
      return;
    strmov(pStart, pEnd + 1);
    KillPrefBlanks(pArg);
    KillPostBlanks(pArg);
  }
}

static Boolean DecodeRegList(const char *pNames[], const char *pArg, LongWord *pResult)
{
  if (!*pArg)
  {
    *pResult = 0;
    return True;
  }

  for (*pResult = 0; pNames[*pResult]; (*pResult)++)
    if (!strcasecmp(pArg, pNames[*pResult]))
      return True;

  WrXError(pNames == pFNames ? 1360 : 1445, pArg);
  return False;
}

static Boolean DecodeDSTA(const char *pArg, LongWord *pResult, IntType *pSize)
{
  static const char *pNames[] =
  {
    "LC0", "LC1", "CP", "OFP",
    "DP0", "DP1", "DP2", "DP3",
    "MOD0", "MOD1", "MOD2", "MOD3",
    "DF0", "DF1", NULL
  };

  if (!*pArg)
  {
    WrXError(1445, pArg);
    return False;
  }

  if (!DecodeRegList(pNames, pArg, pResult))
    return False;

  if (3 == *pResult)
    *pSize = UInt6;
  else if (2 == *pResult)
    *pSize = UInt9;
  else
    *pSize = (*pResult <= 1) ? UInt8 : UInt7;
  return True;
}

static Boolean DecodeWRF(const char *pArg, LongWord *pResult)
{
  static const char *pNames[] =
  {
    "WF0", "WF1", "WF2", "W1",
    NULL
  };

  return DecodeRegList(pNames, pArg, pResult);
}

static Boolean DecodeDEST(const char *pArg, LongWord *pResult, Boolean AllowAll)
{
  static const char *pNames[] =
  {
    "NOP", "X", "W0", "W1",
    "Y", "DP", "XAD", "PO",
    "SO0", "SO1", "SO2", "XD",
    "C", "O", "D", "B",
    "XA", "A", 
    NULL
  };
  Boolean Result;

  Result = DecodeRegList(pNames, pArg, pResult);
  if (!Result)
    return Result;

  switch (*pResult)
  {
    case 17:
      if (AllowAll)
        *pResult = 14; /* A -> D */
      else
        Result = False;
      break;
    case 16:
      if (AllowAll)
        *pResult = 11; /* XA -> XD */
      else
        Result = False;
      break;
    case 15:
      if (!AllowAll)
        Result = False;
      break;
    default:
      break;
  }
  if (!Result)
    WrXError(1445, pArg);
  return Result;
}

static Boolean DecodeSOUR(const char *pArg, LongWord *pResult, Boolean AllowAll)
{
  static const char *pNames[] =
  {
    "RND", "X", "W0", "W1",
    "", "DP", "", "PI",
    "PO", "SI0", "SI1", "RMR",
    "C", "O", "D", "B",
    "A",
    NULL
  };
  Boolean Result;

  Result = DecodeRegList(pNames, pArg, pResult);
  if (!Result)
    return Result;

  switch (*pResult)
  {
    case 16:
      if (AllowAll)
        *pResult = 14; /* A -> D */
      else
        Result = False;
      break;
    case 15:
      if (!AllowAll)
        Result = False;
      break;
    default:
      break;
  }

  if (!Result)
    WrXError(1445, pArg);
  return Result;
}

static Boolean DecodeXCNT(const char *pArg, LongWord *pResult)
{
  static const char *pNames[] =
  {
    "STBY", "XRD", "WRX", "XREF",
    NULL
  };

  return DecodeRegList(pNames, pArg, pResult);
}

static Boolean DecodeXS(const char *pArg, LongWord *pResult)
{
  static const char *pNames[] =
  {
    "X", "W",
    NULL
  };

  return DecodeRegList(pNames, pArg, pResult);
}

static Boolean DecodeYS(const char *pArg, LongWord *pResult)
{
  static const char *pNames[] =
  {
    "Y", "X",
    NULL
  };

  return DecodeRegList(pNames, pArg, pResult);
}

static Boolean DecodeZS(const char *pArg, LongWord *pResult)
{
  static const char *pNames[] =
  {
    "0", "W", "AC",
    NULL
  };

  return DecodeRegList(pNames, pArg, pResult);
}

static Boolean DecodeWS(const char *pArg, LongWord *pResult)
{
  static const char *pNames[] =
  {
    "W0", "W1",
    NULL
  };

  return DecodeRegList(pNames, pArg, pResult);
}

static Boolean DecodeCP(const char *pArg, LongWord *pResult)
{
  static const char *pNames[] =
  {
    "NOP", "CP+",
    NULL
  };

  return DecodeRegList(pNames, pArg, pResult);
}

static Boolean DecodeDPS(const char *pArg, LongWord *pResult)
{
  static const char *pNames[] =
  {
    "DP0", "DP1", "DP2", "DP3",
    NULL
  };

  return DecodeRegList(pNames, pArg, pResult);
}

static Boolean DecodeMODE(const char *pArg, LongWord *pResult)
{
  static const char *pNames[] =
  {
    "NOP", "", "-", "+",
    "-DF0", "+DF0", "-DF1", "+DF1",
    NULL
  };

  return DecodeRegList(pNames, pArg, pResult);
}

static Boolean DecodeOFP(const char *pArg, LongWord *pResult)
{
  static const char *pNames[] =
  {
    "NOP", "OFP+",
    NULL
  };

  return DecodeRegList(pNames, pArg, pResult);
}

static Boolean DecodeER(const char *pArg, LongWord *pResult)
{
  static const char *pNames[] =
  {
    "NOP", "ER",
    NULL
  };

  return DecodeRegList(pNames, pArg, pResult);
}

static Boolean DecodeDSTB(const char *pArg, LongWord *pResult)
{
  static const char *pNames[] =
  {
    "W1", "X",
    NULL
  };

  return DecodeRegList(pNames, pArg, pResult);
}

static Boolean DecodeFORM(const char *pArg, LongWord *pResult)
{
  static const char *pNames[] =
  {
    "L", "R",
    NULL
  };

  return DecodeRegList(pNames, pArg, pResult);
}

static Boolean DecodeGFX(const char *pArg, LongWord *pResult)
{
  static const char *pNames[] =
  {
    "GFS", "", "", "GFC", 
    NULL
  };

  if (!*pArg)
  {
    WrXError(1445, pArg);
    return False;
  }

  return DecodeRegList(pNames, pArg, pResult);
}

static Boolean DecodeGF(const char *pArg, LongWord *pResult)
{
  static const char *pNames[] =
  {
    "GF0", "GF1", "GF2", "GF3",
    "OV0", "OV1", "FCHG", "BNK",
    NULL
  };

  return DecodeRegList(pNames, pArg, pResult);
}


/* ------------------------------------------------------------------------- */
/* Instruction Decoders */

static void DecodeMAIN(Word Code)
{
  LongWord Xs, Ys, Zs, Wrf, Ws, Cp, Dps, Mode, Dest, Sour, Xcnt, Ofp, Er,
           LCode = Lo(Code);
  Boolean UseZS = Hi(Code) || False;

  if (!ChkArgCnt(15, 15));
  else if (*ArgStr[1]) WrError(1350);
  else if (!DecodeXS(ArgStr[3], &Xs));
  else if (!DecodeYS(ArgStr[4], &Ys));
  else if (!DecodeZS(ArgStr[5], &Zs));
  else if (!DecodeWRF(ArgStr[6], &Wrf));
  else if (!DecodeWS(ArgStr[7], &Ws));
  else if (!DecodeCP(ArgStr[8], &Cp));
  else if (!DecodeDPS(ArgStr[9], &Dps));
  else if (!DecodeMODE(ArgStr[10], &Mode));
  else if (!DecodeDEST(ArgStr[11], &Dest, True));
  else if (!DecodeSOUR(ArgStr[12], &Sour, True));
  else if (!DecodeXCNT(ArgStr[13], &Xcnt));
  else if (Xcnt == 2) WrXError(1445, ArgStr[13]);
  else if (!DecodeOFP(ArgStr[14], &Ofp));
  else if (!DecodeER(ArgStr[15], &Er));
  else
  {
    Boolean OK = True;
    Word Shift;

    FirstPassUnknown = False;
    Shift = *ArgStr[2] ? EvalIntExpression(ArgStr[2], UInt3, &OK) : 0;
    if (FirstPassUnknown)
      Shift = 0;

    DAsmCode[0] = (LCode << 26)
                | (Xs << 24) | (Ys << 23)
                | (Wrf << 20) | (Ws << 22) | (Cp << 16) | (Dps << 13)
                | (Mode << 10) | (Dest << 0) | (Sour << 4) | (Xcnt << 8)
                | (Ofp << 15) | (Er << 25);
    if (UseZS)
      DAsmCode[0] |= (Zs << 26);
    switch (Shift)
    {
      case 0:
        break;
      case 1:
        DAsmCode[0] |= 0x00040000ul;
        break;
      case 4:
        DAsmCode[0] |= 0x00080000ul;
        break;
      default:
        WrXError(1380, ArgStr[2]);
        return;
    }

    CodeLen = 1;
  }
}

static void DecodeLDA(Word Code)
{
  LongWord DstA, Wrf, Dest, Sour, Xcnt;
  IntType IntSize;

  UNUSED(Code);

  if (!ChkArgCnt(7, 7));
  else if (*ArgStr[1]) WrError(1350);
  else if (!DecodeDSTA(ArgStr[2], &DstA, &IntSize));
  else if (!DecodeWRF(ArgStr[4], &Wrf));
  else if (!DecodeDEST(ArgStr[5], &Dest, False));
  else if (!DecodeSOUR(ArgStr[6], &Sour, False));
  else if (Sour > 14) WrXError(1445, ArgStr[6]);
  else if (!DecodeXCNT(ArgStr[7], &Xcnt));
  else if (Xcnt == 2) WrXError(1445, ArgStr[7]);
  else
  {
    Boolean OK;
    LongWord Value = EvalIntExpression(ArgStr[3], IntSize, &OK);

    if (OK)
    {
      DAsmCode[0] = 0x04000000ul
                  | (DstA << 22) | (Wrf << 20) | (Dest << 0) | (Sour << 4) | (Xcnt << 8)
                  | ((Value << 10) & 0x7fc00);
      CodeLen = 1;
    }
  }
}

static void DecodeLDB(Word Code)
{
  LongWord DstB, Form;

  UNUSED(Code);

  if (!ChkArgCnt(4, 4));
  else if (*ArgStr[1]) WrError(1350);
  else if (!DecodeDSTB(ArgStr[2], &DstB));
  else if (!DecodeFORM(ArgStr[4], &Form));
  else
  {
    Boolean OK;
    LongWord Value = EvalIntExpression(ArgStr[3], UInt24, &OK);

    if (OK)
    {
      DAsmCode[0] = 0x08000000ul
                  | (DstB << 1) | (Form << 0)
                  | ((Value & 0xffffff) << 2);
      CodeLen = 1;
    }
  }
}

static void DecodeBR(Word Code)
{
  LongWord Cp, Dps, Mode, Dest, Sour, Xcnt, Ofp;

  if (!ChkArgCnt(9, 9));
  else if (*ArgStr[1]) WrError(1350);
  else if (!DecodeCP(ArgStr[3], &Cp));
  else if (!DecodeDPS(ArgStr[4], &Dps));
  else if (!DecodeMODE(ArgStr[5], &Mode));
  else if (!DecodeDEST(ArgStr[6], &Dest, False));
  else if (!DecodeSOUR(ArgStr[7], &Sour, False));
  else if (!DecodeXCNT(ArgStr[8], &Xcnt));
  else if (!DecodeOFP(ArgStr[9], &Ofp));
  else
  {
    Boolean OK;
    LongWord Address;

    FirstPassUnknown = False;
    Address = EvalIntExpression(ArgStr[2], UInt9, &OK);
    if (FirstPassUnknown)
      Address &= 0xff;
    if (OK && ChkRange(Address, 0, SegLimits[SegCode]))
    {
      DAsmCode[0] = (((LongWord)Code) << 26)
                  | (Address << 17) | (Cp << 16) | (Dps << 13) | (Mode << 10)
                  | (Dest << 0) | (Sour << 4) | (Xcnt << 8) | (Ofp << 15);
      CodeLen = 1;
    }
  }
}

static void DecodeJC(Word Code)
{
  LongWord F, Cp, Dps, Mode, Dest, Sour, Xcnt, Ofp;

  if (!ChkArgCnt(10, 10));
  else if (*ArgStr[1]) WrError(1350);
  else if (!DecodeRegList(pFNames, ArgStr[3], &F));
  else if (!DecodeCP(ArgStr[4], &Cp));
  else if (!DecodeDPS(ArgStr[5], &Dps));
  else if (!DecodeMODE(ArgStr[6], &Mode));
  else if (!DecodeDEST(ArgStr[7], &Dest, False));
  else if (!DecodeSOUR(ArgStr[8], &Sour, False));
  else if (!DecodeXCNT(ArgStr[9], &Xcnt));
  else if (!DecodeOFP(ArgStr[10], &Ofp));
  else
  {
    Boolean OK;
    LongWord Address;

    FirstPassUnknown = False;
    Address = EvalIntExpression(ArgStr[2], UInt9, &OK);
    if (FirstPassUnknown)
      Address &= 0xff;
    if (OK && ChkRange(Address, 0, SegLimits[SegCode]))
    {
      F = (F << 2) + ((LongWord)Code);
      DAsmCode[0] = (F << 24)
                  | (Address << 17) | (Cp << 16) | (Dps << 13) | (Mode << 10)
                  | (Dest << 0) | (Sour << 4) | (Xcnt << 8) | (Ofp << 15);
      CodeLen = 1;
    }
  }
}

static void DecodeRET(Word Code)
{
  LongWord Xs, Ys, Wrf, Ws, Cp, Dps, Mode, Dest, Sour, Xcnt, Ofp;

  UNUSED(Code);

  if (!ChkArgCnt(13, 13));
  else if (*ArgStr[1]) WrError(1350);
  else if (!DecodeXS(ArgStr[3], &Xs));
  else if (!DecodeYS(ArgStr[4], &Ys));
  else if (!DecodeWRF(ArgStr[5], &Wrf));
  else if (!DecodeWS(ArgStr[6], &Ws));
  else if (!DecodeCP(ArgStr[7], &Cp));
  else if (!DecodeDPS(ArgStr[8], &Dps));
  else if (!DecodeMODE(ArgStr[9], &Mode));
  else if (!DecodeDEST(ArgStr[10], &Dest, True));
  else if (!DecodeSOUR(ArgStr[11], &Sour, True));
  else if (!DecodeXCNT(ArgStr[12], &Xcnt));
  else if (Xcnt == 2) WrXError(1445, ArgStr[12]);
  else if (!DecodeOFP(ArgStr[13], &Ofp));
  else
  {
    Boolean OK = True;
    Word Shift;

    FirstPassUnknown = False;
    Shift = *ArgStr[2] ? EvalIntExpression(ArgStr[2], UInt3, &OK) : 0;
    if (FirstPassUnknown)
      Shift = 0;

    DAsmCode[0] = 0xc4000000ul
                | (Xs << 24) | (Ys << 23)
                | (Wrf << 20) | (Ws << 22) | (Cp << 16) | (Dps << 13)
                | (Mode << 10) | (Dest << 0) | (Sour << 4) | (Xcnt << 8)
                | (Ofp << 15);
    switch (Shift)
    {
      case 0:
        break;
      case 1:
        DAsmCode[0] |= 0x00040000ul;
        break;
      case 4:
        DAsmCode[0] |= 0x00080000ul;
        break;
      default:
        WrXError(1380, ArgStr[2]);
        return;
    }

    /* ??? */

    if ((Sour == 15) || (Dest == 15))
      DAsmCode[0] |= 0x00020000;

    CodeLen = 1;
  }
}

static void DecodeGMAx(Word Code)
{
  LongWord Gfx, Gf, Wrf, Ws, Cp, Dps, Mode, Dest, Sour, Xcnt, Ofp;

  if (!ChkArgCnt(12, 12));
  else if (*ArgStr[1]) WrError(1350);
  else if (!DecodeGFX(ArgStr[2], &Gfx));
  else if (!DecodeGF(ArgStr[3], &Gf));
  else if (!DecodeWRF(ArgStr[4], &Wrf));
  else if (!DecodeWS(ArgStr[5], &Ws));
  else if (!DecodeCP(ArgStr[6], &Cp));
  else if (!DecodeDPS(ArgStr[7], &Dps));
  else if (!DecodeMODE(ArgStr[8], &Mode));
  else if (!DecodeDEST(ArgStr[9], &Dest, True));
  else if (!DecodeSOUR(ArgStr[10], &Sour, True));
  else if (!DecodeXCNT(ArgStr[11], &Xcnt));
  else if (Xcnt == 2) WrXError(1445, ArgStr[11]);
  else if (!DecodeOFP(ArgStr[12], &Ofp));
  else
  {
    DAsmCode[0] = 0x0c000000ul | (((LongWord)Code) << 16)
                | (Gfx << 28) | (Gf << 23)
                | (Wrf << 20) | (Ws << 22) | (Cp << 16) | (Dps << 13)
                | (Mode << 10) | (Dest << 0) | (Sour << 4) | (Xcnt << 8)
                | (Ofp << 15);

    /* ??? */

    if ((Sour == 15) || (Dest == 15))
      DAsmCode[0] |= 0x00020000;

    CodeLen = 1;
  }
}

/* ------------------------------------------------------------------------- */
/* Dynamic Code Table Handling */

static void InitFields(void)
{
  InstTable = CreateInstTable(107);

  AddInstTable(InstTable, "NOP",  0x0000, DecodeMAIN);
  AddInstTable(InstTable, "AND",  0x0004, DecodeMAIN);
  AddInstTable(InstTable, "OR",   0x0005, DecodeMAIN);
  AddInstTable(InstTable, "XOR",  0x0006, DecodeMAIN);
  AddInstTable(InstTable, "NOT",  0x0007, DecodeMAIN);
  AddInstTable(InstTable, "SUB",  0x0108, DecodeMAIN);
  AddInstTable(InstTable, "ABS",  0x000b, DecodeMAIN);
  AddInstTable(InstTable, "CMP",  0x010c, DecodeMAIN);
  AddInstTable(InstTable, "WCL",  0x0010, DecodeMAIN);
  AddInstTable(InstTable, "WRS",  0x0011, DecodeMAIN);
  AddInstTable(InstTable, "ACS",  0x0012, DecodeMAIN);
  AddInstTable(InstTable, "MAD",  0x0114, DecodeMAIN);
  AddInstTable(InstTable, "MADS", 0x0118, DecodeMAIN);
  AddInstTable(InstTable, "DMAD", 0x001e, DecodeMAIN);
  AddInstTable(InstTable, "DMAS", 0x001f, DecodeMAIN);
  AddInstTable(InstTable, "LDA", 0, DecodeLDA);
  AddInstTable(InstTable, "LDB", 0, DecodeLDB);
  AddInstTable(InstTable, "JMP0", 0x20, DecodeBR);
  AddInstTable(InstTable, "JMP1", 0x21, DecodeBR);
  AddInstTable(InstTable, "JNZ0", 0x22, DecodeBR);
  AddInstTable(InstTable, "JNZ1", 0x23, DecodeBR);
  AddInstTable(InstTable, "JZ0",  0x32, DecodeBR);
  AddInstTable(InstTable, "JZ1",  0x33, DecodeBR);
  AddInstTable(InstTable, "CALL", 0x30, DecodeBR);
  AddInstTable(InstTable, "JC", 0xd0, DecodeJC);
  AddInstTable(InstTable, "JNC", 0x90, DecodeJC);
  AddInstTable(InstTable, "RET", 0, DecodeRET);
  AddInstTable(InstTable, "GMA" , 0, DecodeGMAx);
  AddInstTable(InstTable, "GMAD", 4, DecodeGMAx);
  AddInstTable(InstTable, "GMAS", 8, DecodeGMAx);
}

static void DeinitFields(void)
{
  DestroyInstTable(InstTable);
}

/* ------------------------------------------------------------------------- */
/* Adaptors */

static void MakeCode_9331(void)
{
  int z;

  CodeLen = 0;
  DontPrint = False;

  /* TC9331 knows default instructions: may be NOP or GF command */

  if (Memo(""))
  {
    switch (ArgCnt)
    {
      case 12:
        strmaxcpy(OpPart, "GMA", sizeof(OpPart));
        break;
      case 15:
        strmaxcpy(OpPart, "NOP", sizeof(OpPart));
        break;
      case 0:
        return;
      default:
        WrXError(1200, OpPart);
        return;
    }
  }

  /* Toshiba assembler treats parts in ( ) as comments - trim them
     out of the arguments.  This will also break more complex formulas,
     but you are not going to use them on a processor of 320 words
     of program space anyway... */

  for (z = 1; z < ArgCnt; z++)
    StripComment(ArgStr[z]);

  if (!LookupInstTable(InstTable, OpPart))
    WrXError(1200, OpPart);
}

static Boolean IsDef_9331(void)
{
  return False;
}

static void SwitchFrom_9331(void)
{
  DeinitFields();
}

static void SwitchTo_9331(void)
{
  const TFamilyDescr *pDescr;

  pDescr = FindFamilyByName("TC9331");

  TurnWords = False;
  ConstMode = ConstModeIntel;
  SetIsOccupied = False;

  PCSymbol = "$";
  HeaderID = pDescr->Id;
  NOPCode = 0x00000000;
  DivideChars = ",";
  HasAttrs = False;

  ValidSegs = 1 << SegCode;
  Grans[SegCode] = 4; ListGrans[SegCode] = 4; SegInits[SegCode] = 0;
  SegLimits[SegCode] = 319;

  MakeCode = MakeCode_9331;
  IsDef = IsDef_9331;
  SwitchFrom = SwitchFrom_9331;
  InitFields();
}

extern void code9331_init(void)
{
  CPU9331 = AddCPU("TC9331", SwitchTo_9331);
}

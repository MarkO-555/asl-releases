#ifndef _SYMBOLSIZE_H
#define _SYMBOLSIZE_H
/* symbolsize.h */
/*****************************************************************************/
/* Macro Assembler AS                                                        */
/*                                                                           */
/* Definition of a symbol's/instruction's operand type & size                */
/*                                                                           */
/*****************************************************************************/

typedef enum
{
  eSymbolSizeUnknown = -1,
  eSymbolSize8Bit = 0,
  eSymbolSize16Bit = 1,
  eSymbolSize32Bit = 2,
  eSymbolSize64Bit = 3,
  eSymbolSize80Bit = 4, /* Intel 80 Bit extended float */
  eSymbolSizeFloat32Bit = 5,
  eSymbolSizeFloat64Bit = 6,
  eSymbolSizeFloat96Bit = 7,
  eSymbolSize24Bit = 8,
  eSymbolSizeFloatDec96Bit = 9
} tSymbolSize;

extern const char *GetSymbolSizeName(tSymbolSize Size);

extern unsigned GetSymbolSizeBytes(tSymbolSize Size);

#endif /* _SYMBOLSIZE_H */
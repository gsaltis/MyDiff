/*****************************************************************************
 * FILE NAME    : SourceLineNumber.cpp
 * DATE         : June 06 2023
 * PROJECT      : 
 * COPYRIGHT    : Copyright (C) 2023 by Gregory R Saltis
 *****************************************************************************/

/*****************************************************************************!
 * Global Headers
 *****************************************************************************/
#include <QtCore>
#include <QtGui>
#include <QWidget>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/
#include "SourceLineNumber.h"

/*****************************************************************************!
 * Function : SourceLineNumber
 *****************************************************************************/
SourceLineNumber::SourceLineNumber
(int InLineNumber) : QWidget()
{
  LineNumber = InLineNumber;
  Processed = false;
}

/*****************************************************************************!
 * Function : ~SourceLineNumber
 *****************************************************************************/
SourceLineNumber::~SourceLineNumber
()
{
}

/*****************************************************************************!
 * Function : GetLineNumber
 *****************************************************************************/
int
SourceLineNumber::GetLineNumber(void)
{
  return LineNumber;  
}

/*****************************************************************************!
 * Function : SetLineNumber
 *****************************************************************************/
void
SourceLineNumber::SetLineNumber
(int InLineNumber)
{
  LineNumber = InLineNumber;  
}

/*****************************************************************************!
 * Function : GetProcessed
 *****************************************************************************/
bool
SourceLineNumber::GetProcessed(void)
{
  return Processed;  
}

/*****************************************************************************!
 * Function : SetProcessed
 *****************************************************************************/
void
SourceLineNumber::SetProcessed
(bool InProcessed)
{
  Processed = InProcessed;  
}

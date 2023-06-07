/*****************************************************************************
 * FILE NAME    : SourceLineReference.cpp
 * DATE         : June 07 2023
 * PROJECT      : 
 * COPYRIGHT    : Copyright (C) 2023 by Vertiv Company
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
#include "SourceLineReference.h"

/*****************************************************************************!
 * Function : SourceLineReference
 *****************************************************************************/
SourceLineReference::SourceLineReference
() : QWidget()
{
  count = 0;
}

/*****************************************************************************!
 * Function : ~SourceLineReference
 *****************************************************************************/
SourceLineReference::~SourceLineReference
()
{
}

/*****************************************************************************!
 * Function : GetCount
 *****************************************************************************/
int
SourceLineReference::GetCount
()
{
  return count;
}

/*****************************************************************************!
 * Function : AddLineNumber
 *****************************************************************************/
void
SourceLineReference::AddLineNumber
(int InLineNumber)
{
  lineNumbers << new SourceLineNumber(InLineNumber);
  IncrementCount();
}

/*****************************************************************************!
 * Function : GetLineNumberCount
 *****************************************************************************/
int
SourceLineReference::GetLineNumberCount
()
{
  return lineNumbers.size();
}

/*****************************************************************************!
 * Function : GetLineNumberByIndex
 *****************************************************************************/
SourceLineNumber*
SourceLineReference::GetLineNumberByIndex
(int InIndex)
{
  if ( InIndex < 0 || InIndex >= lineNumbers.size() ) {
    return 0;
  }
  return lineNumbers[InIndex];
}


/*****************************************************************************!
 * Function : IncrementCount
 *****************************************************************************/
void
SourceLineReference::IncrementCount
()
{
  count++;
}




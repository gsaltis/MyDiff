/*****************************************************************************
 * FILE NAME    : StringCount.cpp
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
#include "StringCount.h"

/*****************************************************************************!
 * Function : StringCount
 *****************************************************************************/
StringCount::StringCount
(QString InString, int InLineNumber) : QWidget()
{
  fileRef1 = new SourceLineReference();
  fileRef2 = new SourceLineReference();
  fileRef1->AddLineNumber(InLineNumber);
  string = InString;
}

/*****************************************************************************!
 * Function : StringCount
 *****************************************************************************/
StringCount::StringCount
() : QWidget()
{
  string = "";
}

/*****************************************************************************!
 * Function : ~StringCount
 *****************************************************************************/
StringCount::~StringCount
()
{
}

/*****************************************************************************!
 * Function : IncreaseCount
 *****************************************************************************/
void
StringCount::IncreaseCount
(int InLineNumber)
{
  fileRef1->AddLineNumber(InLineNumber);
}

/*****************************************************************************!
 * Function : GetString
 *****************************************************************************/
QString
StringCount::GetString
()
{
  return string;
}

/*****************************************************************************!
 * Function : GetCount
 *****************************************************************************/
int
StringCount::GetCount
()
{
  return fileRef1->GetCount();
}

/*****************************************************************************!
 * Function : GetLineNumberCount
 *****************************************************************************/
int
StringCount::GetLineNumberCount
()
{
  return fileRef1->GetLineNumberCount();
}

/*****************************************************************************!
 * Function : GetLineNumber
 *****************************************************************************/
int
StringCount::GetLineNumber
(int InIndex)
{
  SourceLineNumber*                     lineNumber;

  lineNumber = fileRef1->GetLineNumberByIndex(InIndex);
  if ( NULL == lineNumber ) {
    return 0;
  }
  return lineNumber->GetLineNumber();
}

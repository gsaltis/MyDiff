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
  string = InString;
  count = 1;
  lineNumbers << new SourceLineNumber(InLineNumber);
}

/*****************************************************************************!
 * Function : StringCount
 *****************************************************************************/
StringCount::StringCount
() : QWidget()
{
  string = "";
  count = 0;
}

/*****************************************************************************!
 * Function  StringCount
 *****************************************************************************/
StringCount::StringCount
(StringCount& InCount) : QWidget()
{
  count = InCount.GetCount();
  string = InCount.GetString();
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
  lineNumbers << new SourceLineNumber(InLineNumber);
  count++;
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
  return count;
}

/*****************************************************************************!
 * Function : GetLineNumberCount
 *****************************************************************************/
int
StringCount::GetLineNumberCount
()
{
  return lineNumbers.size();
}

/*****************************************************************************!
 * Function : GetLineNumber
 *****************************************************************************/
int
StringCount::GetLineNumber
(int InIndex)
{
  if ( InIndex < 0 || InIndex >= lineNumbers.size() ) {
    return -1;
  }
  return lineNumbers[InIndex]->GetLineNumber();
}

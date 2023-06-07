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
  TrackCount = 2;
  fileRef[0] = new SourceLineReference();
  fileRef[1] = new SourceLineReference();
  fileRef[0]->AddLineNumber(InLineNumber);
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
(int InLineNumber, int InTrack)
{
  if ( InTrack >= TrackCount ) {
    return;
  }
  fileRef[InTrack]->AddLineNumber(InLineNumber);
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
(int InTrack)
{
  if ( InTrack >= TrackCount ) {
    return 0;
  }
  return fileRef[InTrack]->GetCount();
}

/*****************************************************************************!
 * Function : GetLineNumberCount
 *****************************************************************************/
int
StringCount::GetLineNumberCount
(int InTrack)
{
  if ( InTrack >= TrackCount ) {
    return 0;
  }
  return fileRef[InTrack]->GetLineNumberCount();
}

/*****************************************************************************!
 * Function : GetLineNumber
 *****************************************************************************/
int
StringCount::GetLineNumber
(int InIndex, int InTrack)
{
  SourceLineNumber*                     lineNumber;

  if ( InTrack >= TrackCount ) {
    return 0;
  }
  lineNumber = fileRef[InTrack]->GetLineNumberByIndex(InIndex);
  if ( NULL == lineNumber ) {
    return 0;
  }
  return lineNumber->GetLineNumber();
}

/*****************************************************************************!
 * Function : HasLineNumberReference
 *****************************************************************************/
bool
StringCount::HasLineNumberReference
(int InLineNumber, int InTrack)
{
  if ( InTrack >= TrackCount ) {
    return 0;
  }
  return fileRef[InTrack]->HasLineNumberReference(InLineNumber);
}

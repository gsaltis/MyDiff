/*****************************************************************************
 * FILE NAME    : StringCountList.cpp
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
#include "StringCountList.h"

/*****************************************************************************!
 * Function : StringCountList
 *****************************************************************************/
StringCountList::StringCountList
() 
{
}

/*****************************************************************************!
 * Function : ~StringCountList
 *****************************************************************************/
StringCountList::~StringCountList
()
{
}

/*****************************************************************************!
 * Function : FindString
 *****************************************************************************/
StringCount*
StringCountList::FindString
(QString InString)
{
  int                                   i, n;
  StringCount*                          sc;
  
  n = strings.size();

  for (i = 0; i < n; i++) {
    sc = strings[i];
    if ( sc->GetString() == InString ) {
      return strings[i];
    }
  }
  return NULL;
}

/*****************************************************************************!
 * Function : FindStringCount
 *****************************************************************************/
int
StringCountList::FindStringCount
(QString InString)
{
  StringCount*                    s;

  s = FindString(InString);
  if ( NULL == s ) {
    return 0;
  }
  return s->GetCount();
}

/*****************************************************************************!
 * Function : Append
 *****************************************************************************/
void
StringCountList::Append
(StringCount* InStringCount)
{
  strings << InStringCount;
}

/*****************************************************************************!
 * Funtion : GetByIndex
 *****************************************************************************/
StringCount*
StringCountList::GetByIndex
(int InIndex)
{
  if ( InIndex < 0 || InIndex >= strings.size() ) {
    return NULL;
  }

  return strings[InIndex];
}

/*****************************************************************************!
 * Function : GetSize
 *****************************************************************************/
int
StringCountList::GetSize(void)
{
  return strings.size();
}

/*****************************************************************************!
 * Function : Exists
 *****************************************************************************/
bool
StringCountList::Exists
(QString InString)
{
  return FindString(InString) != NULL;
}

/*****************************************************************************!
 * Function : IncreaseStringCount
 *****************************************************************************/
void
StringCountList::IncreaseStringCount
(QString InString, int InLineNummber)
{
  StringCount*                          st;

  st = FindString(InString);
  if ( NULL == st ) {
    return;
  }
  st->IncreaseCount(InLineNummber);
}

/*****************************************************************************
 * FILE NAME    : StringCountList.h
 * DATE         : June 06 2023
 * PROJECT      : 
 * COPYRIGHT    : Copyright (C) 2023 by Gregory R Saltis
 *****************************************************************************/
#ifndef _stringcountlist_h_
#define _stringcountlist_h_

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
 * Exported Macros
 *****************************************************************************/

/*****************************************************************************!
 * Exported Class : StringCountList
 *****************************************************************************/
class StringCountList : QWidget
{
  Q_OBJECT;

 //! Constructors
 public :
  StringCountList               ();

 //! Destructor
 public :
  ~StringCountList              ();

 //! Public Methods
 public :
  StringCount*                  FindString              (QString InString);
  int                           FindStringCount         (QString InString);
  void                          Append                  (StringCount* InStringCount);
  StringCount*                  GetByIndex              (int InIndex);
  int                           GetSize                 (void);
  bool                          Exists                  (QString InString);
  void                          IncreaseStringCount     (QString InString, int InLineNumber);
  
 //! Public Data
 public :

 //! Protected Methods
 protected :

 //! Protected Data
 protected :

 //! Private Methods
 private :

 //! Private Data
 private :
  QList<StringCount*>         strings;
  
 //! Public Slots
 public slots :

 //! Public Signals
 signals :

 //! Public Actions
 public :

};

#endif /* _stringcountlist_h_*/

/*****************************************************************************
 * FILE NAME    : StringCount.h
 * DATE         : June 06 2023
 * PROJECT      : 
 * COPYRIGHT    : Copyright (C) 2023 by Gregory R Saltis
 *****************************************************************************/
#ifndef _stringcount_h_
#define _stringcount_h_

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
#include "SourceLineReference.h"

/*****************************************************************************!
 * Exported Macros
 *****************************************************************************/

/*****************************************************************************!
 * Exported Class : StringCount
 *****************************************************************************/
class StringCount : public QWidget
{
  Q_OBJECT;

 //! Constructors
 public :
  StringCount                   ();
  StringCount                   (QString InString, int InLineNumber);

 //! Destructor
 public :
  ~StringCount                  ();

 //! Public Methods
 public :
  void                          IncreaseCount           (int InLineNumber, int InTrack);
  QString                       GetString               ();
  int                           GetCount                (int InTrack);
  int                           GetLineNumber           (int InIndex, int InTrack);
  int                           GetLineNumberCount      (int InTrack);
  bool                          HasLineNumberReference  (int InLineNumber, int InTrack);
  
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
  QString                               string;
  SourceLineReference*                  fileRef[2];
  int                                   TrackCount;
  
 //! Public Slots
 public slots :

 //! Public Signals
 signals :

 //! Public Actions
 public :

};

#endif /* _stringcount_h_*/

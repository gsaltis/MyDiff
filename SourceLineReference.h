/*****************************************************************************
 * FILE NAME    : SourceLineReference.h
 * DATE         : June 07 2023
 * PROJECT      : 
 * COPYRIGHT    : Copyright (C) 2023 by Vertiv Company
 *****************************************************************************/
#ifndef _sourcelinereference_h_
#define _sourcelinereference_h_

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
 * Exported Macros
 *****************************************************************************/

/*****************************************************************************!
 * Exported Class : SourceLineReference
 *****************************************************************************/
class SourceLineReference : public QWidget
{
  Q_OBJECT;

 //! Constructors
 public :
  SourceLineReference           ();

 //! Destructor
 public :
  ~SourceLineReference          ();

 //! Public Methods
 public :
  int                           GetCount                ();
  void                          AddLineNumber           (int InLineNumber);
  int                           GetLineNumberCount      ();
  SourceLineNumber*             GetLineNumberByIndex    (int InIndex);
  void                          IncrementCount          ();
  bool                          HasLineNumberReference  (int InLineNumber);
  
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
  int                                   count;
  SourceLineNumberList                  lineNumbers;

 //! Public Slots
 public slots :

 //! Public Signals
 signals :

 //! Public Actions
 public :

};

#endif /* _sourcelinereference_h_*/

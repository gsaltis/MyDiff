/*****************************************************************************
 * FILE NAME    : SourceLineNumber.h
 * DATE         : June 06 2023
 * PROJECT      : 
 * COPYRIGHT    : Copyright (C) 2023 by Gregory R Saltis
 *****************************************************************************/
#ifndef _sourcelinenumber_h_
#define _sourcelinenumber_h_

/*****************************************************************************!
 * Global Headers
 *****************************************************************************/
#include <QtCore>
#include <QtGui>
#include <QWidget>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/

/*****************************************************************************!
 * Exported Macros
 *****************************************************************************/

/*****************************************************************************!
 * Exported Class : SourceLineNumber
 *****************************************************************************/
class SourceLineNumber : public QWidget
{
  Q_OBJECT;

 //! Constructors
 public :
  SourceLineNumber              (int InLineNumber);

 //! Destructor
 public :
  ~SourceLineNumber             ();

 //! Public Methods
 public :
  int                           GetLineNumber           (void);
  void                          SetLineNumber           (int InLineNumber);
  bool                          GetProcessed            (void);
  void                          SetProcessed            (bool InProcessed);

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
  int                           LineNumber;
  bool                          Processed;

 //! Public Slots
 public slots :

 //! Public Signals
 signals :

 //! Public Actions
 public :

};

#endif /* _sourcelinenumber_h_*/

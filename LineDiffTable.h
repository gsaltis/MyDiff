/*****************************************************************************
 * FILE NAME    : LineDiffTable.h
 * DATE         : June 07 2023
 * PROJECT      : 
 * COPYRIGHT    : Copyright (C) 2023 by Vertiv Company
 *****************************************************************************/
#ifndef _linedifftable_h_


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
#define LINE_DIFF_TABLE_X               200
#define LINE_DIFF_TABLE_Y               200
#define LINE_DIFF_TABLE_WIDTH           200
#define LINE_DIFF_TABLE_HEIGHT          200

/*****************************************************************************!
 * Exported Class : LineDiffTable
 *****************************************************************************/
class LineDiffTable : public QWidget
{
  Q_OBJECT;

 //! Constructors
 public :
  LineDiffTable                 ();

 //! Destructor
 public :
  ~LineDiffTable                ();

 //! Public Methods
 public :

 //! Public Data
 public :

 //! Protected Methods
 protected :

 //! Protected Data
 protected :

 //! Private Methods
 private :
  void                          initialize              ();
  void                          CreateSubWindows        ();
  void                          InitializeSubWindows    ();
  void                          resizeEvent             (QResizeEvent* InEvent);

 //! Private Data
 private :

 //! Public Slots
 public slots :

 //! Public Signals
 signals :

 //! Public Actions
 public :

};

#define _linedifftable_h_

#endif /* _linedifftable_h_*/

/*****************************************************************************
 * FILE NAME    : SystemConfig.h
 * DATE         : June 06 2023
 * PROJECT      : MyDiff
 * COPYRIGHT    : Copyright (C) 2023 by Vertiv Company
 *****************************************************************************/
#ifndef _systemconfig_h_
#define _systemconfig_h_

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
 * Exported Class : SystemConfig
 *****************************************************************************/
class SystemConfig : public QWidget
{
  Q_OBJECT;

 //! Constructors
 public :
  SystemConfig                  ();

 //! Destructor
 public :
  ~SystemConfig                 ();

 //! Public Methods
 public :

 //! Public Data
 public :
  static QString                SystemName;

 //! Protected Methods
 protected :

 //! Protected Data
 protected :

 //! Private Methods
 private :

 //! Private Data
 private :

 //! Public Slots
 public slots :

 //! Public Signals
 signals :

 //! Public Actions
 public :

};

#endif /* _systemconfig_h_*/

/*****************************************************************************
 FILE NAME      : main.cpp
 DATE           : June 06 2023
 PROJECT        : MyDiff
 COPYRIGHT      : Copyright (C) 2023 by Vertiv Company
******************************************************************************/

/*****************************************************************************!
 * Global Headers
 *****************************************************************************/
#include <QtCore>
#include <QApplication>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QtGui>
#include <QCommandLineParser>

/*****************************************************************************!
 * Local Headers
 *****************************************************************************/
#include "MainWindow.h"
#include "main.h"
#include "trace.h"
#include "StringCountList.h"
#include "StringCount.h"

/*****************************************************************************!
 * Local Macros
 *****************************************************************************/

/*****************************************************************************!
 * Local Functions
 *****************************************************************************/
void
VerifyCommandLine
();

bool
ReadFiles
();

void
CompareFiles
();

bool
ReadFile
(QString InFileName, QStringList& InFileLines, int InTrack);

QString
ProcessFileBuffer
(QString InFileBuffer);

void
ProcessFileLines
(QStringList InLineList, int InTrack);

QString
ProcessFileLine
(QString InLine);

void
DisplayFileLines
(bool InDisplayLineNumbers);

/*****************************************************************************!
 * Local Data
 *****************************************************************************/
QString
mainFileName1;

QString
mainFileName2;

QStringList
mainFileLines1;

QStringList
mainFileLines2;

StringCountList*
mainStringsCount;

bool
mainDisplayLineNumbers = true;

/*****************************************************************************!
 * Function : main
 *****************************************************************************/
int
main
(int argc, char** argv)
{
  QApplication                          application(argc, argv);
  MainWindow*                           w;
  QCommandLineParser                    commandLineParser;
  
  application.setApplicationName("MyDiff");
  application.setApplicationVersion("0.0.0");
  application.setOrganizationName("Vertiv Company");
  application.setOrganizationDomain("www.vertivco.com");
  
  commandLineParser.setApplicationDescription("CodeCompare");
  commandLineParser.addHelpOption();
  commandLineParser.addVersionOption();
  QCommandLineOption fileName1Option(QStringList() << "1" << "filename1",
                                     QCoreApplication::translate("main", "Specifiy the first file name."),
                                     QCoreApplication::translate("main", "filename"));
  QCommandLineOption fileName2Option(QStringList() << "2" << "filename2",
                                     QCoreApplication::translate("main", "Specifiy the second file name."),
                                     QCoreApplication::translate("main", "filename"));
  commandLineParser.addOption(fileName1Option);
  commandLineParser.addOption(fileName2Option);

  commandLineParser.process(application);
  mainFileName1 = commandLineParser.value(fileName1Option);
  mainFileName2 = commandLineParser.value(fileName2Option);

  mainStringsCount = new StringCountList();
  
  VerifyCommandLine();
  if ( ! ReadFiles() ) {
    exit(EXIT_FAILURE);
  }
  DisplayFileLines(mainDisplayLineNumbers);
  w = new MainWindow(NULL);
  w->resize(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT);
  w->move(MAIN_WINDOW_X, MAIN_WINDOW_Y);
  w->show();

  return application.exec();
}


/*****************************************************************************!
 * Function : VerifyCommandLine
 *****************************************************************************/
void
VerifyCommandLine
()
{
  if ( mainFileName1.isEmpty() ) {
    fprintf(stderr, "Missing file name 1\n");
    exit(EXIT_FAILURE);
  }

  if ( mainFileName2.isEmpty() ) {
    fprintf(stderr, "Missing file name 2\n");
    exit(EXIT_FAILURE);
  }

  QFileInfo                             file1(mainFileName1);
  if ( !file1.exists() ) {
    fprintf(stderr, "%s does not exist\n", mainFileName1.toStdString().c_str());
    exit(EXIT_FAILURE);
  }

  QFileInfo                             file2(mainFileName2);
  if ( !file2.exists() ) {
    fprintf(stderr, "%s does not exist\n", mainFileName2.toStdString().c_str());
    exit(EXIT_FAILURE);
  }
}

/*****************************************************************************!
 * Function : ReadFiles
 *****************************************************************************/
bool
ReadFiles
()
{
  if ( ! ReadFile(mainFileName1, mainFileLines1, 0) ) {
    fprintf(stderr, "Could not read %s\n", mainFileName1.toStdString().c_str());
    return false;
  }


  if ( ! ReadFile(mainFileName2, mainFileLines2, 1) ) {
    fprintf(stderr, "Could not read %s\n", mainFileName2.toStdString().c_str());
    return false;
  }

  return true;
}

/*****************************************************************************!
 * Function : ReadFile
 *****************************************************************************/
bool
ReadFile
(QString InFileName, QStringList& InFileList, int InTrack)
{
  QString                               buffer;
  QString                               buffer2;
  
  QFile                                 file(InFileName);

  if ( ! file.open(QIODeviceBase::ReadOnly) ) {
    return false;
  }

  buffer = QString(file.readAll());
  buffer2 = ProcessFileBuffer(buffer);
  InFileList = buffer2.split("\n", Qt::KeepEmptyParts);
  ProcessFileLines(InFileList, InTrack);
  fflush(stdout);
  
  file.close();
  return true;
}

/*****************************************************************************!
 * Function : ProcessFileBuffer
 *****************************************************************************/
QString
ProcessFileBuffer
(QString InFileBuffer)
{
  QString                               returnBuffer;
  QChar                                 ch, nextCh;
  int                                   n;
  int                                   i, nextI;
  int                                   bufferSize;
  
  enum {
    InBlockComment,
    InLineComment,
    InNoComment
  }                                     state;
  state = InNoComment;

  n = InFileBuffer.length();
  bufferSize = n;
  
  for (i = 0; i < n; i++) {
    ch = InFileBuffer[i];

    switch (state) {
      case InBlockComment : {
        if ( ch == '\n' ) {
          returnBuffer.append(ch);
          break;
        }
        if ( ch != '*' ) {
          break;
        }
        nextI = i + 1;
        if ( nextI >= bufferSize ) {
          break;
        }
        nextCh = InFileBuffer[nextI];
        if ( nextCh == '/' ) {
          i = nextI;
          state = InNoComment;
        }
        break;
      }
      case InLineComment : {
        if ( ch == '\n' ) {
          returnBuffer.append(ch);
          state = InNoComment;
        }
        break;
      }
      case InNoComment : {
        if ( ch == '/' ) {
          nextI = i + 1;
          if ( nextI >= bufferSize ) {
            break;
          }
          nextCh = InFileBuffer[nextI];
          if ( nextCh == '/' ) {
            i = nextI;
            state = InLineComment;
            break;
          }
          if ( nextCh == '*' ) {
            i = nextI;
            state = InBlockComment;
            break;
          }
        }
        returnBuffer.append(ch);
        break;
      }
    }
  }
  return returnBuffer;
}

/*****************************************************************************!
 * Function : ProcessFileLines
 *****************************************************************************/
void
ProcessFileLines
(QStringList InLines, int InTrack)
{
  QString                               line;
  QString                               line1;
  int                                   n;
  int                                   i;

  n = InLines.size();

  for (i = 0; i < n; i++) {
    line = InLines[i];
    line1 = ProcessFileLine(line);
    line1 = line1.trimmed();
    if ( ! mainStringsCount->Exists(line1) ) {
      mainStringsCount->Append(new StringCount(line1, i + 1));
    } else {
      mainStringsCount->IncreaseStringCount(line1, i + 1, InTrack);
    }
  }
}

/*****************************************************************************!
 * Function : DisplayFileLines
 *****************************************************************************/
void
DisplayFileLines
(bool InDisplayLineNumbers)
{
  int                                   p;
  int                                   n;
  int                                   k;
  int                                   m;
  StringCount*                          line2;
  int                                   i;
  
  m = mainStringsCount->GetSize();
  printf("%d\n", m);
  for (i = 0; i < m; i++) {
    line2 = mainStringsCount->GetByIndex(i);
    printf("%d\t%d\t%s\n", line2->GetCount(0), line2->GetCount(1), line2->GetString().toStdString().c_str());
    if ( !InDisplayLineNumbers ) {
      continue;
    }
    for ( p = 0 ; p < 2 ; p++ ) {
      n = line2->GetLineNumberCount(p);
      printf("\t\t");
      for ( k = 0 ; k < n ; k++ ) {
        printf("%d ", line2->GetLineNumber(k, p));
      }
      printf("\n");
    }
  }
}

/*****************************************************************************!
 * Function : ProcessFileLine
 *****************************************************************************/
QString
ProcessFileLine
(QString InLine)
{
  int                                   i;
  int                                   n;
  QChar                                 ch;
  QString                               returnLine;
  enum {
    InStartOfLine,
    InNonSpaces,
    InSpaces
  } state = InStartOfLine;

  
  n = InLine.length();
  for (i = 0; i < n; i++) {
    ch = InLine[i];
    switch ( state ) {
      //!
      case InStartOfLine : {
        if ( ch.isSpace() ) {
          break;
        }
        returnLine.append(ch);
        state = InNonSpaces;
        break;
      }
      //!
      case InNonSpaces : {
        if ( ch.isSpace() ) {
          returnLine.append(' ');
          state = InSpaces;
          break;
        }
        returnLine.append(ch);
        break;
      }

      //!
      case InSpaces : {
        if ( ch.isSpace() ) {
          break;
        }
        returnLine.append(ch);
        state = InNonSpaces;
        break;
      }
    }
  }
  return returnLine;
}

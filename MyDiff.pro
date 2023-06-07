QMAKE_CXX                       = @echo [C+] $@ && g++ 

QMAKE_CC                        = @echo [C+] $@ && gcc 

QMAKE_MOC                       = @echo [MO] $@ && moc 

QMAKE_LINK                      = @echo [LD] $@ && g++ 

QMAKE_RC                        = @echo [RC] $@ && windres 

QMAKE_LFLAGS_WINDOWS            += -Wl,--subsystem,console

CONFIG                          += console

QMAKE_CXXFLAGS                  += -DTRACE_USE

QMAKE_LIBS                      = 

TARGET                          = mydiff

VERSION                         = "0.0.0"
QMAKE_TARGET_COMPANY            = "Vertiv Company"

QMAKE_TARGET_PRODUCT            = "MyDiff"

QMAKE_TARGET_COPYRIGHT          = "Copyright (C) 2023 Vertiv Company"

QT                              += core gui widgets

DEFINES                         += \

SOURCES                         += \
				   LineDiffTable.cpp                            \
				   MainDisplayWindow.cpp                        \
				   MainWindow.cpp                               \
				   SourceLineNumber.cpp                         \
				   SourceLineReference.cpp                      \
				   StringCount.cpp                              \
				   StringCountList.cpp                          \
				   SystemConfig.cpp                             \
				   main.cpp                                     \
				   trace.cpp                                    \

HEADERS                         += \
				   LineDiffTable.h                              \
				   MainDisplayWindow.h                          \
				   MainWindow.h                                 \
				   SourceLineNumber.h                           \
				   SourceLineReference.h                        \
				   StringCount.h                                \
				   StringCountList.h                            \
				   SystemConfig.h                               \
				   trace.h                                      \
				   trace_printf.h                               \
                                   
FORMS                           += \

QMAKE_EXTRA_TARGETS             += junkclean
junkclean.target                = junkclean
junkclean.commands              = @echo Removing junk                           \
                                  && rm -rf $(wildcard core* *~)
junkclean.depends               = 

QMAKE_EXTRA_TARGETS             += cleanall
cleanall.target                 = cleanall
cleanall.commands               = 
cleanall.depends                = junkclean                                     \
                                  clean

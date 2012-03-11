# -------------------------------------------------
# Project created by QtCreator 2010-01-25T16:54:18
# -------------------------------------------------
TARGET = dalculator
TEMPLATE = app
QT += network
SOURCES += main.cpp \
    calc/calc.cpp \
    calc/settinghandler.cpp \
    calc/mathfunction.cpp \
    calc/calc_private.cpp \
    mainwindow.cpp \
    updatechecker.cpp \
    qtcalc.cpp \
    varswidget.cpp \
    funcswidget.cpp \
    funcwidget.cpp \
    varwidget.cpp \
    varsfuncsdialog.cpp \
    dialogabout.cpp \
    dini/inivalue.cpp \
    dini/inisection.cpp \
    dini/inifile.cpp \
    dini/dini_private.cpp \
    calchistorydialog.cpp
HEADERS += mainwindow.h \
    calchistorydialog.h \
    calc/calc_private.h \
    calc/calc.h \
    calc/settinghandler.h \
    calc/types.h \
    calc/mathfunction.h \
    calc/error.h \
    updatechecker.h \
    qtcalc.h \
    varswidget.h \
    funcswidget.h \
    funcwidget.h \
    varwidget.h \
    varsfuncsdialog.h \
    dialogabout.h \
    dini/inivalue.h \
    dini/inisection.h \
    dini/inifile.h \
    dini/dini_private.h \
    dini/dini.h
FORMS += mainwindow.ui \
    varsfuncsdialog.ui \
    dialogabout.ui \
    calchistorydialog.ui
RESOURCES += resources.qrc
TRANSLATIONS = resources/lang_en.ts \
               resources/lang_nl.ts

target.path = /usr/bin
INSTALLS += target

data.path = /usr/share/dalculator/data
data.files = resources/lang_en.qm \
             resources/lang_nl.qm \
             resources/icons/dalculator.ico
INSTALLS += data

help.path = /usr/share/dalculator/data/help
help.files = resources/help/*
INSTALLS += help

# -------------------------------------------------
# Project created by QtCreator 2010-01-25T16:54:18
# -------------------------------------------------
TARGET = dalculator
TEMPLATE = app

QT += widgets network

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

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

desktop.path = /usr/share/applications
desktop.files = resources/dalculator.desktop
INSTALLS += desktop

icon16.path = /usr/share/icons/hicolor/16x16/apps
icon16.files = resources/icons/16/dalculator.png
icon24.path = /usr/share/icons/hicolor/24x24/apps
icon24.files = resources/icons/24/dalculator.png
icon32.path = /usr/share/icons/hicolor/32x32/apps
icon32.files = resources/icons/32/dalculator.png
icon48.path = /usr/share/icons/hicolor/48x48/apps
icon48.files = resources/icons/48/dalculator.png
icon64.path = /usr/share/icons/hicolor/64x64/apps
icon64.files = resources/icons/64/dalculator.png
INSTALLS += icon16 icon24 icon32 icon48 icon64

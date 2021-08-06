QT       += core sql

QT       -= gui

TARGET    = test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

ios {
    CONFIG(debug, debug|release) {
        LIBS += ../../sqlitecipher/plugins/sqldrivers/ -lsqlitecipher_debug
    } else {
        LIBS += ../../sqlitecipher/plugins/sqldrivers/ -lsqlitecipher
    }
}

SOURCES += main.cpp

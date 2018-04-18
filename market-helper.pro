#-------------------------------------------------
#
# Project created by QtCreator 2014-11-29T21:43:21
#
#-------------------------------------------------

QT       += core gui xml
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = market-helper
TEMPLATE = app

SOURCES += main.cpp\
    offer_editor_dialog.cpp \
    main_window.cpp \
    item_editor_dialog.cpp \
    item_info.cpp \
    offer_info.cpp \
    market_model.cpp \
    market_info.cpp \
    item_profit_delegate.cpp \
    transactions_report_dialog.cpp \
    transaction_report_model.cpp \
    localized_spin_box.cpp \
    market_view.cpp \
    offer_history_dialog.cpp \
    offer_history_model.cpp

HEADERS  += \
    item_info.h \
    offer_editor_dialog.h \
    main_window.h \
    item_editor_dialog.h \
    offer_info.h \
    market_model.h \
    market_info.h \
    item_profit_delegate.h \
    transactions_report_dialog.h \
    transaction_report_model.h \
    localized_spin_box.h \
    market_view.h \
    offer_history_dialog.h \
    offer_history_model.h

FORMS    += \
    offer_editor_dialog.ui \
    main_window.ui \
    item_editor_dialog.ui \
    transactions_report_dialog.ui \
    offer_history_dialog.ui

RESOURCES += \
    resources.qrc

# This file is generated automatically. Do not edit.
# Use project properties -> Build -> Qt -> Expert -> Custom Definitions.
TEMPLATE = app
DESTDIR = dist/Release/GNU-Linux-x86
TARGET = OpenSimulator
VERSION = 1.0.0
CONFIG -= debug_and_release app_bundle lib_bundle
CONFIG += release 
PKGCONFIG +=
QT = core gui
SOURCES += mainwindow.cpp testVerts.cpp Body_trimesh.cpp main.cpp Simulation.cpp Body_Face.cpp about.cpp Body_Edge.cpp Body_Vertex.cpp Body_object.cpp Body_polymesh.cpp glwidget.cpp Body_sphere.cpp
HEADERS += Body_Vertex.h Body_polymesh.h Body_trimesh.h about.h Body_Edge.h Simulation.h glwidget.h ui_mainwindow.h Body_Face.h ui_information.h Body_object.h ui_info.h ui_about.h Body_sphere.h mainwindow.h
FORMS += about.ui mainwindow.ui
RESOURCES +=
TRANSLATIONS +=
OBJECTS_DIR = build/Release/GNU-Linux-x86
MOC_DIR = 
RCC_DIR = 
UI_DIR = 
QMAKE_CC = gcc
QMAKE_CXX = g++
DEFINES += 
INCLUDEPATH += 
LIBS += 

IAMTOP=1
FROM_TOP=1
TOPDIR=$(CURDIR)

PACKAGE_NAME=Camera1394

# Compilador
CXX=g++
CXXFLAGS = -Wall -O2  `wx-config --cxxflags` #-DVERBOSE

# Banderas de bibliotecas
#export GTK_LDFLAGS=`pkg-config --libs gtkmm-2.4`

# Banderas aplicadas de forma global
INCLUDE_CXXFLAGS=-I $(TOPDIR)/include
LD_CXXFLAGS= `wx-config --libs` 
LIBS= -lpthread

# Globalización de variables comunes
export FROM_TOP TOPDIR CXX CXXFLAGS INCLUDE_CXXFLAGS LD_CXXFLAGS LIBS OBJS_DIR

# Objetivos
SUBDIRS = src examples

include Makefile-rules

#                           -------------------------------------
#                           Leonardo Library
#                           Makefile (C) 2001-2003 Camil Demetrescu, Gianni Bevacqua.
#                           All rights reserved.
#                           Last changed:   $Date: 2003/05/15 19:34:59 $
#                           Changed by:     $Author: demetres $   
#                           Revision:       $Revision: 1.2 $    
#                           -------------------------------------

#                           -------------------------------------
#                           Macros
#                           -------------------------------------
C        =                  gcc -O4 -I../include
CC       =                  g++ -O4 -I../include -I/usr/local/qt/include -DNULL=0L -DBOOL=Bool
LSL_CORE =                  ../src/core/common/*.c ../src/core/posix/*.c
LSL_QT   =                  ../src/gui/qt/*.c
MOC      =                  /usr/local/qt/bin/moc

#                           -------------------------------------
#                           Rules
#                           -------------------------------------

mac:
	                        cd bin; $(C) $(LSL_CORE) -D__LSL_PLATFORM__=1 -c

linux:
	                        cd bin; $(C) $(LSL_CORE) -D__LSL_PLATFORM__=2 -c

qt:
	                        cd bin; $(CC) $(LSL_CORE) -D__LSL_PLATFORM__=2 -c
	                        cd bin; cd ../src/gui/qt; rm -f  *.moc.c
	                        cd bin; cd ../src/gui/qt; $(MOC) ../../../include/LApplication_qt.h  -o  LApplication_qt.moc.c
	                        cd bin; cd ../src/gui/qt; $(MOC) ../../../include/LMenu_qt.h  -o  LMenu_qt.moc.c	                        
	                        cd bin; cd ../src/gui/qt; $(MOC) ../../../include/LRadioButton_qt.h  -o  LRadioButton_qt.moc.c
	                        cd bin; cd ../src/gui/qt; $(MOC) ../../../include/LTextEditor_qt.h  -o  LTextEditor_qt.moc.c
	                        cd bin; cd ../src/gui/qt; $(MOC) ../../../include/LPushButton_qt.h  -o  LPushButton_qt.moc.c
	                        cd bin; cd ../src/gui/qt; $(MOC) ../../../include/LCheckBox_qt.h  -o  LCheckBox_qt.moc.c
	                        cd bin; cd ../src/gui/qt; $(MOC) ../../../include/LScrollBar_qt.h  -o  LScrollBar_qt.moc.c
	                        cd bin; cd ../src/gui/qt; $(MOC) ../../../include/LListBox_qt.h  -o  LListBox_qt.moc.c
	                        cd bin; $(CC) $(LSL_QT) -D__LSL_PLATFORM__=2  -c	

testmac:
	                        cd bin; $(C) -D__LSL_PLATFORM__=1 *.o ../testapp/main_core.c -o testcore -lm

testlinux:
	                        cd bin; $(C) -D__LSL_PLATFORM__=2 *.o ../testapp/main_core.c -o testcore -lm

testqt:
	                        cd bin; $(CC)  -D__LSL_PLATFORM__=2 *.o ../testapp/main_gui.c -o testgui_linux -L/usr/local/qt/lib -L/usr/X11R6/lib -lm -lqt -lXext -lX11

clean:
	                        cd bin; rm -f *.o

pack:
	                        $(MAKE) clean; cd ..; tar cf ll.tar lsl; gzip ll.tar


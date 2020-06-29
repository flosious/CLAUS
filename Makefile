RM=rm -rf
CCw=i686-w64-mingw32-g++	#windows 32bit
# CCw=x86_64-w64-mingw32-g++	#windows 64bit
# don t forget: export PKG_CONFIG_PATH=/opt/gtk3-win32/lib/pkgconfig
CC=g++	#linux


#WIN		= -mwindows
#WIN		= -static
STD		= -std=c++11 -O3
#STD		= -std=c++11 -ggdb -DDEBUGGA

STDw		= -std=c++11 -static -O3
#COPT		= -export-dynamic
#GTKCFLAGS       = `PKG_CONFIG_PATH=/usr/lib/pkgconfig pkg-config --cflags --libs gtk+-3.0`
FLAGS       = `PKG_CONFIG_PATH=/usr/lib/x86_64-linux-gnu/pkgconfig pkg-config --cflags --libs gsl sqlite3 libcurl` -lpthread
FLAGSw       = `PKG_CONFIG_PATH=/opt/gsl-win32/lib/pkgconfig pkg-config --cflags --libs gsl` `PKG_CONFIG_PATH=/opt/sqlite3-win/lib/pkgconfig pkg-config --cflags --libs sqlite3`
#SQLITE      = `PKG_CONFIG_PATH=/opt/sqlite3-win/lib/pkgconfig pkg-config --cflags --libs sqlite3`
#GTKLDFLAGSw      = `PKG_CONFIG_PATH=/opt/gtk3-win32/lib/pkgconfig pkg-config --cflags --libs gtk+-3.0`

EXECUTABLE = claus
SOURCEDIR = src
BUILDDIR = build

SOURCES = $(wildcard $(SOURCEDIR)/*.cpp)
OBJECTS = $(patsubst $(SOURCEDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))
OBJECTSw = $(patsubst $(SOURCEDIR)/%.cpp,$(BUILDDIR)/%.ow,$(SOURCES))

all: dir $(BUILDDIR)/$(EXECUTABLE)

dir:
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/$(EXECUTABLE): $(OBJECTS)
	$(CC) $^ -o $@ $(STD) $(FLAGS) $(COPT)

$(OBJECTS): $(BUILDDIR)/%.o : $(SOURCEDIR)/%.cpp
	$(CC) $< -c -o $@ $(STD) $(FLAGS) $(COPT)

win: dir $(BUILDDIR)/$(EXECUTABLE).exe

$(BUILDDIR)/$(EXECUTABLE).exe: $(OBJECTSw)
	$(CCw) $^ -o $@ $(STDw) $(WIN) $(FLAGSw) $(COPT)
	$(unset $(PKG_CONFIG_PATH))

$(OBJECTSw): $(BUILDDIR)/%.ow : $(SOURCEDIR)/%.cpp
	$(CCw) $< -c -o $@ $(STDw) $(FLAGSw) $(COPT)

clean:
	$(RM) $(BUILDDIR)/*o $(BUILDDIR)/*ow

deepclean:
	$(RM) $(BUILDDIR)/*o $(BUILDDIR)/*ow $(BUILDDIR)/$(EXECUTABLE) $(BUILDDIR)/$(EXECUTABLE).exe

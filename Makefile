RELEASE = ON
CFLAGS := -Iexternal/SDL2/include -Iexternal/SDL2_image/include -Iexternal/imgui -Iexternal/imgui/backends -Iexternal/imgui/misc/cpp -Iexternal/NFDe/include -Iexternal/json -Iexternal/mINI
LDFLAGS := -Lexternal/SDL2/lib -Lexternal/SDL2_image/lib -static -Wall -lmingw32 -lSDL2main -lSDL2 external/SDL2/lib/libSDL2.a -lSDL2_image -mwindows  -Wl,--dynamicbase -Wl,--nxcompat -Wl,--high-entropy-va -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lsetupapi -lversion -luuid
ifeq ($(RELEASE), ON)
	LDFLAGS += -Wl,-subsystem,windows
endif
OBJDIR := build
BINDIR := bin

APP_DIRS = src/
APP_SRCS = $(foreach dir, $(APP_DIRS), $(wildcard $(dir)/*.cpp))
APP_OBJS = $(patsubst %.cpp, $(OBJDIR)/%.o, $(notdir $(APP_SRCS)))

IMGUI_DIRS = external/imgui external/imgui/backends external/imgui/misc/cpp
IMGUI_SRCS = $(foreach dir, $(IMGUI_DIRS), $(filter-out $(dir)/imgui_demo.cpp, $(wildcard $(dir)/*.cpp)))
IMGUI_OBJS = $(patsubst %.cpp, $(OBJDIR)/%.o, $(notdir $(IMGUI_SRCS)))

all: $(OBJDIR) $(BINDIR) $(BINDIR)/UTSaveEditor.exe

$(OBJDIR) $(BINDIR):
	mkdir -p $@

$(BINDIR)/UTSaveEditor.exe: $(APP_OBJS) $(OBJDIR)/app.res $(IMGUI_OBJS) $(OBJDIR)/nfd_win.o
	$(CXX) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: src/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OBJDIR)/app.res: resources/app.rc
	windres $< -O coff -o $@

$(OBJDIR)/nfd_win.o: external/NFDe/nfd_win.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: external/imgui/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: external/imgui/backends/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: external/imgui/misc/cpp/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJDIR) $(BINDIR)
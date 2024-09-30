RELEASE = OFF
CFLAGS += -Iexternal/SDL2/include -Lexternal/SDL2/lib -Iexternal/SDL2_image/include -Lexternal/SDL2_image/lib -Iexternal/imgui -Iexternal/imgui/backends -Iexternal/imgui/misc/cpp -Iexternal/NFDe/include -Iexternal/json/include
LDFLAGS = -Wall -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lole32 -luuid -lshell32
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
	cp external/SDL2/bin/SDL2.dll $(BINDIR)
	cp external/SDL2_image/bin/SDL2_image.dll $(BINDIR)

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
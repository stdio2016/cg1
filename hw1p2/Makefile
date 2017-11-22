ALL = workingDir/hw1p2
SRCS = Camera.cpp DisplayObject.cpp FileName.cpp LightSystem.cpp main.cpp Mesh.cpp SceneManager.cpp TextureMapping.cpp Vec3.cpp
OBJDIR = ./obj
df = $(OBJDIR)/$(*F)

CPPFLAGS += -Wall
DEPFLAGS =  -MD -MP -MM
LIBS = -lz -lm -lGL -lGLU -lglut lib/libcorona.a -lpng -ljpeg
OBJS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS))
DEPS = $(OBJS:%.o=%.d)

all: $(ALL)
workingDir/hw1p2: $(OBJS)
	$(CXX) $(LDFLAGS) $+ $(LIBS) -o $@

-include $(DEPS)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(DEPFLAGS) -MT $(df).o $< -o $(df).d
	$(CXX) -c $< $(CPPFLAGS) -o $@

clean:
	-rm $(OBJS) $(DEPS)

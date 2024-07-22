include build/conandeps.mk
CXXFLAGS            += $(CONAN_CXXFLAGS)
CPPFLAGS            += $(addprefix -I, $(CONAN_INCLUDE_DIRS))
CPPFLAGS            += $(addprefix -D, $(CONAN_DEFINES))
LDFLAGS             += $(addprefix -L, $(CONAN_LIB_DIRS))
LDLIBS              += $(addprefix -l, $(CONAN_LIBS))
EXELINKFLAGS        += $(CONAN_EXELINKFLAGS)

all:
	$(CXX) ../myapp.cpp $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) $(EXELINKFLAGS) -o myapp
         
TARGET=LBSkin
OBJECTS_DIR=obj
# as I want to support 4.8 and 5 this will set a flag for some of the mac stuff
# mainly in the types.h file for the setMacVisual which is native in Qt5
isEqual(QT_MAJOR_VERSION, 5) {
    cache()
    DEFINES +=QT5BUILD
}

MOC_DIR=moc
CONFIG-=app_bundle
QT+=gui opengl core

SOURCES += \
    src/main.cpp \
    src/MainWindow.cpp \
    src/GLWindow.cpp \
    src/SkinDeformer.cpp \
    src/SceneLoader.cpp \
    src/AIUtil.cpp \
    src/Dualquaternion.cpp

HEADERS += \
    include/MainWindow.h \
    include/GLWindow.h \
    include/SkinDeformer.h \
    include/SceneLoader.h \
    include/DataTypes.h \
    include/AIUtil.h \
    include/Dualquaternion.h \
    include/Util.h

FORMS += \
    ui/MainWindow.ui

OTHER_FILES+= \
              shaders/Phong.vs \
    shaders/diffuseVertex.glsl \
    shaders/diffuseFragment.glsl \
    shaders/colorVertex.glsl \
    shaders/colorFragment.glsl \
    shaders/TextureVertex.glsl \
    shaders/TextureFragment.glsl \
    shaders/SurfaceVertex.glsl \
    shaders/SurfaceFragment.glsl \
    shaders/DiffuseVertex.glsl \
    shaders/DiffuseFragment.glsl

CONFIG += console
CONFIG -= app_bundle
INCLUDEPATH+=./include

# use this to suppress some warning from boost
QMAKE_CXXFLAGS_WARN_ON +=  "-Wno-unused-parameter"
QMAKE_CXXFLAGS+= -msse -msse2 -msse3
macx:QMAKE_CXXFLAGS+= -arch x86_64
macx:INCLUDEPATH+=/usr/local/include/
linux-g++:QMAKE_CXXFLAGS +=  -march=native
linux-g++-64:QMAKE_CXXFLAGS +=  -march=native
# define the _DEBUG flag for the graphics lib
DEFINES +=NGL_DEBUG\
ASSIMP_DEBUG

LIBS += -L/usr/local/lib
LIBS+=-lassimp
# add the ngl lib
LIBS +=  -L/$(HOME)/NGL/lib -l NGL

unix:LIBS += -L/usr/local/lib
# add the ngl lib


# now if we are under unix and not on a Mac (i.e. linux) define GLEW
linux-*{
    linux-*:QMAKE_CXXFLAGS +=  -march=native
    linux-*:DEFINES+=GL42
    DEFINES += LINUX
}

DEPENDPATH+=include
# if we are on a mac define DARWIN
macx:DEFINES += DARWIN
# this is where to look for includes
INCLUDEPATH += $$(HOME)/NGL/include/

win32: {
    PRE_TARGETDEPS+=C:/NGL/lib/NGL.lib
    INCLUDEPATH+=-I c:/boost
    DEFINES+=GL42
    DEFINES += WIN32
    DEFINES+=_WIN32
    DEFINES+=_USE_MATH_DEFINES
    LIBS += -LC:/NGL/lib/ -lNGL
    DEFINES+=NO_DLL
}

##############################
#                            #
#     MLDemos Variables      #
#                            #
##############################

# PLEASE EDIT THIS PART TO FIT YOUR NEEDS/SETUP

win32{
	MLBUILD = C:/tmp/MLDemos/$$NAME
    CONFIG += WIN32
}else{
    MLBUILD = /tmp/MLDemos/$$NAME
}

# choices are opencv22 or opencv21
CONFIG += opencv21

CONFIG(opencv22)|CONFIG(opencv21){
	message("You have selected to use the OpenCV library, if you do not have/desire it, please modify MLDemos_variables.pri")
}

CONFIG(boost){
	message("You have selected to use the Boost headers library, if you do not have/desire it, please modify MLDemos_variables.pri")
	DEFINES += WITHBOOST
}
macx|unix{
CONFIG(boost):BOOST = /usr/local/boost_1_47_0
}else{
CONFIG(boost):BOOST = E:/DEV/boost_1_47_0
}

# PLEASE EDIT UNTIL HERE TO FIT YOUR NEEDS/SETUP

mainApp|coreLib{
}else{
	TARGET = $$qtLibraryTarget($$NAME)
        CONFIG(debug, debug|release):DESTDIR = "$$MLPATH/pluginsDebug"
        CONFIG(release, debug|release):DESTDIR = "$$MLPATH/plugins"
}
CONFIG(debug, debug|release){
	DEFINES += DEBUG
	message("debug mode")
}else{
	message("release mode")
}
macx:DEFINES += MACX
win32:DEFINES += WIN32
win32{
	CONFIG(Debug, Debug|Release){
		MOC_DIR = $${MLBUILD}/Debug
		UI_DIR = $${MLBUILD}/Debug
		RCC_DIR = $${MLBUILD}/Debug
		OBJECTS_DIR = $${MLBUILD}/Debug
	}else{
		MOC_DIR = $${MLBUILD}/Release
		UI_DIR = $${MLBUILD}/Release
		RCC_DIR = $${MLBUILD}/Release
		OBJECTS_DIR = $${MLBUILD}/Release
	}
}else{
	MOC_DIR = $${MLBUILD}/build
	UI_DIR = $${MLBUILD}/build
	RCC_DIR = $${MLBUILD}/build
	OBJECTS_DIR = $${MLBUILD}/build
}

DEPENDPATH += . \
		.. \
		$${MLPATH}/Core \
		$${MLPATH}/_3rdParty
INCLUDEPATH += . \
		$${MLPATH}/Core \
		$${MLPATH}/MLDemos \
		$${MLPATH}/_3rdParty

unix{
INCLUDEPATH += /usr/include/qt4 \
	/usr/include/qt4/QtCore \
	/usr/include/qt4/QtGui \
	/usr/include/qt4/QtSvg \
	/usr/include/qt4/QtOpenGL
LIBS += -L/usr/local/lib
}

CONFIG(coreLib){
}else{
	LIBS += -L$$MLPATH/Core -lCore
}
LIBS += -L$$MLPATH/_3rdParty -l3rdParty

################################
# Turn the bloody warnings off #
################################
macx|unix {
	QMAKE_CXXFLAGS_WARN_ON = ""
	QMAKE_CXXFLAGS += -Wno-all
	QMAKE_CXXFLAGS += -Wno-endif-labels
	QMAKE_CXXFLAGS += -Wno-unused-variable
	QMAKE_CXXFLAGS += -Wno-unused-parameter
	QMAKE_CXXFLAGS += -Wno-switch
	QMAKE_CXXFLAGS += -Wtrigraphs
	QMAKE_CXXFLAGS += -Wreturn-type
	#QMAKE_CXXFLAGS += -Wnon-virtual-dtor
	QMAKE_CXXFLAGS += -Woverloaded-virtual
	#QMAKE_CXXFLAGS += -Wunused-variable
	#QMAKE_CXXFLAGS += -Wunused-value
	QMAKE_CXXFLAGS += -Wunknown-pragmas
	QMAKE_CXXFLAGS += -Wno-shadow
	QMAKE_CXXFLAGS += -Wno-deprecated-declarations
	QMAKE_CXXFLAGS += -Wno-missing-braces
}

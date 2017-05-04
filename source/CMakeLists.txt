#Generate a ROOT dictionary.
ROOT_GENERATE_DICTIONARY(root2tikzDict
	../include/TikzPlot.hpp
	LINKDEF ../include/root2tikz_LinkDef.h
	MODULE root2tikz)

#Install the ROOT 6 PCM and rootmap files.
if (${ROOT_VERSION} VERSION_GREATER "6.0")
	execute_process(COMMAND sed -i.bak s/\\.so/.dylib/
		${CMAKE_CURRENT_BINARY_DIR}/libroot2tikz.rootmap)
	install(FILES
			${CMAKE_CURRENT_BINARY_DIR}/libroot2tikz_rdict.pcm
			${CMAKE_CURRENT_BINARY_DIR}/libroot2tikz.rootmap
		DESTINATION lib/)
endif (${ROOT_VERSION} VERSION_GREATER "6.0")

#Create the root2tikz library with the ROOT dictionary.
add_library(root2tikz SHARED TikzPlot.cpp
	TikzTh2.cpp root2tikzDict)

#Link to ROOT.
target_link_libraries(root2tikz ${ROOT_LIBRARIES})

#Set the install directory.
install(TARGETS root2tikz DESTINATION lib/)

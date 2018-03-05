#Generate a ROOT dictionary.
ROOT_GENERATE_DICTIONARY(root2tikzDict
	../include/TikzPlot.hpp
	TikzPicture.hpp
	TikzOptions.hpp
	PgfPlotsPlot.hpp
	PgfPlotsAxis.hpp
	PgfPlotsGroupPlot.hpp
	PgfPlotsGroupSubPlot.hpp
	LINKDEF ../include/root2tikz_LinkDef.h
	MODULE root2tikz)

set(ROOT2TIKZ_SOURCES
		TikzPlot.cpp
		TikzPicture.cpp
		TikzOptions.cpp
		PgfPlotsPlot.cpp
		PgfPlotsAxis.cpp
		PgfPlotsGroupPlot.cpp
		PgfPlotsGroupSubPlot.cpp
	)

#Create the root2tikz library with the ROOT dictionary.
add_library(root2tikz SHARED ${ROOT2TIKZ_SOURCES} root2tikzDict)

#Link to ROOT.
target_link_libraries(root2tikz ${ROOT_LIBRARIES})

#Set the install directory.
install(TARGETS root2tikz DESTINATION lib/)

#Install the ROOT 6 PCM and rootmap files.
if (${ROOT_VERSION} VERSION_GREATER "6.0")
	add_custom_command(TARGET root2tikz POST_BUILD
		COMMAND sed -i.bak s/\\.so/.dylib/
		${CMAKE_CURRENT_BINARY_DIR}/libroot2tikz.rootmap
		COMMENT "Modifying libroot2tikz.rootmap for Mac extension.")
	install(FILES
			${CMAKE_CURRENT_BINARY_DIR}/libroot2tikz_rdict.pcm
			${CMAKE_CURRENT_BINARY_DIR}/libroot2tikz.rootmap
		DESTINATION lib/)
endif (${ROOT_VERSION} VERSION_GREATER "6.0")


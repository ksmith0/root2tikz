
#Create the root2tikz library with the ROOT dictionary.
add_library(root2tikz SHARED
	TikzTh1.cpp TikzTh2.cpp)

#Link to ROOT.
target_link_libraries(root2tikz ${ROOT_LIBRARIES})

#Set the install directory.
install(TARGETS root2tikz DESTINATION lib/)

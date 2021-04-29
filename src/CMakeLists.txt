add_executable(mini-mc
    main.cpp
    opengl/context.cpp
    opengl/shader.cpp
    opengl/camera.cpp
)

target_compile_features(mini-mc PRIVATE cxx_std_20)
target_include_directories(mini-mc PUBLIC .)
target_include_directories(mini-mc SYSTEM PRIVATE ../3rd_party/glm-0.9.9.8 ../3rd_party/glad-gl4.6-core/include)
target_link_libraries(mini-mc PRIVATE glfw glad GL dl)



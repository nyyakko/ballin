add_subdirectory(source)
add_subdirectory(include/${PROJECT_NAME})

add_executable(${PROJECT_NAME} "${ballin_SourceFiles}")

if (ENABLE_CLANGTIDY)
    enable_clang_tidy()
endif()

if (ENABLE_CPPCHECK)
    enable_cppcheck()
endif()

target_include_directories(${PROJECT_NAME} 
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" 
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/ballin"
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)

target_link_options(${PROJECT_NAME} PRIVATE ${ballin_LinkerOptions})
target_compile_options(${PROJECT_NAME} PRIVATE ${ballin_CompilerOptions})
target_link_libraries(${PROJECT_NAME} PRIVATE ${ballin_ExternalLibraries})

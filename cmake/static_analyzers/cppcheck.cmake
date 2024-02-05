function(enable_cppcheck)

    find_program(CPPCHECK cppcheck)

    if (NOT CPPCHECK)
        message(WARNING "[${PROJECT_NAME}] Couldn't find a valid ``cppcheck`` installation.")
        return()
    endif()

    if(CMAKE_GENERATOR MATCHES ".*Visual Studio.*")
        set(CPPCHECK_TEMPLATE "vs")
    else()
        set(CPPCHECK_TEMPLATE "gcc")
    endif()

    if("${CPPCHECK_OPTIONS}" STREQUAL "")
        set(CMAKE_CXX_CPPCHECK ${CPPCHECK}
            --template=${CPPCHECK_TEMPLATE}
            --enable=style,performance,warning,portability
            --inline-suppr
            --suppress=cppcheckError
            --suppress=internalAstErrorasd
            --suppress=unmatchedSuppression
            --suppress=passedByValue
            --suppress=syntaxError
            --suppress=preprocessorErrorDirective
            --inconclusive
            --error-exitcode=2)
    else()
        set(CMAKE_CXX_CPPCHECK ${CPPCHECK} --template=${CPPCHECK_TEMPLATE} ${CPPCHECK_OPTIONS})
    endif()

    if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
        set(CMAKE_CXX_CPPCHECK ${CMAKE_CXX_CPPCHECK} --platform=win64)
    endif()

    if(NOT "${CMAKE_CXX_STANDARD}" STREQUAL "")
        set(CMAKE_CXX_CPPCHECK ${CMAKE_CXX_CPPCHECK} CACHE INTERNAL --std=c++${CMAKE_CXX_STANDARD})
    elseif(NOT "${CMAKE_C_STANDARD}" STREQUAL "")
        set(CMAKE_C_CPPCHECK ${CMAKE_C_CPPCHECK} CACHE INTERNAL --std=c${CMAKE_C_STANDARD})
    endif()
endfunction()

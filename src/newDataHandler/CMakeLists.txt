FILE( GLOB_RECURSE NEWDATAHANDLER_SRC "*.cpp" "*.h" )

ADD_SUBDIRECTORY( structuralTypes )

ADD_LIBRARY( OWnewDataHandler ${NEWDATAHANDLER_SRC} )
TARGET_LINK_LIBRARIES( OWnewDataHandler OWcommon ${Boost_LIBRARIES} )

# Unit tests
IF( OW_COMPILE_TESTS )
    CXXTEST_ADD_TESTS_FROM_LIST( "${NEWDATAHANDLER_SRC}" "OWnewDataHandler" )

    # Copy fixtures if existend
    IF( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures )
        ADD_CUSTOM_TARGET( OWnewDataHandler_CopyFixtures
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures ${CMAKE_BINARY_DIR}/dataHandler/fixtures/
            COMMENT "Copy fixtures of OWnewDataHandler"
        )
        ADD_DEPENDENCIES( OWnewDataHandler OWnewDataHandler_CopyFixtures )
    ENDIF( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures )
ENDIF( OW_COMPILE_TESTS )

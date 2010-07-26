FILE( GLOB NEWDATAHANDLER_SRC "*.cpp" "*.h" )

ADD_LIBRARY( OWnewDataHandler ${NEWDATAHANDLER_SRC} )
TARGET_LINK_LIBRARIES( OWnewDataHandler OWcommon ${Boost_LIBRARIES} )

# Unit tests
IF( OW_COMPILE_TESTS )
    CXXTEST_ADD_TESTS_FROM_LIST( "${DATAHANDLER_SRC}" "OWnewDataHandler" )

    # Copy fixtures if existend
    IF( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures )
        ADD_CUSTOM_TARGET( OWdataHandler_CopyFixtures
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures ${CMAKE_BINARY_DIR}/dataHandler/fixtures/
            COMMENT "Copy fixtures of OWnewDataHandler"
        )
        ADD_DEPENDENCIES( OWdataHandler OWdataHandler_CopyFixtures )
    ENDIF( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test/fixtures )
    IF( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/io/test/fixtures )
        ADD_CUSTOM_TARGET( OWdataHandlerIO_CopyFixtures
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/io/test/fixtures ${CMAKE_BINARY_DIR}/dataHandler/io/fixtures/
            COMMENT "Copy fixtures of OWnewDataHandler IO"
        )
        ADD_DEPENDENCIES( OWdataHandler OWdataHandlerIO_CopyFixtures )
    ENDIF( EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/io/test/fixtures )
ENDIF( OW_COMPILE_TESTS )

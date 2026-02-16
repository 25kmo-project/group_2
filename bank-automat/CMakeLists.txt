cmake_minimum_required(VERSION 3.19)
project(bank-automat LANGUAGES CXX)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Test Widgets Svg Network)

qt_standard_project_setup()

qt_add_executable(bank-automat
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    images.qrc
    accountselect.h accountselect.cpp accountselect.ui
    account.h account.cpp account.ui
    avatar.h avatar.cpp
    logevents.h logevents.cpp
    logs.h logs.cpp
    adminwindow.h adminwindow.cpp adminwindow.ui
    user.h user.cpp
    userdata.h userdata.cpp
    accounts.h accounts.cpp
    accountsdata.h accountsdata.cpp
    card.h card.cpp
    carddata.h carddata.cpp
    apiclient.cpp
    apiclient.h

)


target_link_libraries(bank-automat
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Test
        Qt::Svg
        Qt::Network
)


add_executable(logintest
    logintest.cpp
)


target_link_libraries(logintest
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Test
        Qt::Svg
        Qt::Network   
)


include(CTest)
add_test(NAME LoginTest COMMAND logintest)

include(GNUInstallDirs)

install(TARGETS bank-automat
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET bank-automat
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

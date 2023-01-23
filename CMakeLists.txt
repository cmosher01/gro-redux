cmake_minimum_required(VERSION 3.25.2)

set(CMAKE_VERBOSE_MAKEFILE TRUE CACHE BOOL "Echo make commands.")
set(PACKAGE_SUITE TRUE CACHE BOOL "Build entire suite of packages; otherwise just ZIP file.")

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)





set(APP_NAME gro)

project(${APP_NAME}
    VERSION 1.7.0
    DESCRIPTION "Genealogy Research Organizer"
    HOMEPAGE_URL https://github.com/cmosher01/gro-redux
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(PROJECT_FOURCC gror)
set(PROJECT_VENDOR "nu.mine.mosher")
set(CPACK_PACKAGE_VENDOR "${PROJECT_VENDOR}")
set(CPACK_PACKAGE_CONTACT "cmosher01@gmail.com")
set(CPACK_PACKAGE_DESCRIPTION "${PROJECT_DESCRIPTION}")

file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory; make a build subdirectory.")
endif()

set(CMAKE_INSTALL_PREFIX "C:/Program Files/${PROJECT_NAME}")
unset(CPACK_PACKAGING_INSTALL_PREFIX)

set(sources
About.cpp
AppInfo.cpp
BigView.cpp
BirthCalc.cpp
ChildFrm.cpp
ChooseFont.cpp
ConfirmDelete.cpp
Connect.cpp
DateValue.cpp
DlgWebServer.cpp
Dlgsavechanges.cpp
DocSize.cpp
EditCitation.cpp
EditDate.cpp
EditEvent.cpp
EditName.cpp
EditRepo.cpp
EditSex.cpp
EditSource.cpp
EditSubm.cpp
Find.cpp
GedCmd.cpp
Gedtree.cpp
Gedtreedoc.cpp
Gedtreeview.cpp
HeadNotes.cpp
Holiday.cpp
IndiClip.cpp
Mainfrm.cpp
PickRels.cpp
PickRepo.cpp
PickSource.cpp
Places.cpp
Progress.cpp
Register.cpp
Setup.cpp
Stdafx.cpp
UnicodeSel.cpp
WebClient.cpp
WebServer.cpp
Zoom.cpp
attr.cpp
cita.cpp
date.cpp
event.cpp
fami.cpp
gedline.cpp
gedrecord.cpp
gedtreeviewdl.cpp
gedtreeviewil.cpp
gedtreeviewin.cpp
gedtreeviewpd.cpp
head.cpp
indi.cpp
mydc.cpp
name.cpp
note.cpp
repo.cpp
rfor.cpp
sour.cpp
util.cpp
webbrowser.cpp
)

set(resources
gedtree.rc
Res/Gedtree.rc2
Res/Toolbar.bmp
Res/citation.ico
Res/ged_il.ico
Res/ged_in.ico
Res/ged_pd.ico
Res/ged_tre.ico
Res/gedtree.ico
Res/idd_edit.ico
Res/idd_repo.ico
Res/menudef.h
Res/source.ico
)

set(CMAKE_MFC_FLAG 2)

add_executable(${APP_NAME} WIN32 ${sources} ${resources})

target_link_libraries(${APP_NAME} PRIVATE "version.lib")

set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${resources}")

set(CMAKE_INSTALL_MFC_LIBRARIES TRUE)
set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
include(InstallRequiredSystemLibraries)

install(TARGETS ${APP_NAME}
    RUNTIME_DEPENDENCIES
        DIRECTORIES ${CMAKE_SYSTEM_LIBRARY_PATH}
        PRE_EXCLUDE_REGEXES "api-ms-" "ext-ms-"
        POST_EXCLUDE_REGEXES ".*system32/.*\\.dll"
    BUNDLE DESTINATION .
    RESOURCE DESTINATION share/Resources PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)

set(CPACK_GENERATOR ZIP)

if(PACKAGE_SUITE)
    set(CPACK_GENERATOR ${CPACK_GENERATOR} TGZ 7Z NSIS)
endif()

include(CPack)

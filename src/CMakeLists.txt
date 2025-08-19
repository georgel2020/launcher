# Define the Qt resource file.
qt_add_resources(RESOURCES ../resources/launcher.qrc)

# Define the executable.
qt_add_executable(launcher
        main.cpp
        # App.
        app/Launcher.cpp app/Launcher.h
        # Core.
        core/ConfigLoader.cpp core/ConfigLoader.h
        core/HistoryManager.cpp core/HistoryManager.h
        core/HotkeyManager.cpp core/HotkeyManager.h
        # Common.
        common/Action.h
        common/ResultItem.h
        common/Constants.h
        # Modules.
        common/IModule.h
        modules/LauncherCommands.cpp modules/LauncherCommands.h
        modules/EverythingSearch.cpp modules/EverythingSearch.h
        modules/Calculator.cpp modules/Calculator.h
        modules/AppsSearch.cpp modules/AppsSearch.h
        # Widgets.
        widgets/ResultItemWidget.cpp widgets/ResultItemWidget.h
        widgets/ResultItemDelegate.cpp widgets/ResultItemDelegate.h
        ${RESOURCES}
)

# Link Qt libraries.
target_link_libraries(launcher PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
)
add_custom_command(
        TARGET launcher POST_BUILD
        COMMAND windeployqt
        --dir "$<TARGET_FILE_DIR:launcher>"
        "$<TARGET_FILE:launcher>"
        COMMENT "Running windeployqt to deploy Qt libraries. "
)

# Link muparser library.
target_link_libraries(launcher PRIVATE
        muparser
)

# Link Everything SDK.
target_include_directories(launcher PRIVATE
        "${PROJECT_SOURCE_DIR}/third-party/everything-sdk/include"
)
target_link_libraries(launcher PRIVATE
        "${PROJECT_SOURCE_DIR}/third-party/everything-sdk/lib/Everything64.lib"
)
add_custom_command(
        TARGET launcher POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/third-party/everything-sdk/dll/Everything64.dll"
        "$<TARGET_FILE_DIR:launcher>"
        COMMENT "Copying Everything DLL to output directory. "
)

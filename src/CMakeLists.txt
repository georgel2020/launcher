# Define the Qt resource file.
qt_add_resources(RESOURCES ../resources/launcher.qrc)

# Define the executable.
qt_add_executable(launcher
        main.cpp
        # Core app.
        app/Launcher.cpp app/Launcher.h
        # Common.
        common/Action.h
        common/ResultItem.h
        common/Constants.h
        # Modules.
        modules/IModule.h
        modules/LauncherCommands.cpp modules/LauncherCommands.h
        # Widgets.
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

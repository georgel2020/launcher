# Define the executable.
qt_add_executable(launcher
        main.cpp
        # Core app.
        app/Launcher.cpp app/Launcher.h
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

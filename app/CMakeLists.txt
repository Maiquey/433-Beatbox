# Build the app which supports ALSA, using the HAL

include_directories(include)
file(GLOB MY_SOURCES "src/*.c")
add_executable(beatbox ${MY_SOURCES})

# Make use of the HAL library
target_link_libraries(beatbox LINK_PRIVATE hal)

# ---------------- NEW! ----------------
# ALSA support
find_package(ALSA REQUIRED)
target_link_libraries(beatbox LINK_PRIVATE asound)

# PTHREAD support
find_package(Threads REQUIRED)
target_link_libraries(beatbox LINK_PRIVATE Threads::Threads)

# Copy executable to final location (change `beatbox` to project name as needed)
add_custom_command(TARGET beatbox POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:beatbox>"
     "~/cmpt433/public/myApps/beatbox" 
  COMMENT "Copying ARM executable to public NFS directory")

# ---------------- NEW! ----------------
# Copy the WAV folder to NFS
add_custom_command(TARGET beatbox POST_BUILD 
COMMAND "${CMAKE_COMMAND}" -E copy_directory
   "${CMAKE_SOURCE_DIR}/beatbox-wav-files"
   "~/cmpt433/public/myApps/beatbox-wav-files" 
COMMENT "Copying WAVE files to public NFS directory")
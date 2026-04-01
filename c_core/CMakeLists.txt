cmake_minimum_required(VERSION 3.28)
project(PrismLink C Core)

# --- CRITICAL: Turn on CTest ---
enable_testing()

# ==========================================
# 1. THE FACTORY FLOOR SETUP
# Route all compiled files strictly to the build/ folder
# ==========================================
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)

# (Optional) If you ever put global configs in include/, uncomment this:
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)


# ==========================================
# 2. FORGE THE TOOLS (The lib/ folder)
# ==========================================
# Tool 1: Data Structures
add_library(ds STATIC lib/ds/generic_queue.c)

# Tell the Foreman the manual (.h) is right next to the code in lib/ds/
target_include_directories(ds PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/ds)


# ==========================================
# 3. ASSEMBLE THE PRODUCT (The src/ folder)
# ==========================================
add_executable(prismlink_app 
    src/main.c 
)

# 1. Give the app the internal intercom for its own files
target_include_directories(prismlink_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# 2. Give the app permission to use the ds tool
target_link_libraries(prismlink_app PRIVATE ds)


# ==========================================
# 4. THE QUALITY CONTROL AUTOMATION (CTest)
# ==========================================
# Find every single .c file sitting in the tests/ folder
file(GLOB TEST_FILES "tests/*.c")

# Loop through each test file and build a dedicated test machine
foreach(TEST_SRC ${TEST_FILES})
    # Extract just the name (e.g., gets "queue_test" from "tests/queue_test.c")
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    
    # Create the executable machine
    add_executable(${TEST_NAME} ${TEST_SRC})
    
    # Link it to ALL your tools so the test has everything it needs
    target_link_libraries(${TEST_NAME} PRIVATE ds)
    
    # Register this machine with CTest
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
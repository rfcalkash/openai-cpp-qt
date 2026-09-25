cmake_minimum_required(VERSION 3.0)

project(examples)

# You can comment/Uncomment this line if you wish to hide/show verbose output
# add_definitions(-DOPENAI_VERBOSE_OUTPUT=1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

message("Using compiler ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

set (SOURCES_EXAMPLES
    00-showcase
    01-model
    02-completion
    03-edit
    04-image
    05-embedding
    06-file
    07-fine-tune
    09-instances
    10-chat
    11-audio
    12-moderation
)

set (TARGETS_EXAMPLES
    00-showcase
    00-showcase_async
    01-model
    01-model_async
    02-completion
    02-completion_async
    03-edit
    03-edit_async
    04-image
    04-image_async
    05-embedding
    05-embedding_async
    06-file
    06-file_async
    07-fine-tune
    07-fine-tune_async
    09-instances
    09-instances_async
    10-chat
    10-chat_async
    11-audio
    11-audio_async
    12-moderation
    12-moderation_async
)

foreach( name ${TARGETS_EXAMPLES} )
    add_executable(${name} ${name}.cpp)
    set_property(TARGET ${name} PROPERTY CXX_STANDARD 17)
    set_property(TARGET ${name} PROPERTY CXX_STANDARD_REQUIRED ON)
    target_compile_options(${name} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
    )
    target_link_libraries(${name} Qt6::Core Qt6::Network)
 endforeach()


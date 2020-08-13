cmake_minimum_required(VERSION 3.10)

project(MBTI)

# add_library( signal STATIC mbti_signal/mbti_sig*.c )
aux_source_directory(mbti_signal SIGNAL_SRC)
aux_source_directory(mbti_ipc IPC_SRC)
add_executable(mbti.out mbti_main.c ${SIGNAL_SRC} ${IPC_SRC})

target_link_libraries(mbti.out rt)



# target_link_libraries()
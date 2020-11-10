cmake_minimum_required(VERSION 3.10)

project(MBTI)

# add_library( signal STATIC mbti_signal/mbti_sig*.c )
aux_source_directory(mbti_signal SIGNAL_SRC)
aux_source_directory(mbti_semaphore SEM_SRC)
aux_source_directory(mbti_ipc IPC_SRC)
aux_source_directory(mbti_pthread PTHREAD_SRC)


# mbti_signal에서 사용하는 define 입니다.
# __DEBUGMSG가 활성화 될 경우 DEBUG Message가 출력되며,
# USER_EXEC이 활성화 될 경우 sleep(5)가 실행되어 대기 한 뒤 다음 과정으로 넘어갑니다.
# USER_EXEC이 활성화 되지 않을 경우 특정 Signal이 Triggering 되지 않을 때 까지 대기합니다.
add_definitions(-D__DEBUGMSG -DUSER_EXEC)

add_definitions(-DUSER_EXEC -D__ESSENTIAL -std=c99 -fPIC)

add_executable(mbti.out mbti_main.c ${SIGNAL_SRC} ${SEM_SRC} ${IPC_SRC} ${PTHREAD_SRC})

target_link_libraries(mbti.out rt pthread)



# target_link_libraries()

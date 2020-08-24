#include <jni.h>
#include <string>
#include <android/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_client_1protect_MainActivity_NativeString(JNIEnv *env, jobject thiz) {
    // TODO: implement stringFromJNI()
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_client_1protect_MainActivity_PsCommand(JNIEnv *env, jobject thiz) {
    // TODO: implement ExternalCommand()
    int fd[2];
    pipe(fd);
    pid_t pid;
    char foo[4096];

    pid = fork();

    if (pid == -1)
    {
        perror("fork");
        exit(1);
    }

    if(pid == 0){
        dup2 (fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        execl("/system/bin/ps", "/system/bin/ps", "-ef", (char *)0);

    }
    else{
        close(fd[1]);
        int nbytes;
        while(0 != (nbytes = read(fd[0], foo, sizeof(foo)))) {
            __android_log_print(ANDROID_LOG_INFO,"SUKHOON","%s",foo);
            memset(foo, 0, 4096);
        }
        wait(NULL);
    }



}


/**
 *   Time-based detection
 *   : If the debugger stops the flow of execution to analyze this code execution, it takes longer than usual.
 *
 *   retur : True (longer than usual / debugger detected)
 *           false ( not detected)
 */

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_client_1protect_MainActivity_TimeCheck(JNIEnv *env, jobject thiz) {
    // TODO: implement TimeCheck()
    long start, end;
    bool detected = false;
    start = clock();

    /***
     *  Code Logic
     *
     */

    end = clock();
    if(end-start > 10000){
        std::string Detect = "Debug from Time";
        __android_log_print(ANDROID_LOG_INFO,"SUKHOON","%s",Detect.c_str());
        detected = true;
    }
    return detected;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_client_1protect_MainActivity_TracerPidCheck(JNIEnv *env, jobject thiz) {
    // TODO: implement TracerPidCheck()

}
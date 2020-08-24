#include <jni.h>
#include <string>
#include <android/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


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
 *   return : True (longer than usual / debugger detected)
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
        std::string Detect = "Detected!!!";
        __android_log_print(ANDROID_LOG_INFO,"SUKHOON","%s",Detect.c_str());
        detected = true;
    }
    return detected;
}


/**
 *  Detection via Tracer Pid
 *  : /proc/pid/status and /proc/pid/task/pid/status should have TracerPid of 0 in normal state, and PID of debugging process in debugging state.
 *
 *   return : True (longer than usual / debugger detected)
 *           false ( not detected)
 */

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_client_1protect_MainActivity_TracerPidCheck(JNIEnv *env, jobject thiz) {
    // TODO: implement TracerPidCheck()
    bool detected = false;
    std::string hello;
    std::stringstream stream;
    int pid = getpid();
    int fd;
    stream << pid;
    stream >> hello;

    hello = "/proc/" + hello + "/status";

    char* pathname = new char[30];
    strcpy(pathname,hello.c_str());

    char* buf = new char[500];
    fd = open(pathname, O_RDONLY);
    read(fd, buf, 500);

    char* c;
    char* tra = "TracerPid";
    c = strstr(buf, tra);
    char* d;
    d = strstr(c,"\n");

    int length = d-c;
    strncpy(buf,c+11,length-11);
    buf[length-11]='\0';

    std::string Detect;

    if (strcmp(buf,"0")){
        Detect = "Detected!!!";
        __android_log_print(ANDROID_LOG_INFO,"SUKHOON","%s",Detect.c_str());
        detected = true;
    }
    else{
        Detect = "Not Detected";
        __android_log_print(ANDROID_LOG_INFO,"SUKHOON","%s",Detect.c_str());
    }
    close(fd);

    return detected;
}


/**
 *  Detect via Debugging Port(23946)
 *  : check whether we are using port 23946, which seems to be commonly used for debugging
 *
 *   return : True (debuggign port active)
 *           false (debuggign port closed)
 */
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_client_1protect_MainActivity_DebuggingPortCheck(JNIEnv *env, jobject thiz) {
    // TODO: implement DebuggingPortCheck()
    int port_num = 33971;//23946; // default debugging port
    char *hostname = "localhost";

    bool port_active = false;
    int sockfd;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd <0){
        __android_log_print(ANDROID_LOG_INFO,"SUKHOON","%s","ERROR");
        exit(0);
    }

    server = gethostbyname(hostname);

    if (server == NULL){
        __android_log_print(ANDROID_LOG_INFO,"SUKHOON","%s","no such host\n");
        exit(0);
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);

    serv_addr.sin_port = htons(port_num);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        __android_log_print(ANDROID_LOG_INFO,"SUKHOON","%s","port is closed\n");
    } else {
        __android_log_print(ANDROID_LOG_INFO,"SUKHOON","%s","port is Active\n");
        port_active = true;
    }
    close(sockfd);
    return port_active;
}
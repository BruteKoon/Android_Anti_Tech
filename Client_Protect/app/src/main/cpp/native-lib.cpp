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
#include <sys/ptrace.h>
#include <elf.h>


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


/**
 *  Detection using Ptrace
 *  : each process in Linux can be run in one process at the same time, APP can preempt the pit with its own ptrace method.
 *
 *  return : True (debugger detected)
 *           false ( not detected)
 */

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_client_1protect_MainActivity_PtraceCheck(JNIEnv *env, jobject thiz) {
    // TODO: implement PtraceCheck()
    int check = ptrace(PTRACE_TRACEME,0,0,0);
    bool detected = false;

    if(check != 0){
        detected = true;
    }

    return detected;
}


/**
 * to check BreakPoint, getLibAddr Need!
 * @param lib
 * @return lib_address
 */

unsigned long getLibAddr (const char *lib) {
    puts("Enter getLibAddr");
    unsigned long addr = 0;
    char lineBuf[256];

    snprintf(lineBuf, 256 - 1, "/proc/%d/maps", getpid());
    FILE *fp = fopen(lineBuf, "r");
    if (fp == NULL) {
        perror("fopen failed");
        goto bail;
    }
    while (fgets(lineBuf, sizeof(lineBuf), fp)) {
        if (strstr(lineBuf, lib)) {
            char *temp = strtok(lineBuf, "-");
            addr = strtoul(temp, NULL, 16);
            break;
        }
    }
    bail:
    fclose(fp);
    return addr;
}

/**
 *
 *  Checking BreakPoint
 *  : The principle of the debugger is to front the breakpoint assembly instruction at the breakpoint address and back up the original instruction elsewhere. In other words, the method only needs to scan whether there is a breakpoint instruction
 *
 *  return : True (debugger detected)
 *           false ( not detected)
 */
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_client_1protect_MainActivity_BreakPointCheck(JNIEnv *env, jobject thiz) {
    // TODO: implement BreakPointCheck()
    int i, j;
    bool Detected = false;
    unsigned int base, offset, pheader;
    Elf32_Ehdr *elfhdr;
    Elf32_Phdr *ph_t;

    base = getLibAddr ("libnative.so");
    if (base == 0) {
        __android_log_print(ANDROID_LOG_INFO,"SUKHOON","%s","GetLibAddr Failed!!!\n");
        exit(0);
    }

    elfhdr = (Elf32_Ehdr *) base;
    pheader = base + elfhdr->e_phoff;

    for (i = 0; i < elfhdr->e_phnum; i++) {
        ph_t = (Elf32_Phdr*)(pheader + i * sizeof(Elf32_Phdr)); // traverse program header

        if ( !(ph_t->p_flags & 1) ) continue;
        offset = base + ph_t->p_vaddr;
        offset += sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr) * elfhdr->e_phnum;

        char *p = (char*)offset;
        for (j = 0; j < ph_t->p_memsz; j++) {
            if(*p == 0x01 && *(p+1) == 0xde) {
                __android_log_print(ANDROID_LOG_INFO,"SUKHOON","Find thumb bpt %p",p);
                Detected = true;
            } else if (*p == 0xf0 && *(p+1) == 0xf7 && *(p+2) == 0x00 && *(p+3) == 0xa0) {
                __android_log_print(ANDROID_LOG_INFO,"SUKHOON","Find thumb2 bpt %p",p);
                Detected = true;
            } else if (*p == 0x01 && *(p+1) == 0x00 && *(p+2) == 0x9f && *(p+3) == 0xef) {
                __android_log_print(ANDROID_LOG_INFO,"SUKHOON","Find arm bpt %p",p);
                Detected = true;
            }
            p++;
        }
    }
    return Detected;
}



/**
 *
 *  Checking Memrory Using String Search
 *  : /proc/self/maps contains a list of libraries and processes attached to the app's memory. Through this, check whether a specific tool is attached
 *
 *  return : True (debugger detected)
 *           false ( not detected) d
 */
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_example_client_1protect_MainActivity_MemoryCheck(JNIEnv *env, jobject thiz) {
    unsigned long addr = 0;
    char lineBuf[256];
    bool Detected = false;

    const char * tools[] = {"frida", "olley", "gdb", "xposed", "drozer", "RootCloak"};

    snprintf(lineBuf, 256 - 1, "/proc/%d/maps", getpid());
    FILE *fp = fopen(lineBuf, "r");
    if (fp == NULL) {
        perror("fopen failed");
        goto bail;
    }

    while (fgets(lineBuf, sizeof(lineBuf), fp)) {
        for (int i = 0; i < sizeof(tools)/sizeof(tools[0]); i++) {
            if (strstr(lineBuf, tools[i])) {
                Detected = true;
                break;
            }
        }

    }
    bail:
    fclose(fp);
    return Detected;
}

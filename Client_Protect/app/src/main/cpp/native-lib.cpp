#include <jni.h>
#include <string>
#include <dirent.h>
#include<sys/stat.h>
#include <android/log.h>
void getCmdLine(char path[256], char line[256]);

extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_client_1protect_MainActivity_NativeString(JNIEnv *env, jobject thiz) {
    // TODO: implement stringFromJNI()
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_client_1protect_MainActivity_NativeShowProcess(JNIEnv *env, jobject thiz) {
    DIR *dir;                       //  /proc/pid/ 를 가리킬 DIR* 변수
    struct dirent *entry;        // 각 파일의 inode를 통해 파일을 선택할 dirent 구조체
    struct stat fileStat;          // 파일의 정보를 담는 구조체
    int pid;                         // 프로세스는 /proc 디렉토리에 자신의 pid로 파일을 담아 둡니다.
    char cmdLine[256];
    char tempPath[256];
    printf("Processes Info\n");
    dir = opendir("/proc");   //  /proc이란 디렉토리 스트름이 대한 포인터가 반환되었습니다.

    while ((entry = readdir(dir)) != NULL) {   //   /proc에 존재하는 파일들을 차례대로 읽습니다.
        lstat(entry->d_name, &fileStat);          // DIR*가 가리키는 파일의 state 정보를 가져온다.
        if (!S_ISDIR(fileStat.st_mode))            // is dir? 디렉토리인지 확인한다.
            continue;                                    // 프로세스는 /proc에 자신의 pid로 디렉토리만드는 점을 안다면 이해하실거라 생각합니다.
        pid = atoi(entry->d_name);          // 프로세스(디렉토리)인것을 확인하면, 숫자로 반환한다.
        if (pid <= 0) continue;                       // 숫자가 아니라면 다시 continue;
        sprintf(tempPath, "/proc/%d/cmdline", pid); // cmdline :: 프로세스 이름이 적힌파일
        getCmdLine(tempPath, cmdLine);     // /proc/pid/cmdline에서 프로세스의 이름을
        // 가져오는 함수로 보냅니다. 아래에 정의되어있습니다.
        __android_log_print(ANDROID_LOG_DEBUG, "LOG_TAG", "\"SUKHOON [%d] %s\\n\"",pid, cmdLine);
    }

    closedir(dir);

}

void getCmdLine(char *file, char *buf) {
    FILE *srcFp;
    int i;
    srcFp = fopen(file, "r");            //   /proc/pid/cmdline에 이름이 있습니다.

    memset(buf, 0, sizeof(buf));
    fgets(buf, 256, srcFp);
    fclose(srcFp);
}



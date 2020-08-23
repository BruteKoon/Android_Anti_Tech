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

//#include <jni.h>
//#include <string>
//
//extern "C" JNIEXPORT jstring JNICALL
//Java_com_example_daa_1cp_MainActivity_stringFromJNI(
//        JNIEnv* env,
//        jobject /* this */) {
//    std::string hello = "Hello from C++";
//    return env->NewStringUTF(hello.c_str());
//}

#include <jni.h>
#include <string>
#include "lzw.h"
#include "huffmann.h"

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_daa_1cp_MainActivity_compressLZW(JNIEnv* env, jobject /* this */, jstring input) {
    const char* inputStr = env->GetStringUTFChars(input, 0);
    char* result = lzw_compress(inputStr);
    jstring outputString = env->NewStringUTF(result);
    env->ReleaseStringUTFChars(input, inputStr);
    free(result);
    return outputString;
}

// Add similar functions for Huffman and Deflate if needed
extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_daa_1cp_MainActivity_compressHuffman(JNIEnv *env, jobject thiz, jstring input) {
    const char *inputStr = env->GetStringUTFChars(input, 0);
    char* result = compressHuffman(inputStr);
    jstring resultString = env->NewStringUTF(result);
    env->ReleaseStringUTFChars(input, inputStr);
    free(result);
    return resultString;
}
extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_daa_1cp_MainActivity_compressDeflate(JNIEnv *env, jobject thiz, jstring input) {
    // TODO: implement compressDeflate()
}
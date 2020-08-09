#include <jni.h>
#include <string>
#include <android/log.h>

#define TAG "WF"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

JavaVM *jvm;
jobject instance;

extern "C" JNIEXPORT jstring JNICALL
Java_com_firewings_jnipractice_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_firewings_jnipractice_MainActivity_test(JNIEnv *env, jobject thiz, jboolean b, jbyte bt,
                                                 jchar c, jshort s, jlong l, jfloat f, jdouble d,
                                                 jstring name, jint age, jintArray arr,
                                                 jobjectArray str_arr, jobject person,
                                                 jbooleanArray b_arr) {

    //1. 接收 Java 传递过来的 boolean 值
    unsigned char b_boolean = b;
    LOGD("boolean-> %d", b_boolean);

    //2. 接收 Java 传递过来的 boolean 值
    char c_byte = bt;
    LOGD("jbyte-> %d", c_byte);


    //3. 接收 Java 传递过来的 char 值
    unsigned short c_char = c;
    LOGD("char-> %d", c_char);


    //4. 接收 Java 传递过来的 short 值
    short s_short = s;
    LOGD("short-> %d", s_short);

    //5. 接收 Java 传递过来的 long 值
    long l_long = l;
    LOGD("long-> %d", l_long);

    //6. 接收 Java 传递过来的 float 值
    float f_float = f;
    LOGD("float-> %f", f_float);

    //7. 接收 Java 传递过来的 double 值
    double d_double = d;
    LOGD("double-> %f", d_double);

    //8. 接收 Java 传递过来的 String 值
    const char *name_string = env->GetStringUTFChars(name, 0);
    LOGD("string-> %s", name_string);

    //9. 接收 Java 传递过来的 int 值
    int age_java = age;
    LOGD("int:%d", age_java);

    //10. 打印 Java 传递过来的 int []
    jint *intArray = env->GetIntArrayElements(arr, NULL);
    //拿到数组长度
    jsize intArraySize = env->GetArrayLength(arr);
    for (int i = 0; i < intArraySize; ++i) {
        LOGD("intArray->%d：", intArray[i]);
    }
    //释放数组
    env->ReleaseIntArrayElements(arr, intArray, 0);

    //11. 打印 Java 传递过来的 String[]
    jsize stringArrayLength = env->GetArrayLength(str_arr);
    for (int i = 0; i < stringArrayLength; ++i) {
        jobject jobject1 = env->GetObjectArrayElement(str_arr, i);
        //强转 JNI String
        jstring stringArrayData = static_cast<jstring >(jobject1);

        //转 C  String
        const char *itemStr = env->GetStringUTFChars(stringArrayData, NULL);
        LOGD("String[%d]: %s", i, itemStr);
        //回收 String[]
        env->ReleaseStringUTFChars(stringArrayData, itemStr);
    }



    //12. 打印 Java 传递过来的 Object 对象
    //12.1 获取字节码
    const char *person_class_str = "com/firewings/jnipractice/Person";
    //12.2 转 jni jclass
    jclass person_class = env->FindClass(person_class_str);
    //12.3 拿到方法签名 javap -a
    const char *sig = "()Ljava/lang/String;";
    jmethodID jmethodID1 = env->GetMethodID(person_class, "getName", sig);

    jobject obj_string = env->CallObjectMethod(person, jmethodID1);
    jstring perStr = static_cast<jstring >(obj_string);
    const char *itemStr2 = env->GetStringUTFChars(perStr, NULL);
    LOGD("Person: %s", itemStr2);
    env->DeleteLocalRef(person_class); // 回收
    env->DeleteLocalRef(person); // 回收


    //13. 打印 Java 传递过来的 booleanArray
    jsize booArrayLength = env->GetArrayLength(b_arr);
    jboolean *bArray = env->GetBooleanArrayElements(b_arr, NULL);
    for (int i = 0; i < booArrayLength; ++i) {
        bool b = bArray[i];
        jboolean b2 = bArray[i];
        LOGD("boolean:%d", b);
        LOGD("jboolean:%d", b2);
    }
    //回收
    env->ReleaseBooleanArrayElements(b_arr, bArray, 0);

}


extern "C"
JNIEXPORT void JNICALL
dynamicRegister(JNIEnv *env, jobject thiz, jstring s) {
    const char *str = env->GetStringUTFChars(s, NULL);
    LOGD("动态注册 %s", str);
    env->ReleaseStringChars(s, reinterpret_cast<const jchar *>(str));
}

extern "C"
JNIEXPORT void JNICALL
dynamicRegister2(JNIEnv *env, jobject thiz, jstring s) {
    const char *str = env->GetStringUTFChars(s, NULL);
    LOGD("动态注册 %s", str);

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID methodId = env->GetMethodID(clazz, "testException", "()V");
    env->CallVoidMethod(thiz, methodId);
    jthrowable exc = env->ExceptionOccurred();

    if (exc) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass newExcCls = env->FindClass("java/lang/IllegalArgumentException");
        env->ThrowNew(newExcCls, "JNI 中发生了一个异常信息");
    }

    env->ReleaseStringUTFChars(s, str);
}

jclass personClass;

extern "C"
JNIEXPORT void JNICALL
localRef(JNIEnv *env, jobject thiz) {
    LOGD("测试外部引用");
    if (personClass == NULL) {
        const char *person_class = "com/firewings/jnipractice/Person";
        // personClass = env->FindClass(person_class);
        jclass personCls = env->FindClass(person_class);
        // personClass = static_cast<jclass>(env->NewGlobalRef(personCls));
        personClass = static_cast<jclass>(env->NewWeakGlobalRef(personCls));
        LOGD("personClass == null 执行了");
    }
    const char *sig = "()V";
    const char *method = "<init>";
    jmethodID init = env->GetMethodID(personClass, method, sig);
    env->NewObject(personClass, init);

    // env->DeleteGlobalRef(personClass);
    env->DeleteWeakGlobalRef(personClass);
    personClass = NULL;
}

extern "C"
JNIEXPORT void JNICALL
nativeCount(JNIEnv *env, jobject thiz) {

    // LOGI("nativeCount");

    jclass cls = env->GetObjectClass(thiz);
    jfieldID fieldId = env->GetFieldID(cls, "count", "I");

    if (env->MonitorEnter(thiz) != JNI_OK) {
        LOGE("%s: MonitorEnter() failed", __FUNCTION__);
    }

    int val = env->GetIntField(thiz, fieldId);
    val++;
    LOGI("count=%d", val);
    env->SetIntField(thiz, fieldId, val);

    if (env->ExceptionOccurred()) {
        LOGE("ExceptionOccurred()...");
        if (env->MonitorExit(thiz) != JNI_OK) {
            LOGE("%s: MonitorExit() failed", __FUNCTION__);
        }
    }

    if (env->MonitorExit(thiz) != JNI_OK) {
        LOGE("%s: MonitorExit() failed", __FUNCTION__);
    }

}

void *customThread(void *pVoid) {
    // 调用的话，一定需要JNIEnv *env
    // JNIEnv *env 无法跨越线程，只有JavaVM才能跨越线程

    JNIEnv *env = NULL;
    int result = jvm->AttachCurrentThread(&env, 0);
    if (result != 0) {
        return 0;
    }

    jclass mainClass = env->GetObjectClass(instance);

    const char *sig = "()V";
    jmethodID updateUI = env->GetMethodID(mainClass, "updateUI", sig);
    env->CallVoidMethod(instance, updateUI);

    jvm->DetachCurrentThread();

    return 0;
}

extern "C"
JNIEXPORT void JNICALL
testThread(JNIEnv *env, jobject thiz) {
    // 全局的，就不会被释放，所以可以在线程里面用
    // 如果是非全局的，函数一结束，就被释放了
    instance = env->NewGlobalRef(thiz);
    pthread_t pthreadID;
    pthread_create(&pthreadID, 0, customThread, instance);
    pthread_join(pthreadID, 0);
}

extern "C"
JNIEXPORT void JNICALL
unThread(JNIEnv *env, jobject thiz) {

    if (NULL != thiz) {
        env->DeleteGlobalRef(instance);
        instance = NULL;
    }

}

static const JNINativeMethod jniNativeMethod[] = {
        {"dynamicRegister", ("(Ljava/lang/String;)V"), (void *) (dynamicRegister)},
        {"dynamicRegister2", ("(Ljava/lang/String;)V"), (void *) (dynamicRegister2)},
        {"localRef", ("()V"), (void *) (localRef)},
        {"nativeCount", ("()V"), (void *) (nativeCount)},
        {"testThread", ("()V"), (void *) (testThread)},
        {"unThread", ("()V"), (void *) (unThread)}
};

JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *javaVM, void *pVoid) {
    jvm = javaVM;

    JNIEnv *jniEnv = NULL;
    jint result = javaVM->GetEnv(reinterpret_cast<void **>(&jniEnv), JNI_VERSION_1_6);

    if (result != JNI_OK) {
        return JNI_ERR;
    }

    jclass activityClass = jniEnv->FindClass("com/firewings/jnipractice/MainActivity");
    jniEnv->RegisterNatives(activityClass, jniNativeMethod,
                            sizeof(jniNativeMethod) / sizeof(JNINativeMethod));

    return JNI_VERSION_1_6;
}
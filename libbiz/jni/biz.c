#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <jni.h>
#include <android/log.h>
#include "xhook.h"

static int my_system_log_print(int prio, const char* tag, const char* fmt, ...)
{
    va_list ap;
    char buf[1024];
    int r;
    
    snprintf(buf, sizeof(buf), "[%s] %s %s", (NULL == tag ? "" : tag), "system world", (NULL == fmt ? "" : fmt));

    va_start(ap, fmt);
    r = __android_log_vprint(prio, "xhook_system_furture", buf, ap);
    va_end(ap);
    return r;
}

static int my_libtest_log_print(int prio, const char* tag, const char* fmt, ...)
{
    va_list ap;
    char buf[1024];
    int r;
    
    snprintf(buf, sizeof(buf), "[%s] %s %s", (NULL == tag ? "" : tag),  "furture world", (NULL == fmt ? "" : fmt));

    va_start(ap, fmt);
    r = __android_log_vprint(prio, "xhook_libtest_furture", buf, ap);
    va_end(ap);
    return r;
}

static int(*origin_android_log_print)(int prio, const char* tag, const char* fmt, ...);

static int my_log_print(int prio, const char* tag, const char* fmt, ...)
{
    va_list ap;
    char buf[1024];
    int r;
    
    snprintf(buf, sizeof(buf), "[%s] %s %s", (NULL == tag ? "" : tag),  "furture world", (NULL == fmt ? "" : fmt));

    va_start(ap, fmt);
    r = __android_log_vprint(prio, "xhook_libtest_furture", buf, ap);
    va_end(ap);
    return r;
}


int (*origin__android_log_buf_write)(int bufID, int prio,
                                              const char* tag, const char* msg);


int my__android_log_buf_write(int bufID, int prio, const char* tag, const char* msg) {
      (void)tag;
      (void)msg;
      char buf[1024];
      snprintf(buf, sizeof(buf), "hook %s %s", (NULL == tag ? "" : tag),  msg);

     return origin__android_log_buf_write(bufID, prio, "hellohook", buf);
}


void Java_com_qiyi_biz_NativeHandler_start(JNIEnv* env, jobject obj)
{
    (void)env;
    (void)obj;

    xhook_register("^/system/.*\\.so$",  "__android_log_print", my_system_log_print,  NULL);
    xhook_register("^/vendor/.*\\.so$",  "__android_log_print", my_system_log_print,  NULL);
    xhook_register(".*/libtest\\.so$", "__android_log_print", my_libtest_log_print, NULL);

    xhook_register(".*\\.so$", "__android_log_print", my_libtest_log_print, NULL);


    //xhook_register(".*\\.so$", "__android_log_write",  my_log_print,  NULL);
    xhook_register(".*\\.so$", "__android_log_print",  my_log_print,  (void**)(&origin_android_log_print));
    xhook_register(".*\\.so$", "__android_log_buf_write",  my__android_log_buf_write,  (void**)(&origin__android_log_buf_write));
    //xhook_register(".*\\.so$", "__android_log_vprint", my_log_print, NULL);
    //xhook_register(".*\\.so$", "__android_log_assert", my_log_assert, NULL);

    xhook_register(".*/liblog\\.so$", "__android_log_print", my_libtest_log_print, NULL); 

    //just for testing
    //xhook_ignore(".*/liblog\\.so$", "__android_log_print"); //ignore __android_log_print in liblog.so
    //xhook_ignore(".*/libjavacore\\.so$", NULL); //ignore all hooks in libjavacore.so
}

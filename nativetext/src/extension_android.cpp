#if defined(DM_PLATFORM_ANDROID)

#include <android/native_window_jni.h>
#include <dmsdk/sdk.h>

#include "extension.h"
#include "android/java_lua.h"

static jobject java_extension_object = NULL;
static jmethodID java_extension_update = NULL;
static jmethodID java_extension_app_activate = NULL;
static jmethodID java_extension_app_deactivate = NULL;
static jmethodID java_extension_finalize = NULL;
static jmethodID java_extension_init = NULL;
static jmethodID java_extension_generate_text_bitmap = NULL;

static jfieldID java_extension_width = NULL;
static jfieldID java_extension_height = NULL;
static jfieldID java_extension_pixels = NULL;

static jbyteArray pixels_object = NULL;
static jbyte* pixels_buffer = NULL;

int EXTENSION_INIT(lua_State *L) {
	int result = 0;
	if (java_extension_object != NULL) {
		ThreadAttacher attacher;
		result = attacher.env->CallIntMethod(java_extension_object, java_extension_init, (jlong)L);
	}
	return result;
}

int EXTENSION_GENERATE_TEXT_BITMAP(const char *text, int font_size, const char *font_name,
	int text_width, float spacing_mult, float spacing_add, float outline_size, float shadow_size,
	float color_r, float color_g, float color_b, float color_a,
	float outline_color_r, float outline_color_g, float outline_color_b, float outline_color_a,
	float shadow_color_r, float shadow_color_g, float shadow_color_b, float shadow_color_a,
	float background_color_r, float background_color_g, float background_color_b, float background_color_a,
	int *width, int *height, void **pixels) {
	int result = 0;
	if (java_extension_object != NULL) {
		ThreadAttacher attacher;
		JNIEnv *env = attacher.env;
		jstring text_string = env->NewStringUTF(text);
		jstring font_name_string = env->NewStringUTF(font_name);
		result = env->CallIntMethod(java_extension_object, java_extension_generate_text_bitmap,
			text_string, (jint)font_size, font_name_string,
			(jint)text_width, (jfloat)spacing_mult, (jfloat)spacing_add, (jfloat)outline_size, (jfloat)shadow_size,
			(jfloat)color_r, (jfloat)color_g, (jfloat)color_b, (jfloat)color_a,
			(jfloat)outline_color_r, (jfloat)outline_color_g, (jfloat)outline_color_b, (jfloat)outline_color_a,
			(jfloat)shadow_color_r, (jfloat)shadow_color_g, (jfloat)shadow_color_b, (jfloat)shadow_color_a,
			(jfloat)background_color_r, (jfloat)background_color_g, (jfloat)background_color_b, (jfloat)background_color_a
		);
		env->DeleteLocalRef(text_string);
		env->DeleteLocalRef(font_name_string);
		*width = env->GetIntField(java_extension_object , java_extension_width);
		*height = env->GetIntField(java_extension_object , java_extension_height);
		jbyteArray pixels_object = (jbyteArray)env->GetObjectField(java_extension_object , java_extension_pixels);
		jbyte *pixels_buffer = env->GetByteArrayElements(pixels_object, NULL);
		*pixels = pixels_buffer;
	}
	return result;
}

void EXTENSION_TEXT_BITMAP_RELEASE() {
	if (pixels_object != NULL && pixels_buffer != NULL) {
		ThreadAttacher attacher;
		attacher.env->ReleaseByteArrayElements(pixels_object, pixels_buffer, JNI_ABORT);
		pixels_object = NULL;
		pixels_buffer = NULL;
	}
}

void EXTENSION_INITIALIZE(lua_State *L) {
	// Mention java_lua.h exports so they don't get optimized away.
	if (L == NULL) {
		JAVA_LUA_REGISTRYINDEX(NULL, NULL, 0);
		JAVA_LUA_GETTOP(NULL, NULL, 0);
	}
	ThreadAttacher attacher;
	JNIEnv *env = attacher.env;
	ClassLoader class_loader = ClassLoader(env);

	// Invoke Extension from the Java extension.
	jclass java_extension_class = class_loader.load("extension/" EXTENSION_NAME_STRING "/Extension");
	if (java_extension_class == NULL) {
		dmLogError("java_extension_class is NULL");
	}
	jmethodID java_extension_constructor = env->GetMethodID(java_extension_class, "<init>", "(Landroid/app/Activity;)V");
	java_extension_init = env->GetMethodID(java_extension_class, "init", "(J)I");
	java_extension_generate_text_bitmap = env->GetMethodID(java_extension_class, "generate_text_bitmap", "(Ljava/lang/String;ILjava/lang/String;IFFFFFFFFFFFFFFFFFFFF)I");
	java_extension_finalize = env->GetMethodID(java_extension_class, "extension_finalize", "(J)V");
	java_extension_update = env->GetMethodID(java_extension_class, "update", "(J)V");
	java_extension_app_activate = env->GetMethodID(java_extension_class, "app_activate", "(J)V");
	java_extension_app_deactivate = env->GetMethodID(java_extension_class, "app_deactivate", "(J)V");
	java_extension_object = (jobject)env->NewGlobalRef(env->NewObject(java_extension_class, java_extension_constructor, dmGraphics::GetNativeAndroidActivity()));
	if (java_extension_object == NULL) {
		dmLogError("java_extension_object is NULL");
	}
	java_extension_width = env->GetFieldID(java_extension_class, "width" , "I");
	java_extension_height = env->GetFieldID(java_extension_class, "height" , "I");
	java_extension_pixels = env->GetFieldID(java_extension_class, "pixels" , "[B");
}

void EXTENSION_UPDATE(lua_State *L) {
	if (java_extension_object != NULL) {
		ThreadAttacher attacher;
		// Update the Java side so it can invoke any pending listeners.
		attacher.env->CallVoidMethod(java_extension_object, java_extension_update, (jlong)L);
	}
}

void EXTENSION_APP_ACTIVATE(lua_State *L) {
	if (java_extension_object != NULL) {
		ThreadAttacher attacher;
		attacher.env->CallVoidMethod(java_extension_object, java_extension_app_activate, (jlong)L);
	}
}

void EXTENSION_APP_DEACTIVATE(lua_State *L) {
	if (java_extension_object != NULL) {
		ThreadAttacher attacher;
		attacher.env->CallVoidMethod(java_extension_object, java_extension_app_deactivate, (jlong)L);
	}
}

void EXTENSION_FINALIZE(lua_State *L) {
	ThreadAttacher attacher;
	if (java_extension_object != NULL) {
		attacher.env->CallVoidMethod(java_extension_object, java_extension_finalize, (jlong)L);
		attacher.env->DeleteGlobalRef(java_extension_object);
	}
	java_extension_object = NULL;
}

#endif

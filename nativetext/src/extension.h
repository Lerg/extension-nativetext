#ifndef extension_h
#define extension_h

// The name of the extension affects Lua module name and Java package name.
#define EXTENSION_NAME nativetext

// Convert extension name to C const string.
#define STRINGIFY(s) #s
#define STRINGIFY_EXPANDED(s) STRINGIFY(s)
#define EXTENSION_NAME_STRING STRINGIFY_EXPANDED(EXTENSION_NAME)

#include <dmsdk/sdk.h>

// Each extension must have unique exported symbols. Construct function names based on the extension name.
#define FUNCTION_NAME(extension_name, function_name) Extension_ ## extension_name ## _ ## function_name
#define FUNCTION_NAME_EXPANDED(extension_name, function_name) FUNCTION_NAME(extension_name, function_name)

#define APP_INITIALIZE FUNCTION_NAME_EXPANDED(EXTENSION_NAME, AppInitialize)
#define APP_FINALIZE FUNCTION_NAME_EXPANDED(EXTENSION_NAME, AppFinalize)
#define INITIALIZE FUNCTION_NAME_EXPANDED(EXTENSION_NAME, Initialize)
#define UPDATE FUNCTION_NAME_EXPANDED(EXTENSION_NAME, Update)
#define FINALIZE FUNCTION_NAME_EXPANDED(EXTENSION_NAME, Finalize)

// The following functions are implemented for each platform.
// Lua API.
#define EXTENSION_INIT FUNCTION_NAME_EXPANDED(EXTENSION_NAME, init)
int EXTENSION_INIT(lua_State *L);

#define EXTENSION_NEW FUNCTION_NAME_EXPANDED(EXTENSION_NAME, new)
int EXTENSION_NEW(lua_State *L);

#define EXTENSION_GENERATE_TEXT_BITMAP FUNCTION_NAME_EXPANDED(EXTENSION_NAME, generate_text_bitmap)
int EXTENSION_GENERATE_TEXT_BITMAP(const char *text, int font_size, const char *font_name,
	int text_width, int text_align, float spacing_mult, float spacing_add, float outline_size, float shadow_size, float shadow_x, float shadow_y,
	float color_r, float color_g, float color_b, float color_a,
	float outline_color_r, float outline_color_g, float outline_color_b, float outline_color_a,
	float shadow_color_r, float shadow_color_g, float shadow_color_b, float shadow_color_a,
	float background_color_r, float background_color_g, float background_color_b, float background_color_a,
	int *width, int *height, void **pixels);

#define EXTENSION_TEXT_BITMAP_RELEASE FUNCTION_NAME_EXPANDED(EXTENSION_NAME, text_bitmap_release)
void EXTENSION_TEXT_BITMAP_RELEASE();

// Extension lifecycle functions.
#define EXTENSION_INITIALIZE FUNCTION_NAME_EXPANDED(EXTENSION_NAME, initialize)
#define EXTENSION_FINALIZE FUNCTION_NAME_EXPANDED(EXTENSION_NAME, finalize)
void EXTENSION_INITIALIZE(lua_State *L);
void EXTENSION_FINALIZE(lua_State *L);

#if defined(DM_PLATFORM_ANDROID)

namespace {
	// JNI access is only valid on an attached thread.
	struct ThreadAttacher {
		JNIEnv *env;
		bool has_attached;
		ThreadAttacher() : env(NULL), has_attached(false) {
			if (dmGraphics::GetNativeAndroidJavaVM()->GetEnv((void **)&env, JNI_VERSION_1_6) != JNI_OK) {
				dmGraphics::GetNativeAndroidJavaVM()->AttachCurrentThread(&env, NULL);
				has_attached = true;
			}
		}
		~ThreadAttacher() {
			if (has_attached) {
				if (env->ExceptionCheck()) {
					env->ExceptionDescribe();
				}
				env->ExceptionClear();
				dmGraphics::GetNativeAndroidJavaVM()->DetachCurrentThread();
			}
		}
	};

	// Dynamic Java class loading.
	struct ClassLoader {
		private:
			JNIEnv *env;
			jobject class_loader_object;
			jmethodID load_class;
		public:
			ClassLoader(JNIEnv *env) : env(env) {
				jclass activity_class = env->FindClass("android/app/NativeActivity");
				jclass class_loader_class = env->FindClass("java/lang/ClassLoader");
				jmethodID get_class_loader = env->GetMethodID(activity_class, "getClassLoader", "()Ljava/lang/ClassLoader;");
				load_class = env->GetMethodID(class_loader_class, "loadClass", "(Ljava/lang/String;)Ljava/lang/Class;");
				class_loader_object = env->CallObjectMethod(dmGraphics::GetNativeAndroidActivity(), get_class_loader);
			}
			jclass load(const char *class_name) {
				jstring class_name_string = env->NewStringUTF(class_name);
				jclass loaded_class = (jclass)env->CallObjectMethod(class_loader_object, load_class, class_name_string);
				env->DeleteLocalRef(class_name_string);
				return loaded_class;
			}
	};
}

#endif
#endif

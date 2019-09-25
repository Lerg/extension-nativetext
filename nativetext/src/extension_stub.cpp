#if !defined(DM_PLATFORM_IOS) && !defined(DM_PLATFORM_OSX) && !defined(DM_PLATFORM_ANDROID)

#include <dmsdk/sdk.h>
#include "extension.h"

int EXTENSION_INIT(lua_State *L) {
	dmLogInfo("init");
	return 0;
}

int EXTENSION_GENERATE_TEXT_BITMAP(const char *text, int font_size, const char *font_name,
	int text_width, int text_align, float spacing_mult, float spacing_add, float outline_size, float shadow_size, float shadow_x, float shadow_y,
	float color_r, float color_g, float color_b, float color_a,
	float outline_color_r, float outline_color_g, float outline_color_b, float outline_color_a,
	float shadow_color_r, float shadow_color_g, float shadow_color_b, float shadow_color_a,
	float background_color_r, float background_color_g, float background_color_b, float background_color_a,
	int *width, int *height, void **pixels) {
	dmLogInfo("generate_text_bitmap");
	return 0;
}

void EXTENSION_TEXT_BITMAP_RELEASE() {
	dmLogInfo("text_bitmap_release");
}

void EXTENSION_INITIALIZE(lua_State *L) {
}

void EXTENSION_FINALIZE(lua_State *L) {
}

#endif

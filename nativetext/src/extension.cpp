#include <string.h>
#include "extension.h"

// This is the entry point of the extension. It defines Lua API of the extension.

static const luaL_reg lua_functions[] = {
	{"init", EXTENSION_INIT},
	{"new", EXTENSION_NEW},
	{0, 0}
};

// Push loaded data as a Lua table onto the stack.
static void push_image_resource(lua_State *L, int width, int height, dmBuffer::HBuffer buffer) {
	// Image resource.
	lua_newtable(L);

	// Image resource -> Header.
	lua_newtable(L);

	lua_pushnumber(L, width);
	lua_setfield(L, -2, "width");

	lua_pushnumber(L, height);
	lua_setfield(L, -2, "height");

	lua_pushnumber(L, 4);
	lua_setfield(L, -2, "channels");

	lua_pushnumber(L, 1);
	lua_setfield(L, -2, "num_mip_maps");

	lua_getglobal(L, "resource");
	lua_getfield(L, -1, "TEXTURE_TYPE_2D");
	lua_setfield(L, -3, "type");

	lua_getfield(L, -1, "TEXTURE_FORMAT_RGBA");
	lua_setfield(L, -3, "format");

	lua_pop(L, 1); // resource
	
	lua_setfield(L, -2, "header");

	if (buffer != 0) {
		dmScript::LuaHBuffer lua_buffer(buffer, dmScript::OWNER_LUA);
		dmScript::PushBuffer(L, lua_buffer);
		lua_setfield(L, -2, "buffer");
	}
}

int EXTENSION_NEW(lua_State *L) {
	DM_LUA_STACK_CHECK(L, 1);
	int params_index = 1;
	if (!lua_istable(L, params_index)) {
		return DM_LUA_ERROR("The argument must be params table.");
	}

	const char *text = NULL;
	lua_getfield(L, params_index, "text");
	if (lua_isstring(L, -1)) {
		text = lua_tostring(L, -1);
	} else {
		lua_pop(L, 1);
		return DM_LUA_ERROR("Text param must be set.");
	}

	int font_size = 10;
	lua_getfield(L, params_index, "size");
	if (lua_isnumber(L, -1)) {
		font_size = lua_tonumber(L, -1);
	}
	lua_pop(L, 1);

	const char *font_name = NULL;
	lua_getfield(L, params_index, "font");
	if (lua_isstring(L, -1)) {
		font_name = lua_tostring(L, -1);
	}
	lua_pop(L, 1);

	int text_width = 0;
	lua_getfield(L, params_index, "width");
	if (lua_isnumber(L, -1)) {
		text_width = lua_tonumber(L, -1);
	}
	lua_pop(L, 1);

	int text_align = 0;
	lua_getfield(L, params_index, "align");
	if (lua_isnumber(L, -1)) {
		text_align = lua_tonumber(L, -1);
	}
	lua_pop(L, 1);

	bool underline = false;
	lua_getfield(L, params_index, "underline");
	if (lua_isboolean(L, -1)) {
		underline = lua_toboolean(L, -1);
	}
	lua_pop(L, 1);

	float spacing_mult = 1;
	lua_getfield(L, params_index, "spacing_mult");
	if (lua_isnumber(L, -1)) {
		spacing_mult = lua_tonumber(L, -1);
	}
	lua_pop(L, 1);

	float spacing_add = 0;
	lua_getfield(L, params_index, "spacing_add");
	if (lua_isnumber(L, -1)) {
		spacing_add = lua_tonumber(L, -1);
	}
	lua_pop(L, 1);

	float outline_size = 0;
	lua_getfield(L, params_index, "outline_size");
	if (lua_isnumber(L, -1)) {
		outline_size = lua_tonumber(L, -1);
	}
	lua_pop(L, 1);

	float shadow_size = 0;
	lua_getfield(L, params_index, "shadow_size");
	if (lua_isnumber(L, -1)) {
		shadow_size = lua_tonumber(L, -1);
	}
	lua_pop(L, 1);

	float shadow_x = 0;
	lua_getfield(L, params_index, "shadow_x");
	if (lua_isnumber(L, -1)) {
		shadow_x = lua_tonumber(L, -1);
	}
	lua_pop(L, 1);

	float shadow_y = 0;
	lua_getfield(L, params_index, "shadow_y");
	if (lua_isnumber(L, -1)) {
		shadow_y = lua_tonumber(L, -1);
	}
	lua_pop(L, 1);

	float color_r = 1, color_g = 1, color_b = 1, color_a = 1;
	lua_getfield(L, params_index, "color");
	if (lua_isuserdata(L, -1)) {
		lua_getfield(L, -1, "x");
		color_r = lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "y");
		color_g = lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "z");
		color_b = lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "w");
		color_a = lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	float outline_color_r = 0, outline_color_g = 0, outline_color_b = 0, outline_color_a = 0;
	lua_getfield(L, params_index, "outline_color");
	if (lua_isuserdata(L, -1)) {
		lua_getfield(L, -1, "x");
		outline_color_r = lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "y");
		outline_color_g = lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "z");
		outline_color_b = lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "w");
		outline_color_a = lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	float shadow_color_r = 0, shadow_color_g = 0, shadow_color_b = 0, shadow_color_a = 0;
	lua_getfield(L, params_index, "shadow_color");
	if (lua_isuserdata(L, -1)) {
		lua_getfield(L, -1, "x");
		shadow_color_r = lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "y");
		shadow_color_g = lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "z");
		shadow_color_b = lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "w");
		shadow_color_a = lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	float background_color_r = 0, background_color_g = 0, background_color_b = 0, background_color_a = 0;
	lua_getfield(L, params_index, "background_color");
	if (lua_isuserdata(L, -1)) {
		lua_getfield(L, -1, "x");
		background_color_r = lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "y");
		background_color_g = lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "z");
		background_color_b = lua_tonumber(L, -1);
		lua_pop(L, 1);

		lua_getfield(L, -1, "w");
		background_color_a = lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
	lua_pop(L, 1);

	lua_pop(L, 1); // params table.

	int width = 0;
	int height = 0;
	void *pixels = NULL;
	int status = EXTENSION_GENERATE_TEXT_BITMAP(text, font_size, font_name,
		text_width, text_align, underline, spacing_mult, spacing_add, outline_size, shadow_size, shadow_x, shadow_y,
		color_r, color_g, color_b, color_a,
		outline_color_r, outline_color_g, outline_color_b, outline_color_a,
		shadow_color_r, shadow_color_g, shadow_color_b, shadow_color_a,
		background_color_r, background_color_g, background_color_b, background_color_a,
		&width, &height, &pixels);
	if (status == 1) {
		dmBuffer::HBuffer buffer = 0;
		dmBuffer::StreamDeclaration streams_declaration[] = {
			{dmHashString64("pixels"), dmBuffer::VALUE_TYPE_UINT8, 4}
		};
		dmBuffer::Result result = dmBuffer::Create(width * height, streams_declaration, 1, &buffer);
		if (result == dmBuffer::RESULT_OK) {
			uint8_t *data = NULL;
			uint32_t data_size = 0;
			dmBuffer::GetBytes(buffer, (void **)&data, &data_size);
			memcpy(data, pixels, data_size);
			if (dmBuffer::ValidateBuffer(buffer) != dmBuffer::RESULT_OK) {
				dmBuffer::Destroy(buffer);
				buffer = 0;
			}
		} else {
			buffer = 0;
		}
		if (buffer != 0) {
			push_image_resource(L, width, height, buffer);
		} else {
			lua_pushnil(L);
		}
	} else {
		lua_pushnil(L);
	}
	EXTENSION_TEXT_BITMAP_RELEASE();
	return 1;
}

dmExtension::Result APP_INITIALIZE(dmExtension::AppParams *params) {
	return dmExtension::RESULT_OK;
}

dmExtension::Result APP_FINALIZE(dmExtension::AppParams *params) {
	return dmExtension::RESULT_OK;
}

dmExtension::Result INITIALIZE(dmExtension::Params *params) {
	lua_State *L = params->m_L;
	luaL_register(L, EXTENSION_NAME_STRING, lua_functions);

	lua_pushnumber(L, 0);
	lua_setfield(L, -2, "align_normal");

	lua_pushnumber(L, 1);
	lua_setfield(L, -2, "align_left");

	lua_pushnumber(L, 2);
	lua_setfield(L, -2, "align_right");

	lua_pushnumber(L, 3);
	lua_setfield(L, -2, "align_center");

	lua_pushnumber(L, 4);
	lua_setfield(L, -2, "align_justified");

	lua_pop(L, 1);
	EXTENSION_INITIALIZE(L);
	return dmExtension::RESULT_OK;
}

dmExtension::Result UPDATE(dmExtension::Params *params) {
	return dmExtension::RESULT_OK;
}

void EXTENSION_ON_EVENT(dmExtension::Params *params, const dmExtension::Event *event) {
}

dmExtension::Result FINALIZE(dmExtension::Params *params) {
	EXTENSION_FINALIZE(params->m_L);
	return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(EXTENSION_NAME, EXTENSION_NAME_STRING, APP_INITIALIZE, APP_FINALIZE, INITIALIZE, UPDATE, EXTENSION_ON_EVENT, FINALIZE)
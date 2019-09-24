#if defined(DM_PLATFORM_IOS) || defined(DM_PLATFORM_OSX)

#if defined(DM_PLATFORM_OSX)
	#import <AppKit/AppKit.h>
#else
	#import <UIKit/UIKit.h>
#endif

#include <dmsdk/sdk.h>
#include "extension.h"

#if defined(DM_PLATFORM_OSX)
	static NSColor *rgba(float r, float g, float b, float a) {
		return [NSColor colorWithRed:r green:g blue:b alpha:a];
	}
#else
	static UIColor *rgba(float r, float g, float b, float a) {
		return [UIColor colorWithRed:r green:g blue:b alpha:a];
	}
#endif

static NSBitmapImageRep *bitmap = nil;

int EXTENSION_INIT(lua_State *L) {
	return 0;
}

int EXTENSION_GENERATE_TEXT_BITMAP(const char *text, int font_size, const char *font_name,
	int text_width, float spacing_mult, float spacing_add, float outline_size, float shadow_size,
	float color_r, float color_g, float color_b, float color_a,
	float outline_color_r, float outline_color_g, float outline_color_b, float outline_color_a,
	float shadow_color_r, float shadow_color_g, float shadow_color_b, float shadow_color_a,
	float background_color_r, float background_color_g, float background_color_b, float background_color_a,
	int *width, int *height, void **pixels) {
	NSString *text_string = [NSString stringWithUTF8String:text];
	NSMutableDictionary *attributes = [NSMutableDictionary dictionaryWithCapacity:4];
	NSMutableDictionary *effect_attributes = nil;
	attributes[NSForegroundColorAttributeName] = rgba(color_r, color_g, color_b, color_a);
	if (outline_color_a > 0.0f && outline_size > 0.0f) {
		effect_attributes = [NSMutableDictionary dictionaryWithCapacity:4];
		effect_attributes[NSStrokeColorAttributeName] = rgba(outline_color_r, outline_color_g, outline_color_b, outline_color_a);
		effect_attributes[NSStrokeWidthAttributeName] = @(2.0 * outline_size); // Minus indicates both stroking and filling.
	}
	if (shadow_color_a > 0.0f && shadow_size > 0.0f) {
		if (effect_attributes == nil) {
			effect_attributes = [NSMutableDictionary dictionaryWithCapacity:2];
		}
		NSShadow *shadow = [[NSShadow alloc] init];
		shadow.shadowColor = rgba(shadow_color_r, shadow_color_g, shadow_color_b, shadow_color_a);
		shadow.shadowBlurRadius = shadow_size;
		effect_attributes[NSShadowAttributeName] = shadow;
	}
	#if defined(DM_PLATFORM_OSX)
		attributes[NSFontAttributeName] = [NSFont systemFontOfSize:font_size];
	#else
		attributes[NSFontAttributeName] = [UIFont systemFontOfSize:font_size];
	#endif
	CGSize text_size;
	if (effect_attributes != nil) {
		effect_attributes[NSFontAttributeName] = attributes[NSFontAttributeName];
		text_size = [text_string sizeWithAttributes:effect_attributes];
	} else {
		text_size = [text_string sizeWithAttributes:attributes];
	}
	*width = ceil(text_size.width);
	*height = ceil(text_size.height);

	bitmap = [[NSBitmapImageRep alloc]
		initWithBitmapDataPlanes:NULL
		pixelsWide:*width
		pixelsHigh:*height
		bitsPerSample:8
		samplesPerPixel:4
		hasAlpha:YES
		isPlanar:NO
		colorSpaceName:NSDeviceRGBColorSpace
		bytesPerRow:(4 * *width)
		bitsPerPixel:32];
	NSGraphicsContext *ctx = [NSGraphicsContext graphicsContextWithBitmapImageRep:bitmap];
	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:ctx];
	[ctx setShouldAntialias:YES];
	if (background_color_a > 0.0f) {
		[rgba(background_color_r, background_color_g, background_color_b, background_color_a) drawSwatchInRect:NSMakeRect(0.0f, 0.0f, *width, *height)];
	}
	NSAffineTransform *flip_transform = [NSAffineTransform transform];
	[flip_transform translateXBy:0.0 yBy:*height];
	[flip_transform scaleXBy:1.0 yBy:-1.0];
	[flip_transform concat];
	if (effect_attributes != nil) {
		[text_string drawAtPoint:NSZeroPoint withAttributes:effect_attributes];
	}
	[text_string drawAtPoint:NSZeroPoint withAttributes:attributes];
	[ctx flushGraphics];
	[NSGraphicsContext restoreGraphicsState];
	*pixels = bitmap.bitmapData;
	return 1;
}

void EXTENSION_TEXT_BITMAP_RELEASE() {
	if (bitmap != nil) {
		bitmap = nil;
	}
}

void EXTENSION_INITIALIZE(lua_State *L) {
}

void EXTENSION_FINALIZE(lua_State *L) {
}

#endif

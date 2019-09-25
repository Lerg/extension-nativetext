#if defined(DM_PLATFORM_IOS) || defined(DM_PLATFORM_OSX)

#if defined(DM_PLATFORM_OSX)
	#import <AppKit/AppKit.h>
#else
	#import <UIKit/UIKit.h>
#endif

#include <dmsdk/sdk.h>
#include "extension.h"

#import "apple/utils.h"

#if defined(DM_PLATFORM_OSX)
	static NSColor *rgba(float r, float g, float b, float a) {
		return [NSColor colorWithRed:r green:g blue:b alpha:a];
	}
	static NSBitmapImageRep *bitmap = nil;
	#define AppleRect NSRect
	#define AppleMakeRect NSMakeRect
	#define AppleSize NSSize
	#define AppleMakeSize NSMakeSize
	#define AppleZeroPoint NSZeroPoint
#else
	static UIColor *rgba(float r, float g, float b, float a) {
		return [UIColor colorWithRed:r green:g blue:b alpha:a];
	}
	static CGContextRef bitmap = NULL;
	#define AppleRect CGRect
	#define AppleMakeRect CGRectMake
	#define AppleSize CGSize
	#define AppleMakeSize CGSizeMake
	#define AppleZeroPoint CGPointMake(0, 0)
#endif

static int max_texture_width = 1024, max_texture_height = 1024;

int EXTENSION_INIT(lua_State *L) {
	[Utils check_arg_count:L count:1];
	Scheme *scheme = [[Scheme alloc] init];
	[scheme number:@"max_texture_width"];
	[scheme number:@"max_texture_height"];

	Table *params = [[Table alloc] init:L index:1];
	[params parse:scheme];
	max_texture_width = [params get_integer:@"max_texture_width" default:max_texture_width];
	max_texture_height = [params get_integer:@"max_texture_height" default:max_texture_height];
	return 0;
}

int EXTENSION_GENERATE_TEXT_BITMAP(const char *text, int font_size, const char *font_name,
	int text_width, int text_align, float spacing_mult, float spacing_add, float outline_size, float shadow_size, float shadow_x, float shadow_y,
	float color_r, float color_g, float color_b, float color_a,
	float outline_color_r, float outline_color_g, float outline_color_b, float outline_color_a,
	float shadow_color_r, float shadow_color_g, float shadow_color_b, float shadow_color_a,
	float background_color_r, float background_color_g, float background_color_b, float background_color_a,
	int *width, int *height, void **pixels) {
	NSString *text_string = [NSString stringWithUTF8String:text];
	NSMutableDictionary *attributes = [NSMutableDictionary dictionaryWithCapacity:4];
	NSMutableDictionary *effect_attributes = nil;
	attributes[NSForegroundColorAttributeName] = rgba(color_r, color_g, color_b, color_a);
	NSMutableParagraphStyle *paragraph_style = NSParagraphStyle.defaultParagraphStyle.mutableCopy;
	switch (text_align) {
		case 1:
			paragraph_style.alignment = NSTextAlignmentLeft;
			break;
		case 2:
			paragraph_style.alignment = NSTextAlignmentRight;
			break;
		case 3:
			paragraph_style.alignment = NSTextAlignmentCenter;
			break;
		case 4:
			paragraph_style.alignment = NSTextAlignmentJustified;
			break;
		default:
			paragraph_style.alignment = NSTextAlignmentNatural;
	}
	paragraph_style.lineSpacing = spacing_mult;
	attributes[NSParagraphStyleAttributeName] = paragraph_style;
	if (outline_color_a > 0.0f && outline_size > 0.0f) {
		effect_attributes = [NSMutableDictionary dictionaryWithCapacity:4];
		effect_attributes[NSStrokeColorAttributeName] = rgba(outline_color_r, outline_color_g, outline_color_b, outline_color_a);
		effect_attributes[NSStrokeWidthAttributeName] = @(2.0 * outline_size);
	}
	if (shadow_color_a > 0.0f && shadow_size > 0.0f) {
		if (effect_attributes == nil) {
			effect_attributes = [NSMutableDictionary dictionaryWithCapacity:2];
		}
		NSShadow *shadow = [[NSShadow alloc] init];
		shadow.shadowBlurRadius = shadow_size;
		shadow.shadowOffset = AppleMakeSize(shadow_x, -shadow_y);
		shadow.shadowColor = rgba(shadow_color_r, shadow_color_g, shadow_color_b, shadow_color_a);
		effect_attributes[NSShadowAttributeName] = shadow;
	}
	#if defined(DM_PLATFORM_OSX)
		attributes[NSFontAttributeName] = [NSFont systemFontOfSize:font_size];
	#else
		attributes[NSFontAttributeName] = [UIFont systemFontOfSize:font_size];
	#endif
	CGSize text_size;
	NSMutableDictionary *size_attributes = attributes;
	if (effect_attributes != nil) {
		effect_attributes[NSFontAttributeName] = attributes[NSFontAttributeName];
		effect_attributes[NSParagraphStyleAttributeName] = attributes[NSParagraphStyleAttributeName];
		size_attributes = effect_attributes;
	}
	bool is_rect = false;
	if (text_width == 0) {
		text_size = [text_string sizeWithAttributes:size_attributes];
		text_size.width = MIN(text_size.width, max_texture_width);
	} else {
		is_rect = true;
		NSStringDrawingContext *drawing_context = [[NSStringDrawingContext alloc] init];
		text_size = [text_string boundingRectWithSize:AppleMakeSize(MIN(text_width, max_texture_width), max_texture_height)
			options:NSStringDrawingUsesLineFragmentOrigin
			attributes:size_attributes
			context:drawing_context
			].size;
	}
	float padding_left = MAX(shadow_size - shadow_x, outline_size);
	float padding_right = MAX(shadow_size + shadow_x, outline_size);
	float padding_top = MAX(shadow_size - shadow_y, outline_size);
	float padding_bottom = MAX(shadow_size + shadow_y, outline_size);
	*width = ceil(text_size.width + padding_left + padding_right);
	*height = ceil(text_size.height + padding_top + padding_bottom);
	AppleRect draw_rect;
	if (is_rect) {
		draw_rect = AppleMakeRect(0, padding_bottom, text_size.width, text_size.height); // Why only padding_bottom?
	}
	#if defined(DM_PLATFORM_OSX)
		bitmap = [[NSBitmapImageRep alloc]
			initWithBitmapDataPlanes:NULL
			pixelsWide:*width
			pixelsHigh:*height
			bitsPerSample:8
			samplesPerPixel:4
			hasAlpha:YES
			isPlanar:NO
			colorSpaceName:NSDeviceRGBColorSpace
			bytesPerRow:4 * *width
			bitsPerPixel:32];
		NSGraphicsContext *ctx = [NSGraphicsContext graphicsContextWithBitmapImageRep:bitmap];
		[NSGraphicsContext saveGraphicsState];
		[NSGraphicsContext setCurrentContext:ctx];
		[ctx setShouldAntialias:YES];
	#else
		CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();
		bitmap = CGBitmapContextCreate(NULL, *width, *height, 8, 4 * *width, color_space, kCGImageAlphaPremultipliedLast);
		UIGraphicsPushContext(bitmap);
	#endif
	if (background_color_a > 0.0f) {
		#if defined(DM_PLATFORM_OSX)
			[rgba(background_color_r, background_color_g, background_color_b, background_color_a) drawSwatchInRect:AppleMakeRect(0.0f, 0.0f, *width, *height)];
		#else
			[rgba(background_color_r, background_color_g, background_color_b, background_color_a) setFill];
			UIRectFill(AppleMakeRect(0.0f, 0.0f, *width, *height));
		#endif
	}
	#if defined(DM_PLATFORM_OSX)
		NSAffineTransform *flip_transform = [NSAffineTransform transform];
		[flip_transform translateXBy:padding_left yBy:*height - padding_bottom];
		[flip_transform scaleXBy:1.0 yBy:-1.0];
		[flip_transform concat];
	#else
		CGContextTranslateCTM(bitmap, padding_left, 0.0);
	#endif
	if (effect_attributes != nil) {
		if (is_rect) {
			[text_string drawInRect:draw_rect withAttributes:effect_attributes];
		} else {
			[text_string drawAtPoint:AppleZeroPoint withAttributes:effect_attributes];
		}
	}
	if (is_rect) {
		[text_string drawInRect:draw_rect withAttributes:attributes];
	} else {
		[text_string drawAtPoint:AppleZeroPoint withAttributes:attributes];
	}
	#if defined(DM_PLATFORM_OSX)
		[ctx flushGraphics];
		[NSGraphicsContext restoreGraphicsState];
		*pixels = bitmap.bitmapData;
	#else
		*pixels = CGBitmapContextGetData(bitmap);
		UIGraphicsPopContext();
		CGColorSpaceRelease(color_space);
	#endif
	return 1;
}

void EXTENSION_TEXT_BITMAP_RELEASE() {
	#if defined(DM_PLATFORM_OSX)
		if (bitmap != nil) {
			bitmap = nil;
		}
	#else
		if (bitmap != NULL) {
			CGContextRelease(bitmap);
			bitmap = NULL;
		}
	#endif
}

void EXTENSION_INITIALIZE(lua_State *L) {
}

void EXTENSION_FINALIZE(lua_State *L) {
}

#endif

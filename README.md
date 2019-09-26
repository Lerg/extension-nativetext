# extension-nativetext

Native Text Rendering extension for Defold.

This extension renders text into an image resource, that can be applied as a texture.

Supported platforms: macOS, iOS, Android.

# Project Settings

Add this URL to the dependencies section in `game.project`.

- `https://github.com/Lerg/extension-nativetext/archive/master.zip`

# API reference

## nativetext.init(params)

Optionally call this function to set maximum texture size to be allowed to generate.

### `params` reference

- `max_texture_width`, number. Maximum texture width.
- `max_texture_height`, number. Maximum texture height.

## nativetext.new(params)

Generate an image resource with rendered text.

### `params` reference

- `text`, string. Text.
- `width`, number. Maximum width to fit the text in. Height is restricted by maximum texture height. Optional.
- `size`, number. Font size. Optional.
- `spacing_add`, number. Add distance between lines. Optional.
- `align`, number. Text alignment. See below. Optional.
- `color`, vector4. Text color. Optional.
- `outline_size`, number. Outline size. Optional.
- `outline_color`, vector4. Outline color. Optional.
- `shadow_size`, number. Shadow size. Optional.
- `shadow_x`, number. Shadow x offset. Optional.
- `shadow_y`, number. Shadow y offset. Optional.
- `shadow_color`, vector4. Shadow color. Optional.
- `background_color`, vector4. Background color. Optional.

## Text alignment

### nativetext.align_normal
Normal alignment.

### nativetext.align_left
Left alignment.

### nativetext.align_right
Right alignment.

### nativetext.align_center
Center alignment.

### nativetext.align_justified
Justified alignment. iOS/macOS only.

### Syntax

```language-lua
-- Set maximum allowed texture size.
nativetext.init{
	max_texture_width = 2048,
	max_texture_height = 2048
}

-- Generate text image.
local image_resource = nativetext.new{
	text = 'This is a test ど러연关\nыыыьщюш\nตตต ตัตัตั ตั้ตั้ตั้',
	width = 250,
	size = 30,
	spacing_add = 50,
	align = nativetext.align_center,
	color = vmath.vector4(0, 1, 1, 1),
	outline_size = 8,
	outline_color = vmath.vector4(0.9, 0.1, 0.1, 1),
	shadow_size = 8,
	shadow_x = 8, shadow_y = -8,
	shadow_color = vmath.vector4(0, 0, 0, 1),
	background_color = vmath.vector4(0.1, 0.2, 0.8, 1)
}

-- Resize game object to the image size.
go.set_scale(vmath.vector3(image_resource.header.width, image_resource.header.height, 1))

-- Apply image to a model.
resource.set_texture(go.get('#model', 'texture0'), image_resource.header, image_resource.buffer)
```
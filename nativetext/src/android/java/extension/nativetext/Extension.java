package extension.nativetext;

import java.nio.ByteBuffer;
import java.util.Hashtable;
import java.util.Map;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Typeface;
import android.text.Layout;
import android.text.StaticLayout;
import android.text.TextPaint;

import extension.nativetext.Utils.Scheme;
import extension.nativetext.Utils.Table;

@SuppressWarnings("unused")
public class Extension {
	private int width = 0;
	private int height = 0;
	private byte[] pixels = null;
	private int max_texture_width = 1024;
	private int max_texture_height = 1024;

	@SuppressWarnings("unused")
	public Extension(android.app.Activity main_activity) {
		Utils.set_tag("nativetext");
	}

	@SuppressWarnings("unused")
	public void extension_finalize(long L) {
	}

	private int init(long L) {
		Utils.check_arg_count(L, 1);
		Scheme scheme = new Scheme()
			.number("max_texture_width")
			.number("max_texture_height");
		Table params = new Table(L, 1).parse(scheme);
		max_texture_width = params.get_integer("max_texture_width", max_texture_width);
		max_texture_height = params.get_integer("max_texture_height", max_texture_height);
		return 0;
	}

	private static int argb(float a, float r, float g, float b) {
		return Color.argb((int)(a * 255), (int)(r * 255), (int)(g * 255), (int)(b * 255));
	}

	private int generate_text_bitmap(String text, int font_size, String font_name,
		int text_width, int text_align, boolean underline, float spacing_mult, float spacing_add, float outline_size, float shadow_size, float shadow_x, float shadow_y,
		float color_r, float color_g, float color_b, float color_a,
		float outline_color_r, float outline_color_g, float outline_color_b, float outline_color_a,
		float shadow_color_r, float shadow_color_g, float shadow_color_b, float shadow_color_a,
		float background_color_r, float background_color_g, float background_color_b, float background_color_a
		) {
		TextPaint text_paint = new TextPaint(Paint.ANTI_ALIAS_FLAG | Paint.LINEAR_TEXT_FLAG);
		text_paint.setTextSize(font_size);

		if (underline) {
			text_paint.setFlags(Paint.UNDERLINE_TEXT_FLAG);
		}

		if (outline_color_a > 0 && outline_size > 0) {
			text_paint.setStyle(Paint.Style.STROKE);
			text_paint.setStrokeWidth(outline_size);
			text_paint.setColor(argb(outline_color_a, outline_color_r, outline_color_g, outline_color_b));
		}

		if (font_name != null) {
			text_paint.setTypeface(Typeface.create(font_name, Typeface.NORMAL));
		}

		if (shadow_color_a > 0 && shadow_size > 0) {
			text_paint.setShadowLayer(shadow_size, shadow_x, -shadow_y, argb(shadow_color_a, shadow_color_r, shadow_color_g, shadow_color_b));
		}

		if (text_width == 0) {
			text_width = (int)Math.ceil(text_paint.measureText(text));
		}
		Layout.Alignment alignment = Layout.Alignment.ALIGN_NORMAL;
		switch (text_align) {
			case 2:
				alignment = Layout.Alignment.ALIGN_OPPOSITE;
				break;
			case 3:
				alignment = Layout.Alignment.ALIGN_CENTER;
				break;
		}
		StaticLayout text_layout = new StaticLayout(text, text_paint, text_width, alignment, spacing_mult, spacing_add, false);
		float padding_left = Math.max(shadow_size - shadow_x, outline_size);
		float padding_right = Math.max(shadow_size + shadow_x, outline_size);
		float padding_top = Math.max(shadow_size - shadow_y, outline_size);
		float padding_bottom = Math.max(shadow_size + shadow_y, outline_size);
		width = (int)Math.ceil(text_layout.getWidth() + padding_left + padding_right);
		height = (int)Math.ceil(text_layout.getHeight() + padding_top + padding_bottom);
		Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
		Canvas canvas = new Canvas(bitmap);
		canvas.scale(1, -1, width / 2, height / 2);

		if (background_color_a > 0) {
			Paint background_paint = new Paint(Paint.ANTI_ALIAS_FLAG | Paint.LINEAR_TEXT_FLAG);
			background_paint.setStyle(Paint.Style.FILL);
			background_paint.setColor(argb(background_color_a, background_color_r, background_color_g, background_color_b));
			canvas.drawPaint(background_paint);
		}

		if (outline_color_a > 0 && outline_size > 0) {
			text_layout.draw(canvas);
		}

		text_paint.setStyle(Paint.Style.FILL);
		text_paint.setStrokeWidth(0);
		text_paint.setColor(argb(color_a, color_r, color_g, color_b));
		text_layout.draw(canvas);

		ByteBuffer byte_buffer = ByteBuffer.allocate(4 * width * height);
		bitmap.copyPixelsToBuffer(byte_buffer);
		pixels = byte_buffer.array();
		return 1;
	}
}

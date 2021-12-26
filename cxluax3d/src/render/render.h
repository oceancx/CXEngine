#pragma once

namespace cxluax3d
{
	void render_init();

	void render_deinit();

	void render_update();

	void render_begin(int w, int h);

	void render_end();

	void render_on_frame_buffer_size_changed(int w, int h);

}
 
/**************************************************************************/
/*  movie_writer_ogv.h                                                    */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#ifndef MOVIE_WRITER_OGV_H
#define MOVIE_WRITER_OGV_H

#include "servers/audio_server.h"
#include "servers/movie_writer/movie_writer.h"

#include <theora/theoraenc.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>

class MovieWriterOGV : public MovieWriter {
	GDCLASS(MovieWriterOGV, MovieWriter)

	uint32_t mix_rate = 48000;
	AudioServer::SpeakerMode speaker_mode = AudioServer::SPEAKER_MODE_STEREO;
	String base_path;
	uint32_t frame_count = 0;
	uint32_t fps = 0;
	uint32_t audio_ch = 0;
	uint32_t audio_frames = 0;

	Ref<FileAccess> f;

	// Bitrate target for Vorbis audio
	int audio_r = 0;

	// Vorbis quality -0.1 to 1 (-0.1 yields smallest files but lowest fidelity; 1 yields highest fidelity but large files. '0.2' is a reasonable default)
	float audio_q = 0.2;

	// VP3 strict compatibility
	int vp3_compatible = 0;

	// Bitrate target for Theora video
	int video_r = 0;

	// Theora quality selector from 0 to 1.0 (0 yields smallest files but lowest video quality. 1.0 yields highest fidelity but large files)
	float video_q = 0.75;

	// Streaming video
	ogg_uint32_t keyframe_frequency = 64;

	// Buffer delay (in frames). Longer delays allow smoother rate adaptation and provide better overall quality, but require more
	// client side buffering and add latency. The default value is the keyframe interval for one-pass encoding (or somewhat larger if
	// soft-target is used)
	int buf_delay = -1;

	// Sets the encoder speed level. Higher speed levels favor quicker encoding over better quality per bit. Depending on the encoding
	// mode, and the internal algorithms used, quality may actually improve with higher speeds, but in this case bitrate will also
	// likely increase. The maximum value, and the meaning of each value, are implementation-specific and may change depending on the
	// current encoding mode
	int speed = 4;

	/* Use a large reservoir and treat the rate as a soft target; rate control is less strict but resulting quality is usually
	higher/smoother overall. Soft target also allows an optional setting to specify a minimum allowed quality. */
	int soft_target = 1;

	// Take physical pages, weld into a logical stream of packets
	ogg_stream_state to;

	// Take physical pages, weld into a logical stream of packets
	ogg_stream_state vo;

	// Theora encoding context
	th_enc_ctx *td;

	// Theora bitstream information
	th_info ti;

	// Theora comment information
	th_comment tc;

	// Vorbis bitstream information
	vorbis_info vi;

	// Vorbis comment information
	vorbis_comment vc;

	// central working state for the packet->PCM decoder
	vorbis_dsp_state vd;

	// local working space for packet->PCM decode
	vorbis_block vb;

	// Video buffer
	uint8_t *y, *u, *v;
	th_ycbcr_buffer ycbcr;

	ogg_page audiopage;
	ogg_page videopage;

	int encode_audio(const int32_t *p_audio_data);
	int encode_video(const Ref<Image> &p_image);

	inline int ilog(unsigned _v) {
		int ret;
		for (ret = 0; _v; ret++)
			_v >>= 1;
		return ret;
	}

protected:
	virtual uint32_t get_audio_mix_rate() const override;
	virtual AudioServer::SpeakerMode get_audio_speaker_mode() const override;
	virtual void get_supported_extensions(List<String> *r_extensions) const override;

	virtual Error write_begin(const Size2i &p_movie_size, uint32_t p_fps, const String &p_base_path) override;
	virtual Error write_frame(const Ref<Image> &p_image, const int32_t *p_audio_data) override;
	virtual void write_end() override;

	virtual bool handles_file(const String &p_path) const override;

public:
	MovieWriterOGV();
};

#endif // MOVIE_WRITER_OGV_H

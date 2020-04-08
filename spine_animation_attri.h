/******************************************************************************
* Spine Runtimes Software License v2.5
*
* Copyright (c) 2013-2016, Esoteric Software
* All rights reserved.
*
* You are granted a perpetual, non-exclusive, non-sublicensable, and
* non-transferable license to use, install, execute, and perform the Spine
* Runtimes software and derivative works solely for personal or internal
* use. Without the written permission of Esoteric Software (see Section 2 of
* the Spine Software License Agreement), you may not (a) modify, translate,
* adapt, or develop new applications using the Spine Runtimes or otherwise
* create derivative works or improvements of the Spine Runtimes or (b) remove,
* delete, alter, or obscure any trademarks or any copyright, trademark, patent,
* or other intellectual property or proprietary rights notices on or in the
* Software, including any copy thereof. Redistributions in binary or source
* form must include this license and terms.
*
* THIS SOFTWARE IS PROVIDED BY ESOTERIC SOFTWARE "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL ESOTERIC SOFTWARE BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES, BUSINESS INTERRUPTION, OR LOSS OF
* USE, DATA, OR PROFITS) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
* IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/
#pragma once

#include "core/class_db.h"
#include "core/resource.h"
#include "core/reference.h"

class SpineAnimationAttri : public Resource
{
	GDCLASS(SpineAnimationAttri, Resource);

public:
	SpineAnimationAttri();
	virtual ~SpineAnimationAttri();

	String get_animation_name() const;
	void set_animation_name(const String& p_name);
	int get_track() const;
	void set_track(int p_track);
	float get_delay() const;
	void set_delay(float p_time);
	bool get_loop() const;
	void set_loop(bool p_loop);
	float get_mix() const;
	void set_mix(float p_time);
	bool is_use_mix();
	void set_use_mix(bool p_value);
	float get_alpha() const;
	void set_alpha(float p_alpha);
	float get_start() const;
	void set_start(float p_time);
	float get_end() const;
	void set_end(float p_time);
	bool is_use_end();
	void set_use_end(bool p_value);
	float get_speed_scale() const;
	void set_speed_scale(float p_scale);

protected:
	//bool _set(const StringName &p_name, const Variant &p_value);
	//bool _get(const StringName &p_name, Variant &r_ret) const;
	//void _get_property_list(List<PropertyInfo> *p_list) const;
	static void _bind_methods();

private:
	String animation_name;
	int track;
	float delay;
	bool loop;
	float mix;
	bool use_mix;
	float alpha;
	float start;
	bool use_end;
	float end;
	float speed_scale;
};

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
#include "spine_animation_attri.h"


SpineAnimationAttri::SpineAnimationAttri() :
	animation_name(""), track(0), delay(0.0), loop(false), mix(0.0), use_mix(false), alpha(1.0), start(0.0), end(-1.0), use_end(false), speed_scale(1.0)
{

}

SpineAnimationAttri::~SpineAnimationAttri()
{

}

String SpineAnimationAttri::get_animation_name() const
{
	return animation_name;
}

void SpineAnimationAttri::set_animation_name(const String& p_name)
{
	if (animation_name == p_name)
		return;
	animation_name = p_name;
	_change_notify("animation_name");
	notify_change_to_owners();
}

int SpineAnimationAttri::get_track() const
{
	return track;
}

void SpineAnimationAttri::set_track(int p_track)
{
	if (track == p_track)
		return;
	track = p_track;
	_change_notify("track");
	notify_change_to_owners();
}

float SpineAnimationAttri::get_delay() const
{
	return delay;
}

void SpineAnimationAttri::set_delay(float p_time)
{
	if (delay == p_time)
		return;
	delay = p_time;
	_change_notify("delay");
	notify_change_to_owners();
}

bool SpineAnimationAttri::get_loop() const
{
	return loop;
}

void SpineAnimationAttri::set_loop(bool p_loop)
{
	if (loop == p_loop)
		return;
	loop = p_loop;
	_change_notify("loop");
	notify_change_to_owners();
}

float SpineAnimationAttri::get_mix() const
{
	return mix;
}

void SpineAnimationAttri::set_mix(float p_time)
{
	if (mix == p_time)
		return;
	mix = p_time;
	_change_notify("mix");
	notify_change_to_owners();
}

bool SpineAnimationAttri::is_use_mix()
{
	return use_mix;
}

void SpineAnimationAttri::set_use_mix(bool p_value)
{
	if (use_mix == p_value)
		return;
	use_mix = p_value;
	_change_notify("use_mix");
}

float SpineAnimationAttri::get_alpha() const
{
	return alpha;
}

void SpineAnimationAttri::set_alpha(float p_alpha)
{
	if (alpha == p_alpha)
		return;
	alpha = p_alpha;
	_change_notify("alpha");
	notify_change_to_owners();
}

float SpineAnimationAttri::get_start() const
{
	return start;
}

void SpineAnimationAttri::set_start(float p_time)
{
	if (start == p_time)
		return;
	start = p_time;
	_change_notify("start");
	notify_change_to_owners();
}

float SpineAnimationAttri::get_end() const
{
	return end;
}

void SpineAnimationAttri::set_end(float p_time)
{
	if (end == p_time)
		return;
	end = p_time;
	_change_notify("end");
	notify_change_to_owners();
}

bool SpineAnimationAttri::is_use_end()
{
	return use_end;
}

void SpineAnimationAttri::set_use_end(bool p_value)
{
	if (use_end == p_value)
		return;
	use_end = p_value;
	_change_notify("use_end");
}

float SpineAnimationAttri::get_speed_scale() const
{
	return speed_scale;
}

void SpineAnimationAttri::set_speed_scale(float p_scale)
{
	if (speed_scale == p_scale)
		return;
	speed_scale = p_scale;
	_change_notify("speed_scale");
	notify_change_to_owners();
}

//bool SpineAnimationAttri::_set(const StringName& p_name, const Variant& p_value)
//{
//	if (p_name == "end")
//	{
//		print_line("attribute _set end");
//		if (p_value.get_type() == Variant::NIL)
//		{
//			default_end = true;
//		}
//		else
//		{
//			default_end = false;
//			end = p_value;
//		}
//		return true;
//	}
//	return false;
//}
//bool SpineAnimationAttri::_get(const StringName& p_name, Variant& r_ret) const
//{
//	if (p_name == "end")
//	{
//		print_line("attribute _get end");
//		if (default_end)
//		{
//			r_ret = Variant();
//		}
//		else
//		{
//			r_ret = end;
//		}
//		return true;
//	}
//	return false;
//}
//void SpineAnimationAttri::_get_property_list(List<PropertyInfo>* p_list) const
//{
//	uint32_t usage;
//	usage = PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_CHECKABLE | PROPERTY_USAGE_STORAGE;
//	if (!default_end)
//		usage |= PROPERTY_USAGE_CHECKED;;
//	p_list->push_back(PropertyInfo(Variant::REAL, "end", PROPERTY_HINT_NONE, "", usage));
//	print_line("attribute _get_property_list");
//}

void SpineAnimationAttri::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_animation_name", "value"), &SpineAnimationAttri::set_animation_name);
	ClassDB::bind_method(D_METHOD("get_animation_name"), &SpineAnimationAttri::get_animation_name);
	ClassDB::bind_method(D_METHOD("set_track", "value"), &SpineAnimationAttri::set_track);
	ClassDB::bind_method(D_METHOD("get_track"), &SpineAnimationAttri::get_track);
	ClassDB::bind_method(D_METHOD("set_delay", "value"), &SpineAnimationAttri::set_delay);
	ClassDB::bind_method(D_METHOD("get_delay"), &SpineAnimationAttri::get_delay);
	ClassDB::bind_method(D_METHOD("set_loop", "value"), &SpineAnimationAttri::set_loop);
	ClassDB::bind_method(D_METHOD("get_loop"), &SpineAnimationAttri::get_loop);
	ClassDB::bind_method(D_METHOD("set_mix", "value"), &SpineAnimationAttri::set_mix);
	ClassDB::bind_method(D_METHOD("get_mix"), &SpineAnimationAttri::get_mix);
	ClassDB::bind_method(D_METHOD("set_use_mix", "value"), &SpineAnimationAttri::set_use_mix);
	ClassDB::bind_method(D_METHOD("is_use_mix"), &SpineAnimationAttri::is_use_mix);
	ClassDB::bind_method(D_METHOD("set_alpha", "value"), &SpineAnimationAttri::set_alpha);
	ClassDB::bind_method(D_METHOD("get_alpha"), &SpineAnimationAttri::get_alpha);
	ClassDB::bind_method(D_METHOD("set_start", "value"), &SpineAnimationAttri::set_start);
	ClassDB::bind_method(D_METHOD("get_start"), &SpineAnimationAttri::get_start);
	ClassDB::bind_method(D_METHOD("set_use_end", "value"), &SpineAnimationAttri::set_use_end);
	ClassDB::bind_method(D_METHOD("is_use_end"), &SpineAnimationAttri::is_use_end);
	ClassDB::bind_method(D_METHOD("set_end", "time"), &SpineAnimationAttri::set_end);
	ClassDB::bind_method(D_METHOD("get_end"), &SpineAnimationAttri::get_end);
	ClassDB::bind_method(D_METHOD("set_speed_scale", "value"), &SpineAnimationAttri::set_speed_scale);
	ClassDB::bind_method(D_METHOD("get_speed_scale"), &SpineAnimationAttri::get_speed_scale);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "animation_name"), "set_animation_name", "get_animation_name");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "track"), "set_track", "get_track");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "delay"), "set_delay", "get_delay");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "loop"), "set_loop", "get_loop");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "mix"), "set_mix", "get_mix");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_end"), "set_use_end", "is_use_end");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "alpha"), "set_alpha", "get_alpha");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "start"), "set_start", "get_start");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_end"), "set_use_end", "is_use_end");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "end"), "set_end", "get_end");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "speed_scale"), "set_speed_scale", "get_speed_scale");
}

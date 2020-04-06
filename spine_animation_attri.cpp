#include "spine_animation_attri.h"
//#include <core/method_bind_ext.gen.inc>

SpineAnimationAttri::SpineAnimationAttri() :
	animName()/*, hintString()*/, track(0), delay(0.0), loop(false), mix(-1.0), alpha(1.0), start(0.0), default_end(true), end(-1.0), speed_scale(1.0)
{

}

SpineAnimationAttri::~SpineAnimationAttri()
{

}

//String SpineAnimationAttri::get_animation_hint_string() const
//{
//	return hintString;
//}
//
//void SpineAnimationAttri::set_animation_hint_string(const String& p_hint)
//{
//	hintString = p_hint;
//	_change_notify();
//}

String SpineAnimationAttri::get_animation_name() const
{
	return animName;
}

void SpineAnimationAttri::set_animation_name(const String& p_name)
{
	animName = p_name;
	_change_notify("animation_name");
	notify_change_to_owners();
}

int SpineAnimationAttri::get_track() const
{
	return track;
}

void SpineAnimationAttri::set_track(int p_track)
{
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
	mix = p_time;
	_change_notify("mix");
	notify_change_to_owners();
}

float SpineAnimationAttri::get_alpha() const
{
	return alpha;
}

void SpineAnimationAttri::set_alpha(float p_alpha)
{
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
	start = p_time;
	_change_notify("start");
	notify_change_to_owners();
}

bool SpineAnimationAttri::is_default_end()
{
	return default_end;
}

void SpineAnimationAttri::set_default_end(bool p_value)
{
	if (default_end == p_value)
		return;
	default_end = p_value;
	_change_notify("default_end");
}

float SpineAnimationAttri::get_end() const
{
	return end;
}

void SpineAnimationAttri::set_end(float p_time)
{
	end = p_time;
	_change_notify("end");
	notify_change_to_owners();
}

float SpineAnimationAttri::get_speed_scale() const
{
	return speed_scale;
}

void SpineAnimationAttri::set_speed_scale(float p_scale)
{
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
	ClassDB::bind_method(D_METHOD("set_animation_name", "name"), &SpineAnimationAttri::set_animation_name);
	ClassDB::bind_method(D_METHOD("get_animation_name"), &SpineAnimationAttri::get_animation_name);
	ClassDB::bind_method(D_METHOD("set_track", "track"), &SpineAnimationAttri::set_track);
	ClassDB::bind_method(D_METHOD("get_track"), &SpineAnimationAttri::get_track);
	ClassDB::bind_method(D_METHOD("set_delay", "time"), &SpineAnimationAttri::set_delay);
	ClassDB::bind_method(D_METHOD("get_delay"), &SpineAnimationAttri::get_delay);
	ClassDB::bind_method(D_METHOD("set_loop", "loop"), &SpineAnimationAttri::set_loop);
	ClassDB::bind_method(D_METHOD("get_loop"), &SpineAnimationAttri::get_loop);
	ClassDB::bind_method(D_METHOD("set_mix", "time"), &SpineAnimationAttri::set_mix);
	ClassDB::bind_method(D_METHOD("get_mix"), &SpineAnimationAttri::get_mix);
	ClassDB::bind_method(D_METHOD("set_alpha", "alpha"), &SpineAnimationAttri::set_alpha);
	ClassDB::bind_method(D_METHOD("get_alpha"), &SpineAnimationAttri::get_alpha);
	ClassDB::bind_method(D_METHOD("set_start", "time"), &SpineAnimationAttri::set_start);
	ClassDB::bind_method(D_METHOD("get_start"), &SpineAnimationAttri::get_start);
	ClassDB::bind_method(D_METHOD("set_default_end", "time"), &SpineAnimationAttri::set_default_end);
	ClassDB::bind_method(D_METHOD("is_default_end"), &SpineAnimationAttri::is_default_end);
	ClassDB::bind_method(D_METHOD("set_end", "time"), &SpineAnimationAttri::set_end);
	ClassDB::bind_method(D_METHOD("get_end"), &SpineAnimationAttri::get_end);
	ClassDB::bind_method(D_METHOD("set_speed_scale", "scale"), &SpineAnimationAttri::set_speed_scale);
	ClassDB::bind_method(D_METHOD("get_speed_scale"), &SpineAnimationAttri::get_speed_scale);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "animation_name"), "set_animation_name", "get_animation_name");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "track"), "set_track", "get_track");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "delay"), "set_delay", "get_delay");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "loop"), "set_loop", "get_loop");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "mix"), "set_mix", "get_mix");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "alpha"), "set_alpha", "get_alpha");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "start"), "set_start", "get_start");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "default_end"), "set_default_end", "is_default_end");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "end"), "set_end", "get_end");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "speed_scale"), "set_speed_scale", "get_speed_scale");
}

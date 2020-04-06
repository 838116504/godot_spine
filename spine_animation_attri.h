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

	//String get_animation_hint_string() const;
	//void set_animation_hint_string(const String& p_hint);
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
	float get_alpha() const;
	void set_alpha(float p_alpha);
	float get_start() const;
	void set_start(float p_time);
	bool is_default_end();
	void set_default_end(bool p_value);
	float get_end() const;
	void set_end(float p_time);
	float get_speed_scale() const;
	void set_speed_scale(float p_scale);

protected:
	//bool _set(const StringName &p_name, const Variant &p_value);
	//bool _get(const StringName &p_name, Variant &r_ret) const;
	//void _get_property_list(List<PropertyInfo> *p_list) const;
	static void _bind_methods();

private:
	//String hintString;
	String animName;
	int track;
	float delay;
	bool loop;
	float mix;
	float alpha;
	float start;
	bool default_end;
	float end;
	float speed_scale;
};

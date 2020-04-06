#pragma once

#include "core/class_db.h"
#include "core/object.h"
#include "spine_animation_attri.h"
#include <spine/AnimationState.h>

class SpineAnimation : public Object
{
	GDCLASS(SpineAnimation, Object);
	OBJ_CATEGORY("SpineAnimations");
public:
	SpineAnimation();
	SpineAnimation(Variant var);
	virtual ~SpineAnimation();

	float get_alpha() const;
	void set_alpha(float p_alpha);
	float get_delay() const;
	void set_delay(float p_time);
	float get_start() const;
	void set_start(float p_time);
	float get_end() const;
	void set_end(float p_time);
	float get_mix_duration() const;
	void set_mix_duration(float p_time);
	float get_speed_scale() const;
	void set_speed_scale(float p_scale);
	String get_name() const;
	float get_duration() const;
	float get_final_time_scale() const;
	float get_time() const;
	float get_mix() const;
	Object* get_next_animation() const;
	int get_track() const;
	Ref<SpineAnimationAttri> get_attri() const;
	operator Variant() const
	{
		return Variant(cast_to<Object>(this));
	}
protected:
	static void _bind_methods();

public:
	spTrackEntry* trackEntry;
	spAnimationState* state;
};

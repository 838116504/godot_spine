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
#include "spine_animation.h"

SpineAnimation::SpineAnimation() : trackEntry(nullptr)
{
}

SpineAnimation::SpineAnimation(Variant var)
{
	SpineAnimation* target = cast_to<SpineAnimation>(var);
	trackEntry = target->trackEntry;
	state = target->state;
}

SpineAnimation::~SpineAnimation()
{
}

float SpineAnimation::get_alpha() const
{
	ERR_FAIL_COND_V(trackEntry == NULL, 0.0);
	return trackEntry->alpha;
}

void SpineAnimation::set_alpha(float p_alpha)
{
	ERR_FAIL_COND(trackEntry == NULL);
	trackEntry->alpha = p_alpha;
}

float SpineAnimation::get_delay() const
{
	ERR_FAIL_COND_V(trackEntry == NULL, 0.0);
	return trackEntry->delay;
}

void SpineAnimation::set_delay(float p_time)
{
	ERR_FAIL_COND(trackEntry == NULL);
	trackEntry->delay = p_time;
}

float SpineAnimation::get_start() const
{
	ERR_FAIL_COND_V(trackEntry == NULL, 0.0);
	return trackEntry->animationStart;
}

void SpineAnimation::set_start(float p_time)
{
	ERR_FAIL_COND(trackEntry == NULL);
	trackEntry->animationStart = p_time;
}

float SpineAnimation::get_end() const
{
	ERR_FAIL_COND_V(trackEntry == NULL, 0.0);
	return trackEntry->animationEnd;
}

void SpineAnimation::set_end(float p_time)
{
	ERR_FAIL_COND(trackEntry == NULL);
	trackEntry->animationEnd = p_time;
}

float SpineAnimation::get_mix_duration() const
{
	ERR_FAIL_COND_V(trackEntry == NULL, 0.0);
	return trackEntry->mixDuration;
}

void SpineAnimation::set_mix_duration(float p_time)
{
	ERR_FAIL_COND(trackEntry == NULL);
	trackEntry->mixDuration = p_time;
}

float SpineAnimation::get_speed_scale() const
{
	ERR_FAIL_COND_V(trackEntry == NULL, 1.0);
	return trackEntry->timeScale;
}

void SpineAnimation::set_speed_scale(float p_scale)
{
	ERR_FAIL_COND(trackEntry == NULL);
	trackEntry->timeScale = p_scale;
}

void SpineAnimation::set_loop()
{
	ERR_FAIL_COND(trackEntry == NULL);
	trackEntry->loop = true;
}

bool SpineAnimation::is_loop() const
{
	ERR_FAIL_COND_V(trackEntry == NULL, false);
	return trackEntry->loop;
}

String SpineAnimation::get_name() const
{
	ERR_FAIL_COND_V(trackEntry == NULL, "");
	return trackEntry->animation->name;
}

float SpineAnimation::get_duration() const
{
	ERR_FAIL_COND_V(trackEntry == NULL, 0.0);
	return trackEntry->animationEnd - trackEntry->animationStart;
}

float SpineAnimation::get_final_time_scale() const
{
	ERR_FAIL_COND_V(trackEntry == NULL || state == NULL, 0.0);
	return state->timeScale * trackEntry->timeScale;
}

float SpineAnimation::get_time() const
{
	ERR_FAIL_COND_V(trackEntry == NULL, 0.0);
	return spTrackEntry_getAnimationTime(trackEntry);
}

float SpineAnimation::get_mix() const
{
	ERR_FAIL_COND_V(trackEntry == NULL, 0);
	return trackEntry->mixTime;
}

Object* SpineAnimation::get_next_animation() const
{
	ERR_FAIL_COND_V(trackEntry == NULL, nullptr);
	return (SpineAnimation*)trackEntry->next->userData;
}

int SpineAnimation::get_track() const
{
	ERR_FAIL_COND_V(trackEntry == NULL, 0);
	return trackEntry->trackIndex;
}

void SpineAnimation::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_alpha", "alpha"), &SpineAnimation::set_alpha);
	ClassDB::bind_method(D_METHOD("get_alpha"), &SpineAnimation::get_alpha);
	ClassDB::bind_method(D_METHOD("set_delay", "time"), &SpineAnimation::set_delay);
	ClassDB::bind_method(D_METHOD("get_delay"), &SpineAnimation::get_delay);
	ClassDB::bind_method(D_METHOD("set_start", "time"), &SpineAnimation::set_start);
	ClassDB::bind_method(D_METHOD("get_start"), &SpineAnimation::get_start);
	ClassDB::bind_method(D_METHOD("set_end", "time"), &SpineAnimation::set_end);
	ClassDB::bind_method(D_METHOD("get_end"), &SpineAnimation::get_end);
	ClassDB::bind_method(D_METHOD("set_mix_duration", "time"), &SpineAnimation::set_mix_duration);
	ClassDB::bind_method(D_METHOD("get_mix_duration"), &SpineAnimation::get_duration);
	ClassDB::bind_method(D_METHOD("set_speed_scale", "time"), &SpineAnimation::set_speed_scale);
	ClassDB::bind_method(D_METHOD("get_speed_scale"), &SpineAnimation::get_speed_scale);
	ClassDB::bind_method(D_METHOD("get_name"), &SpineAnimation::get_name);
	ClassDB::bind_method(D_METHOD("get_duration"), &SpineAnimation::get_duration);
	ClassDB::bind_method(D_METHOD("get_final_time_scale"), &SpineAnimation::get_final_time_scale);
	ClassDB::bind_method(D_METHOD("get_time"), &SpineAnimation::get_time);
	ClassDB::bind_method(D_METHOD("get_mix"), &SpineAnimation::get_mix);
	ClassDB::bind_method(D_METHOD("get_next_animation"), &SpineAnimation::get_next_animation);
	ClassDB::bind_method(D_METHOD("get_track"), &SpineAnimation::get_track);
	ClassDB::bind_method(D_METHOD("get_attri"), &SpineAnimation::get_attri);

	ADD_SIGNAL(MethodInfo("started"));
	ADD_SIGNAL(MethodInfo("interrupted"));
	ADD_SIGNAL(MethodInfo("completed"));
	ADD_SIGNAL(MethodInfo("ended"));
	ADD_SIGNAL(MethodInfo("event", PropertyInfo(Variant::STRING, "event"), PropertyInfo(Variant::INT, "intParam"), PropertyInfo(Variant::REAL, "floatParam"), PropertyInfo(Variant::STRING, "strParam")));
}

Ref<SpineAnimationAttri> SpineAnimation::get_attri() const
{
	ERR_FAIL_COND_V(trackEntry == NULL || state == NULL || state->data->skeletonData->animationsCount <= 0, nullptr);

	Ref<SpineAnimationAttri> ret = memnew(SpineAnimationAttri);
	ret->set_animation_name(trackEntry->animation->name);
	ret->set_delay(trackEntry->delay);
	ret->set_loop(trackEntry->loop);
	ret->set_alpha(trackEntry->alpha);
	ret->set_start(trackEntry->animationStart);
	ret->set_end(trackEntry->animationEnd);
	ret->set_track(trackEntry->trackIndex);
	ret->set_mix(trackEntry->mixDuration);
	ret->set_speed_scale(trackEntry->timeScale);
	return ret;
}

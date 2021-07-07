/******************************************************************************
 * Spine Runtimes License Agreement
 * Last updated January 1, 2020. Replaces all prior versions.
 *
 * Copyright (c) 2013-2020, Esoteric Software LLC
 *
 * Integration of the Spine Runtimes into software or otherwise creating
 * derivative works of the Spine Runtimes is permitted under the terms and
 * conditions of Section 2 of the Spine Editor License Agreement:
 * http://esotericsoftware.com/spine-editor-license
 *
 * Otherwise, it is permitted to integrate the Spine Runtimes into software
 * or otherwise create derivative works of the Spine Runtimes (collectively,
 * "Products"), provided that each user of the Products must obtain their own
 * Spine Editor license and redistribution of the Products in any form must
 * include this license and copyright notice.
 *
 * THE SPINE RUNTIMES ARE PROVIDED BY ESOTERIC SOFTWARE LLC "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ESOTERIC SOFTWARE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES,
 * BUSINESS INTERRUPTION, OR LOSS OF USE, DATA, OR PROFITS) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THE SPINE RUNTIMES, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#pragma once

#include "core/resource.h"

#include <spine/spine.h>


class SpineEvent : public Resource
{
	GDCLASS(SpineEvent, Resource);

private:
	spEventData* m_event;

protected:
	static void _bind_methods();

public:
	void set_event(spEvent* p_event);

	void set_event_name(const String& p_value);
	String get_event_name() const;

	void set_int_value(int p_value);
	int get_int_value() const;
	void set_float_value(float p_value);
	float get_float_value() const;
	void set_string_value(const String& p_value);
	String get_string_value() const;

	void set_audio_path(const String& p_value);
	String get_audio_path() const;
	void set_volume(float p_value);
	float get_volume() const;
	void set_balance(float p_value);
	float get_balance() const;

	SpineEvent();
	~SpineEvent();
};


class SpineAtlas : public Resource
{
	GDCLASS(SpineAtlas, Resource);

public:
	SpineAtlas();
	~SpineAtlas();

	bool load();

	spAtlas *data;
};


class SpineSkeleton : public Resource
{
	GDCLASS(SpineSkeleton, Resource);

public:
	SpineSkeleton() {};
	~SpineSkeleton() {};

	virtual spSkeletonData* load(spAtlas*, float) = 0;
};

class SpineJsonSkeleton : public SpineSkeleton
{
	GDCLASS(SpineJsonSkeleton, SpineSkeleton);

public:
	SpineJsonSkeleton() {};
	~SpineJsonSkeleton() {};

	virtual spSkeletonData* load(spAtlas*, float);
};


class SpineBinarySkeleton : public SpineSkeleton
{
	GDCLASS(SpineBinarySkeleton, SpineSkeleton);

public:
	SpineBinarySkeleton() {};
	~SpineBinarySkeleton() {};

	virtual spSkeletonData* load(spAtlas*, float);
};


class SpineSkeletonData : public Resource
{
	GDCLASS(SpineSkeletonData, Resource);

public:
	SpineSkeletonData();
	~SpineSkeletonData();

	void set_scale(float p_scale);
	float get_scale();
	void set_atlas(Ref<SpineAtlas> p_atlas);
	Ref<SpineAtlas> get_atlas();
	void set_skeleton(Ref<SpineSkeleton> p_skel);
	Ref<SpineSkeleton> get_skeleton();
	bool is_valid();

protected:
	static void _bind_methods();

public:
	Ref<SpineAtlas> atlas;
	Ref<SpineSkeleton> skel;
	float scale;
	spSkeletonData *data;
};

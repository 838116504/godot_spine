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

#include "core/resource.h"
#include <spine/spine.h>

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

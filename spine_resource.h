#pragma once

#include "core/resource.h"
#include <spine/spine.h>

class SpineAtlas : public Resource
{
	GDCLASS(SpineAtlas, Resource)
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
	//void updatePath();

protected:
	static void _bind_methods();

public:
	Ref<SpineAtlas> atlas;
	Ref<SpineSkeleton> skel;
	float scale;
	spSkeletonData *data;
};

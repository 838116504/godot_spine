#include "spine_resource.h"
//#include <core/method_bind_ext.gen.inc>
#include <spine/extension.h>

SpineAtlas::SpineAtlas() : data(nullptr)
{
}

SpineAtlas::~SpineAtlas()
{
	if (data != nullptr)
		spAtlas_dispose(data);
}

bool SpineAtlas::load()
{
	data = spAtlas_createFromFile(get_path().utf8().get_data(), 0);
	return data != NULL;
}



spSkeletonData* SpineJsonSkeleton::load(spAtlas* atlas, float scale)
{
	String str = "json path: " + get_path();
	print_line(str);
	spSkeletonJson *json = spSkeletonJson_create(atlas);
	ERR_FAIL_COND_V(json == NULL, false);
	json->scale = scale;

	//int len;
	//auto str2 = _spUtil_readFile(get_path().utf8().get_data(), &len);
	//print_line(str2);
	spSkeletonData* data = spSkeletonJson_readSkeletonDataFile(json, get_path().utf8().get_data());
	spSkeletonJson_dispose(json);
	//return data;
	return nullptr;
}



spSkeletonData* SpineBinarySkeleton::load(spAtlas* atlas, float scale)
{
	spSkeletonBinary* bin  = spSkeletonBinary_create(atlas);
	ERR_FAIL_COND_V(bin == NULL, false);
	bin->scale = scale;
	spSkeletonData* data = spSkeletonBinary_readSkeletonDataFile(bin, get_path().utf8().get_data());
	spSkeletonBinary_dispose(bin);
	return data;
}




SpineSkeletonData::SpineSkeletonData() : scale(1.0), data(nullptr)
{
}

SpineSkeletonData::~SpineSkeletonData()
{
	if (data != nullptr)
		spSkeletonData_dispose(data);
}

void SpineSkeletonData::set_scale(float p_scale)
{
	scale = p_scale;
	//updatePath();
	if (skel.is_valid() && atlas.is_valid())
	{
		if (data != NULL)
			spSkeletonData_dispose(data);
		data = skel->load(atlas->data, scale);
		notify_change_to_owners();
	}
}

float SpineSkeletonData::get_scale()
{
	return scale;
}

void SpineSkeletonData::set_skeleton(Ref<SpineSkeleton> p_skel)
{
	skel = p_skel;
	//updatePath();
	if (skel.is_valid() && atlas.is_valid())
	{
		if (data != NULL)
			spSkeletonData_dispose(data);
		data = skel->load(atlas->data, scale);
		notify_change_to_owners();
	}
	else if (data != NULL)
	{
		spSkeletonData_dispose(data);
		data = NULL;
		notify_change_to_owners();
	}
}

void SpineSkeletonData::set_atlas(Ref<SpineAtlas> p_atlas)
{
	atlas = p_atlas;
	//updatePath();
	if (skel.is_valid() && atlas.is_valid())
	{
		if (data != NULL)
			spSkeletonData_dispose(data);
		data = skel->load(atlas->data, scale);
		notify_change_to_owners();
	}
	else if (data != NULL)
	{
		spSkeletonData_dispose(data);
		data = NULL;
		notify_change_to_owners();
	}
}


Ref<SpineAtlas> SpineSkeletonData::get_atlas()
{
	return atlas;
}

Ref<SpineSkeleton> SpineSkeletonData::get_skeleton()
{
	return skel;
}

bool SpineSkeletonData::is_valid()
{
	return data != NULL;
}

//void SpineSkeletonData::updatePath()
//{
	/*if (skel.is_valid() && atlas.is_valid())
	{
		String atlasPath = atlas->get_path();
		int find = atlasPath.find("//");
		if (find >= 0)
		{
			atlasPath = atlasPath.substr(find + 2);
		}
		set_path(skel->get_path() + atlasPath + ":" + Variant(scale));
	} else
		set_path("");*/
//}

void SpineSkeletonData::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_scale", "scale"), &SpineSkeletonData::set_scale);
	ClassDB::bind_method(D_METHOD("get_scale"), &SpineSkeletonData::get_scale);
	ClassDB::bind_method(D_METHOD("set_atlas", "atlas"), &SpineSkeletonData::set_atlas);
	ClassDB::bind_method(D_METHOD("get_atlas"), &SpineSkeletonData::get_atlas);
	ClassDB::bind_method(D_METHOD("set_skeleton", "skel"), &SpineSkeletonData::set_skeleton);
	ClassDB::bind_method(D_METHOD("get_skeleton"), &SpineSkeletonData::get_skeleton);
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "scale"), "set_scale", "get_scale");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "atlas", PROPERTY_HINT_RESOURCE_TYPE, "SpineAtlas"), "set_atlas", "get_atlas");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "skeleton", PROPERTY_HINT_RESOURCE_TYPE, "SpineSkeleton"), "set_skeleton", "get_skeleton");
}

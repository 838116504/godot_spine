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
#include "spine_resource.h"
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
	spSkeletonJson *json = spSkeletonJson_create(atlas);
	ERR_FAIL_COND_V(json == NULL, nullptr);
	json->scale = scale;

	spSkeletonData* data = spSkeletonJson_readSkeletonDataFile(json, get_path().utf8().get_data());
	spSkeletonJson_dispose(json);

	return nullptr;
}

spSkeletonData* SpineBinarySkeleton::load(spAtlas* atlas, float scale)
{
	spSkeletonBinary* bin  = spSkeletonBinary_create(atlas);
	ERR_FAIL_COND_V(bin == NULL, nullptr);
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

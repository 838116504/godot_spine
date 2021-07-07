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

#include "spine_resource.h"

#include <spine/extension.h>


void SpineEvent::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_event_name", "value"), &SpineEvent::set_event_name);
	ClassDB::bind_method(D_METHOD("get_event_name"), &SpineEvent::get_event_name);

	ClassDB::bind_method(D_METHOD("set_int_value", "value"), &SpineEvent::set_int_value);
	ClassDB::bind_method(D_METHOD("get_int_value"), &SpineEvent::get_int_value);
	ClassDB::bind_method(D_METHOD("set_float_value", "value"), &SpineEvent::set_float_value);
	ClassDB::bind_method(D_METHOD("get_float_value"), &SpineEvent::get_float_value);
	ClassDB::bind_method(D_METHOD("set_string_value", "value"), &SpineEvent::set_string_value);
	ClassDB::bind_method(D_METHOD("get_string_value"), &SpineEvent::get_string_value);

	ClassDB::bind_method(D_METHOD("set_audio_path", "value"), &SpineEvent::set_audio_path);
	ClassDB::bind_method(D_METHOD("get_audio_path"), &SpineEvent::get_audio_path);
	ClassDB::bind_method(D_METHOD("set_volume", "value"), &SpineEvent::set_volume);
	ClassDB::bind_method(D_METHOD("get_volume"), &SpineEvent::get_volume);
	ClassDB::bind_method(D_METHOD("set_balance", "value"), &SpineEvent::set_balance);
	ClassDB::bind_method(D_METHOD("get_balance"), &SpineEvent::get_balance);


	ADD_PROPERTY(PropertyInfo(Variant::STRING, "name"), "set_name", "get_name");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "int_value"), "set_int_value", "get_int_value");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "float_value"), "set_float_value", "get_float_value");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "string_value"), "set_string_value", "get_string_value");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "audio_path"), "set_audio_path", "get_audio_path");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "volume"), "set_volume", "get_volume");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "balance"), "set_balance", "get_balance");
}

void SpineEvent::set_event(spEvent* p_event)
{
	ERR_FAIL_NULL(p_event);
	FREE(m_event->name);
	if (p_event->data->name)
		MALLOC_STR(m_event->name, p_event->data->name);
	else
		CONST_CAST(char*, m_event->name) = NULL;

	m_event->intValue = p_event->intValue;
	m_event->floatValue = p_event->floatValue;

	FREE(m_event->stringValue);
	if (p_event->stringValue)
		MALLOC_STR(m_event->stringValue, p_event->stringValue);
	else
		m_event->stringValue = NULL;

	FREE(m_event->audioPath);
	if (p_event->data->audioPath)
		MALLOC_STR(m_event->audioPath, p_event->data->audioPath);
	else
		m_event->audioPath = NULL;

	m_event->volume = p_event->volume;
	m_event->balance = p_event->balance;

	emit_changed();
}

void SpineEvent::set_event_name(const String& p_value)
{
	if (p_value == m_event->name)
		return;

	FREE(m_event->name);
	MALLOC_STR(m_event->name, p_value.utf8().get_data());

	emit_changed();
}

String SpineEvent::get_event_name() const
{
	return m_event->name ? m_event->name : "";
}

void SpineEvent::set_int_value(int p_value)
{
	if (p_value == m_event->intValue)
		return;

	m_event->intValue = p_value;

	emit_changed();
}

int SpineEvent::get_int_value() const
{
	return m_event->intValue;
}

void SpineEvent::set_float_value(float p_value)
{
	if (p_value == m_event->floatValue)
		return;

	m_event->floatValue = p_value;

	emit_changed();
}

float SpineEvent::get_float_value() const
{
	return m_event->floatValue;
}

void SpineEvent::set_string_value(const String& p_value)
{
	if (p_value == m_event->stringValue)
		return;

	FREE(m_event->stringValue);
	MALLOC_STR(m_event->stringValue, p_value.utf8().get_data());

	emit_changed();
}

String SpineEvent::get_string_value() const
{
	return m_event->stringValue ? m_event->stringValue : "";
}

void SpineEvent::set_audio_path(const String& p_value)
{
	if (p_value == m_event->audioPath)
		return;

	FREE(m_event->audioPath);
	if (p_value.length() > 0)
		MALLOC_STR(m_event->audioPath, p_value.utf8().get_data());
	else
		m_event->audioPath = NULL;

	emit_changed();
}

String SpineEvent::get_audio_path() const
{
	return m_event->audioPath ? m_event->audioPath : "";
}

void SpineEvent::set_volume(float p_value)
{
	if (p_value == m_event->volume)
		return;

	m_event->volume = p_value;

	emit_changed();
}

float SpineEvent::get_volume() const
{
	return m_event->volume;
}

void SpineEvent::set_balance(float p_value)
{
	if (p_value == m_event->balance)
		return;

	m_event->balance = p_value;

	emit_changed();
}

float SpineEvent::get_balance() const
{
	return m_event->balance;
}

SpineEvent::SpineEvent()
{
	m_event = spEventData_create("");
}

SpineEvent::~SpineEvent()
{
	spEventData_dispose(m_event);
}



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
	if (!data)
		print_line(json->error);
	spSkeletonJson_dispose(json);

	return data;
}

spSkeletonData* SpineBinarySkeleton::load(spAtlas* atlas, float scale)
{
	spSkeletonBinary* bin  = spSkeletonBinary_create(atlas);
	ERR_FAIL_COND_V(bin == NULL, nullptr);
	bin->scale = scale;
	spSkeletonData* data = spSkeletonBinary_readSkeletonDataFile(bin, get_path().utf8().get_data());
	if (!data)
		print_line(bin->error);
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
	if (p_skel == skel)
		return;

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
	if (p_atlas == atlas)
		return;

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

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
#include "spine.h"
#include <spine/extension.h>
#include <spine/spine.h>
#include "core/array.h"
#include "scene/resources/convex_polygon_shape_2d.h"


/*
Array *Spine::invalid_names = NULL;
Array Spine::get_invalid_names() {
	if (invalid_names == NULL) {
		invalid_names = memnew(Array());
	}
	return *invalid_names;
}
*/

void Spine::spine_animation_callback(spAnimationState *p_state, spEventType p_type, spTrackEntry *p_track, spEvent *p_event)
{
	((Spine *)p_state->rendererObject)->_on_animation_state_event(p_track, p_type, p_event);
}

void Spine::_on_animation_state_event(spTrackEntry *p_track, spEventType p_type, spEvent *p_event)
{
	SpineAnimation* animation = reinterpret_cast<SpineAnimation*>(p_track->userData);
	switch (p_type) {
		case SP_ANIMATION_START:
			if (animation)
			{
				animation->trackEntry = p_track;
			}
			emit_signal("animation_started", animation);
			if (animation)
				animation->emit_signal("started");
			break;
		case SP_ANIMATION_INTERRUPT:
			emit_signal("animation_interrupted", animation);
			if (animation)
				animation->emit_signal("interrupted");
			break;
		case SP_ANIMATION_COMPLETE:
			emit_signal("animation_completed", animation);

			if (animation)
				animation->emit_signal("completed");
			if (!p_track->loop && p_track->next == NULL)
			{
				clear(p_track->trackIndex);
			}
			break;
		case SP_ANIMATION_END:
			emit_signal("animation_ended", animation);

			if (animation)
				animation->emit_signal("ended");

			if (playing && is_end())
			{
				if (!auto_play || internal_animations.size() == 0)
				{
					set_playing(false);
					emit_signal("ended");
				}
			}
			break;
		case SP_ANIMATION_EVENT:
		{
			Dictionary params;
			params["int"] = p_event->intValue;
			params["float"] = p_event->floatValue;
			params["str"] = p_event->stringValue ? p_event->stringValue : "";
			emit_signal("animation_event", animation, p_event->data->name, params);
			if (animation)
				animation->emit_signal("event", p_event->data->name, p_event->intValue, p_event->floatValue, p_event->stringValue ? p_event->stringValue : "");
		} break;
		case SP_ANIMATION_DISPOSE:
			if (animation)
				memdelete(animation);
			if (playing && is_end())
			{
				if (auto_play && internal_animations.size() > 0)
				{
					add_internal_animations();
					//print_line("replay!");
				}
			}
			break;
	}
}

void Spine::_spine_dispose()
{
	if (state)
	{
		spAnimationStateData_dispose(state->data);
		spAnimationState_dispose(state);
		state = NULL;
	}

	if (skeleton)
		spSkeleton_dispose(skeleton);

	
	skeleton = NULL;
	

	for (AttachmentNodes::Element *E = attachment_nodes.front(); E; E = E->next())
	{
		AttachmentNode &node = E->get();
		Object* obj = node.ref->get_ref();
		if (obj != NULL)
			obj->remove_meta("spine_meta");
		memdelete(node.ref);
	}
	attachment_nodes.clear();

	_bounding_boxs.clear();

	update();
}

void Spine::_spine_create()
{
	if (res->data == NULL)
		return;

	skeleton = spSkeleton_create(res->data);
	skeleton->x = spine_offset.x;
	skeleton->y = spine_offset.y;
	skeleton->scaleX = spine_scale.x;
	skeleton->scaleY = spine_scale.y;
	skeleton->time = 0.0f;
	

	state = spAnimationState_create(spAnimationStateData_create(skeleton->data));
	state->rendererObject = this;
	state->timeScale = speed_scale;
	state->data->defaultMix = default_mix;

	state->listener = spine_animation_callback;

	_update_verties_count();

	reset();
	if (auto_play && playing)
	{
		add_internal_animations();
	}
}

static Ref<Texture> spine_get_texture(spRegionAttachment *attachment) {

	if (Ref<Texture> *ref = static_cast<Ref<Texture> *>(((spAtlasRegion *)attachment->rendererObject)->page->rendererObject))
		return *ref;
	return NULL;
}

static Ref<Texture> spine_get_texture(spMeshAttachment *attachment) {

	if (Ref<Texture> *ref = static_cast<Ref<Texture> *>(((spAtlasRegion *)attachment->rendererObject)->page->rendererObject))
		return *ref;
	return NULL;
}

void Spine::_animation_draw() {

	if (skeleton == NULL)
		return;

	Color color;
	const float *uvs = NULL;
	int verties_count = 0;
	unsigned short *triangles = NULL;
	int triangles_count = 0;
	float r = 0, g = 0, b = 0, a = 0;

	RID ci = this->get_canvas_item();
	batcher.reset();
	// VisualServer::get_singleton()->canvas_item_add_set_blend_mode(ci, VS::MaterialBlendMode(get_blend_mode()));

	for (int i = 0, n = skeleton->slotsCount; i < n; i++)
	{
		spSlot *slot = skeleton->drawOrder[i];
		if (!slot->attachment)
			continue;
		
		Ref<Texture> texture;
		switch (slot->attachment->type)
		{
			case SP_ATTACHMENT_REGION:
			{
				spRegionAttachment *attachment = (spRegionAttachment *)slot->attachment;
				spRegionAttachment_computeWorldVertices(attachment, slot->bone, world_verts.ptrw(), 0, 2);
				texture = spine_get_texture(attachment);
				uvs = attachment->uvs;
				verties_count = 8;
				static unsigned short quadTriangles[6] = { 0, 1, 2, 2, 3, 0 };
				triangles = quadTriangles;
				triangles_count = 6;
				r = attachment->color.r;
				g = attachment->color.g;
				b = attachment->color.b;
				a = attachment->color.a;
				break;
			}
			case SP_ATTACHMENT_MESH: {

				spMeshAttachment *attachment = (spMeshAttachment *)slot->attachment;
				spVertexAttachment_computeWorldVertices(SUPER(attachment), slot, 0, attachment->super.worldVerticesLength, world_verts.ptrw(), 0, 2);
				texture = spine_get_texture(attachment);
				uvs = attachment->uvs;
				verties_count = ((spVertexAttachment *)attachment)->worldVerticesLength;
				triangles = attachment->triangles;
				triangles_count = attachment->trianglesCount;
				r = attachment->color.r;
				g = attachment->color.g;
				b = attachment->color.b;
				a = attachment->color.a;
				break;
			}
		}
		if (texture.is_null())
			continue;
		
		color.a = skeleton->color.a * slot->color.a * a;
		color.r = skeleton->color.r * slot->color.r * r;
		color.g = skeleton->color.g * slot->color.g * g;
		color.b = skeleton->color.b * slot->color.b * b;

		batcher.add(texture, world_verts.ptr(), uvs, verties_count, triangles, triangles_count, &color);
	}
	batcher.flush();

	if (show_bone) {
		// Bone size.
		float r = 3.0;
		for (int i = 0; i < skeleton->bonesCount; i++)
		{
			spBone *bone = skeleton->bones[i];
			Vector2 vec(bone->data->length* bone->a, bone->data->length* bone->c);
			float x = vec.x + bone->worldX;
			float y = vec.y + bone->worldY;
			vec = vec.rotated(PI / 2).normalized() * r;
			draw_line(Point2(bone->worldX+vec.x, -bone->worldY-vec.y), Point2(x, -y),	Color(1, 0, 0, 1), 1);
			draw_line(Point2(bone->worldX-vec.x, -bone->worldY+vec.y), Point2(x, -y),	Color(1, 0, 0, 1), 1);
		}
		// Bone origins.
		for (int i = 0; i < skeleton->bonesCount; i++)
		{
			spBone *bone = skeleton->bones[i];
			draw_circle(Point2(bone->worldX, -bone->worldY), r, (i == 0) ? Color(0, 1, 0, 1) : Color(1, 0, 0, 1));
		}
	}
}

void Spine::_animation_process(float p_delta)
{
	spAnimationState_update(state, p_delta);
	spAnimationState_apply(state, skeleton);
	spSkeleton_updateWorldTransform(skeleton);
	_update_attachment_node();

	_update_bounding_box();
	
	//BoundingBox::Element* BBE;
	//BoundingBox::Element* next;
	//for (SlotPolygons::Element* E = _polygons.front(); E; E = E->next())
	//{
	//	BBE = E->get().polygons.front();
	//	slot = E->get().slot;
	//	if (!slot->attachment)
	//	{
	//		E->get().polygons.clear();
	//		continue;
	//	}
	//	switch (slot->attachment->type)
	//	{
	//	case SP_ATTACHMENT_REGION:
	//	{
	//		spRegionAttachment *attachment = (spRegionAttachment *)slot->attachment;
	//		spRegionAttachment_computeWorldVertices(attachment, slot->bone, world_verts.ptrw(), 0, 2);
	//		if (!BBE)
	//		{
	//			E->get().polygons.push_back(Polygon());
	//			BBE = E->get().polygons.back();
	//		}
	//		collision = BBE->get().get_polygon();
	//		if (!collision)
	//		{
	//			collision = memnew(CollisionPolygon2D);
	//			BBE->get().set_polygon(collision);
	//			add_child(collision);
	//		}
	//		Vector<Vector2> temp = Vector<Vector2>();
	//		temp.resize(4);
	//		for (int j = 0; j < temp.size(); ++j)
	//		{
	//			temp.set(j, Vector2(world_verts[j * 2], world_verts[j * 2 + 1]));
	//		}
	//		collision->set_polygon(temp);
	//		BBE = BBE->next();
	//	} break;
	//	case SP_ATTACHMENT_MESH:
	//	{
	//		spMeshAttachment *attachment = (spMeshAttachment *)slot->attachment;
	//		spVertexAttachment_computeWorldVertices(SUPER(attachment), slot, 0, attachment->super.worldVerticesLength, world_verts.ptrw(), 0, 2);
	//		if (attachment->trianglesCount <= 0)
	//			break;
	//
	//		Array vertIds = Array();
	//		//Vector<Vector<unsigned short> > vertIds = Vector<Vector<unsigned short> >();
	//		vertIds.push_back(Array());
	//		for (int i = 0; i < 3; ++i)
	//			vertIds[0].operator Array().push_back(attachment->triangles[i]);
	//		unsigned short find;
	//		Array tempArray;
	//		for (int t = 1; t < attachment->trianglesCount; ++t)
	//		{
	//			find = -1;
	//			for (int i = 0; i < 3; ++i)
	//			{
	//				if (find != -1)
	//					break;
	//
	//				for (int j = 0 ; j < vertIds.size(); ++j)
	//				{
	//					tempArray = vertIds[j].operator Array();
	//					for (int k = 0; k < tempArray.size(); ++k)
	//					{
	//						if (attachment->triangles[t * 3 + i] == tempArray[k].operator unsigned short())
	//						{
	//							unsigned short next;
	//							if (tempArray.size() > k + 1)
	//								next = tempArray[k + 1].operator unsigned short();
	//							else
	//								next = tempArray[0].operator unsigned short();
	//							for (int l = 0; l < 3; ++l)
	//							{
	//								if (l == k)
	//									continue;
	//								if (attachment->triangles[t * 3 + l] == next)
	//								{
	//									if (k != 2 && l != 2)
	//										find = 2;
	//									else if (k != 1 && l != 1)
	//										find = 1;
	//									else
	//										find = 0;
	//
	//									break;
	//								}
	//							}
	//							if (!find)
	//							{
	//								if (k - 1 >= 0)
	//									next = tempArray[k - 1].operator unsigned short();
	//								else
	//									next = tempArray[tempArray.size() - 1].operator unsigned short();
	//								for (int l = 0; l < 3; ++l)
	//								{
	//									if (l == k)
	//										continue;
	//									if (attachment->triangles[t * 3 + l] == next)
	//									{
	//										if (k != 2 && l != 2)
	//											find = 2;
	//										else if (k != 1 && l != 1)
	//											find = 1;
	//										else
	//											find = 0;
	//										
	//										break;
	//									}
	//								}
	//							}
	//							if (find != -1)
	//								break;
	//						}
	//					}
	//					if (find != -1)
	//					{
	//						vertIds[j].operator Array().push_back(t * 3 + find);
	//						break;
	//					}
	//				}
	//			}
	//			if (find == -1)
	//			{
	//				for (int i = 0; i < 3; ++i)
	//					vertIds[0].operator Array().push_back(attachment->triangles[t*3 + i]);
	//			}
	//		}
	//
	//
	//		
	//	} break;
	//	}
	//	while (BBE)
	//	{
	//		next = E->next();
	//
	//		E->get().polygons.erase(BBE);
	//		BBE = next;
	//	}
	//}

	update();
}

bool Spine::_set(const StringName &p_name, const Variant &p_value)
{
	if (p_name == "skin")
	{
		set_skin(p_value);
		return true;
	}
	else if (p_name == "clear_tracks")
	{
		if ((bool)p_value)
		{
			clear();
			_change_notify();
		}
		return true;
	}
	
	return false;
}

bool Spine::_get(const StringName &p_name, Variant &r_ret) const
{
	if (p_name == "skin")
	{
		ERR_FAIL_COND_V(!skeleton, true);
		r_ret = get_skin();
		return true;
	}
	else if (p_name == "clear_tracks")
	{
		ERR_FAIL_COND_V(!skeleton, true);
		r_ret = false;
		return true;
	}

	return false;
}


void Spine::_get_property_list(List<PropertyInfo> *p_list) const
{
	if (skeleton != NULL)
	{
		String hint = skeleton->data->defaultSkin->name;
		if (skeleton->data->skinsCount > 0)
		{
			for (int i = 0; i < skeleton->data->skinsCount; ++i)
			{
				if (skeleton->data->skins[i]->name == skeleton->data->defaultSkin->name)
					continue;
				hint += ",";
				hint += skeleton->data->skins[i]->name;
			}
			p_list->push_back(PropertyInfo(Variant::STRING, "skin", PROPERTY_HINT_ENUM, hint, PROPERTY_USAGE_EDITOR | PROPERTY_USAGE_STORAGE));
		}
		if (!is_end())
		{
			p_list->push_back(PropertyInfo(Variant::BOOL, "clear_tracks", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR));
		}
	}
}

void Spine::_notification(int p_what) {
	switch (p_what)
	{
		case NOTIFICATION_PROCESS: {
			if (animation_process_mode == ANIMATION_PROCESS_FIXED)
				break;

			if (playing)
				_animation_process(get_process_delta_time());
		} break;
		case NOTIFICATION_PHYSICS_PROCESS:
		{
			if (animation_process_mode == ANIMATION_PROCESS_IDLE)
				break;

			if (playing)
				_animation_process(get_physics_process_delta_time());
		} break;
		case NOTIFICATION_DRAW:
		{
			_animation_draw();
		} break;
		case NOTIFICATION_PARENTED:
		case NOTIFICATION_UNPARENTED:
			_update_bounding_box();
			break;
	}
}

void Spine::set_resource(Ref<SpineSkeletonData> p_data)
{
	if (res == p_data)
		return;

	_spine_dispose(); // cleanup
	if (res.is_valid())
		res->unregister_owner(this);
	res = p_data;
		
	if (res.is_null())
	{
		_change_notify();
		
		return;
	}
		

	res->register_owner(this);
	
	_spine_create();
	_change_notify();
	update();
}

Ref<SpineSkeletonData> Spine::get_resource()
{
	return res;
}

void Spine::resource_changed(Ref<Resource> p_res)
{
	if (dynamic_cast<SpineSkeletonData*>(*p_res))
	{
		res = p_res;
		_spine_dispose();
		_spine_create();
		_change_notify();
		update();
	}
	else if (dynamic_cast<SpineAnimationAttri*>(*p_res))
	{
		for (int i = 0; i < internal_animations.size(); ++i)
		{
			if (Ref<SpineAnimationAttri>(internal_animations[i]).ptr() == p_res.ptr())
			{
				internal_animations[i] = p_res;
				break;
			}
		}
	}	
	
	return;
}

PoolStringArray Spine::get_skins()
{
	ERR_FAIL_COND_V(!skeleton, PoolStringArray());

	PoolStringArray ret;
	for (int i = 0; i < skeleton->data->skinsCount; ++i)
	{
		ret.append(skeleton->data->skins[i]->name);
	}
	
	return ret;
}

bool Spine::has_skin(const String& p_skin)
{
	ERR_FAIL_COND_V(!skeleton, false);

	return spSkeletonData_findSkin(skeleton->data, p_skin.utf8().get_data()) != NULL;
}

String Spine::get_skin() const
{
	ERR_FAIL_COND_V(!skeleton, "");

	if (skeleton->skin)
		return skeleton->skin->name;
	else
		return skeleton->data->defaultSkin->name;
}

bool Spine::set_skin(const String& p_skin)
{
	ERR_FAIL_COND_V(!skeleton, false);
	
	if (p_skin == skeleton->data->defaultSkin->name || p_skin == "")
	{
		if (skeleton->skin == NULL)
			return true;
		spSkeleton_setSkin(skeleton, 0);
	}
	else if (has_skin(p_skin))
	{
		if (skeleton->skin && skeleton->skin->name == p_skin)
			return true;
		spSkeleton_setSkinByName(skeleton, String(p_skin).utf8().get_data());
	}
	else
		return false;
	spSkeleton_setSlotsToSetupPose(skeleton);
	spAnimationState_apply(state, skeleton);
	update();
	return true;
}

void Spine::add_internal_animations()
{
	for (int i = 0; i < internal_animations.size(); ++i)
	{
		add_animation_by_attribute(cast_to<SpineAnimationAttri>(internal_animations[i]));
	}
}

SpineAnimation* Spine::add_animation_by_attribute(Ref<SpineAnimationAttri> p_attribute)
{
	ERR_FAIL_COND_V(!state, nullptr);
	ERR_FAIL_COND_V(p_attribute.is_null(), nullptr);
	SpineAnimation* anim;
	if (p_attribute->get_animation_name() != "")
		anim = add_animation(p_attribute->get_animation_name(), p_attribute->get_track(), p_attribute->get_loop(), p_attribute->get_delay());
	else
		anim = add_empty_animation(p_attribute->get_track(), p_attribute->get_mix(), p_attribute->get_delay());
	if (!anim)
		return nullptr;
	if (p_attribute->is_use_mix())
		anim->set_mix_duration(p_attribute->get_mix());
	if (p_attribute->get_alpha() < 1.0)
		anim->set_alpha(p_attribute->get_alpha());
	if (p_attribute->get_start() > 0)
		anim->set_start(p_attribute->get_start());
	if (p_attribute->is_use_end())
		anim->set_end(p_attribute->get_end());
	if (p_attribute->get_speed_scale() != 1.0)
		anim->set_speed_scale(p_attribute->get_speed_scale());
	return anim;
}

SpineAnimation* Spine::add_animation(const String& p_name, bool p_loop, float p_delay, int p_track)
{
	AddAnimationData data = AddAnimationData();
	data.isAdd = true;
	data.anim = p_name;
	data.track = p_track;
	data.loop = p_loop;
	data.delay = p_delay;
	return _add_animation(data);
}

SpineAnimation* Spine::add_empty_animation(float p_mix, float p_delay, int p_track)
{
	AddAnimationData data = AddAnimationData();
	data.isAdd = true;
	data.anim = "";
	data.track = p_track;
	data.mix = p_mix;
	data.delay = p_delay;
	return _add_animation(data);
}

SpineAnimation* Spine::set_animation(const String& p_name, bool p_loop, int p_track)
{
	AddAnimationData data = AddAnimationData();
	data.isAdd = false;
	data.anim = p_name;
	data.track = p_track;
	data.loop = p_loop;
	return _add_animation(data);
}
SpineAnimation* Spine::set_empty_animation(float p_mix, int p_track)
{
	AddAnimationData data = AddAnimationData();
	data.isAdd = false;
	data.anim = "";
	data.track = p_track;
	data.mix = p_mix;
	return _add_animation(data);
}

SpineAnimation* Spine::_add_animation(const AddAnimationData& p_data)
{
	ERR_FAIL_COND_V(!state, nullptr);
	bool ended = is_end();
	SpineAnimation* ret = memnew(SpineAnimation);
	ret->state = state;
	spTrackEntry* trackEntry;
	if (p_data.isAdd)
	{
		if (p_data.anim == "")
			trackEntry = spAnimationState_addEmptyAnimationWithData(state, p_data.track, p_data.mix, p_data.delay, ret);
		else
			trackEntry = spAnimationState_addAnimationByNameWithData(state, p_data.track, p_data.anim.utf8().get_data(), p_data.loop, p_data.delay, ret);
	}
	else
	{
		if (p_data.anim == "")
			trackEntry = spAnimationState_setEmptyAnimationWithData(state, p_data.track, p_data.mix, ret);
		else
			trackEntry = spAnimationState_setAnimationByNameWithData(state, p_data.track, p_data.anim.utf8().get_data(), p_data.loop, ret);
	}
	
	if (!trackEntry)
	{
		memdelete(ret);
		return nullptr;
	}
	/* 需要显示clear tracks屬性 */
	if (ended)
	{
		_change_notify();
	}
	else
	{
		ret->trackEntry = trackEntry;
	}
	return ret;
}

bool Spine::has_animation(const String& p_name) const
{
	ERR_FAIL_COND_V(!skeleton, false);
	
	return spSkeletonData_findAnimation(skeleton->data, p_name.utf8().get_data());
}

PoolStringArray Spine::get_animations() const
{
	PoolStringArray ret;
	if (skeleton == NULL)
		return ret;
	for (int i = 0; i < skeleton->data->animationsCount; ++i)
	{
		ret.append(skeleton->data->animations[i]->name);
	}
	return ret;
}

real_t Spine::get_animation_duration(const String& p_name) const
{
	ERR_FAIL_COND_V(!skeleton, 0.0);
	spAnimation* anim = spSkeletonData_findAnimation(skeleton->data, p_name.utf8().get_data());
	ERR_FAIL_COND_V(!anim, 0.0);
	return anim->duration;
}

SpineAnimation* Spine::get_current_animation(int p_track)
{
	ERR_FAIL_COND_V(!state, nullptr);
	spTrackEntry* track = spAnimationState_getCurrent(state, p_track);
	if (!track || !track->userData)
		return nullptr;
	return (SpineAnimation*)(track->userData);
}

PoolStringArray Spine::get_bones() const
{
	ERR_FAIL_COND_V(!skeleton, PoolStringArray());
	PoolStringArray ret = PoolStringArray();
	for (int i = 0; i < skeleton->data->bonesCount; ++i)
	{
		ret.append(skeleton->data->bones[i]->name);
	}
	return ret;
}

bool Spine::has_bone(const String& p_name) const
{
	ERR_FAIL_COND_V(!skeleton, false);
	return spSkeletonData_findBone(skeleton->data, p_name.utf8().get_data());
}

Transform2D Spine::get_bone_local_transform(const String& p_name) const
{
	ERR_FAIL_COND_V(!state, Transform2D());
	spBone* bone = spSkeleton_findBone(skeleton, p_name.utf8().get_data());
	float rotationY = bone->rotation + 90 + bone->shearY;
	float a = COS_DEG(bone->rotation + bone->shearX) * bone->scaleX;
	float b = COS_DEG(rotationY) * bone->scaleY;
	float c = SIN_DEG(bone->rotation + bone->shearX) * bone->scaleX;
	float d = SIN_DEG(rotationY) * bone->scaleY;
	/*Transform2D ret = Transform2D(Math::atan2(c, d), Vector2(bone->x, -bone->y));
	ret.set_scale(Vector2(SQRT(a * a + c * c), SQRT(b * b + d * d)));*/
	Transform2D ret = Transform2D(a, b, c, d, bone->x, -bone->y);
	return ret;
}
Transform2D Spine::get_bone_world_transform(const String& p_name) const
{
	ERR_FAIL_COND_V(!state, Transform2D());
	spBone* bone = spSkeleton_findBone(skeleton, p_name.utf8().get_data());
	/*Transform2D ret = Transform2D(Math::atan2(bone->c, bone->d), Vector2(bone->worldX, -bone->worldY));
	ret.set_scale(Vector2(spBone_getWorldScaleX(bone), spBone_getWorldScaleY(bone)));*/
	Transform2D ret = Transform2D(bone->a, bone->b, bone->c, bone->d, bone->worldX, -bone->worldY);
	return ret;
}

PoolStringArray Spine::get_slots() const
{
	ERR_FAIL_COND_V(!skeleton, PoolStringArray());
	PoolStringArray ret = PoolStringArray();
	for (int i = 0; i < skeleton->data->slotsCount; ++i)
	{
		ret.append(skeleton->data->slots[i]->name);
	}
	return ret;
}

bool Spine::has_slot_attachment(const String& p_slot, const String& p_attachment) const
{
	ERR_FAIL_COND_V(!skeleton, false);
	return spSkeleton_getAttachmentForSlotName(skeleton, p_slot.utf8().get_data(), p_attachment.utf8().get_data());
}

PoolStringArray Spine::get_slot_attachments(const String& p_name) const
{
	ERR_FAIL_COND_V(!skeleton, PoolStringArray());
	int slotId = spSkeletonData_findSlotIndex(skeleton->data, p_name.utf8().get_data());
	ERR_FAIL_COND_V(slotId == -1, PoolStringArray());
	spSkin* skin;
	if (!skeleton->skin)
		skin = skeleton->data->defaultSkin;
	else
		skin = skeleton->skin;
	spSkinEntry* entry = spSkin_getAttachments(skin);
	PoolStringArray ret = PoolStringArray();
	while (entry)
	{
		if (entry->slotIndex == slotId)
		{
			ret.append(entry->name);
		}
		entry = entry->next;
	}
	return ret;
}

bool Spine::set_slot_attachment(const String& p_slot, const String& p_attachment)
{
	ERR_FAIL_COND_V(!skeleton, false);

	if (p_attachment == "")
		return spSkeleton_setAttachment(skeleton, p_slot.utf8().get_data(), 0);
	else
		return spSkeleton_setAttachment(skeleton, p_slot.utf8().get_data(), p_attachment.utf8().get_data());
}

String Spine::get_slot_attachment(const String& p_name) const
{
	ERR_FAIL_COND_V(!skeleton, "");
	spSlot* slot = spSkeleton_findSlot(skeleton, p_name.utf8().get_data());
	ERR_FAIL_COND_V(!slot, "");
	return slot->attachment->name;
}

bool Spine::set_slot_color(const String& p_name, const Color& p_color)
{
	ERR_FAIL_COND_V(!skeleton, false);
	spSlot* slot = spSkeleton_findSlot(skeleton, p_name.utf8().get_data());
	ERR_FAIL_COND_V(!slot, false);

	spColor_setFromFloats(&slot->color, p_color.r, p_color.g, p_color.b, p_color.a);
	update();
	return true;
}
Color Spine::get_slot_color(const String& p_name) const
{
	ERR_FAIL_COND_V(!skeleton, Color());
	spSlot* slot = spSkeleton_findSlot(skeleton, p_name.utf8().get_data());
	ERR_FAIL_COND_V(!slot, Color());

	Color ret = Color(slot->color.r, slot->color.g, slot->color.b, slot->color.a);
	return ret;
}

bool Spine::is_end() const
{
	if (skeleton == NULL)
		return true;

	for (int i = 0; i < state->tracksCount; ++i)
	{
		if (spAnimationState_getCurrent(state, i) != NULL)
			return false;
	}
	return true;
}

void Spine::clear(int p_track)
{
	ERR_FAIL_COND(!state);
	if (p_track < 0)
		spAnimationState_clearTracks(state);
	else
		spAnimationState_clearTrack(state, p_track);
}

PoolIntArray Spine::get_tracks() const
{
	ERR_FAIL_COND_V(!state, PoolIntArray());
	PoolIntArray ret = PoolIntArray();
	for (int i = 0; i < state->tracksCount; ++i)
	{
		if (state->tracks[i])
			ret.append(state->tracks[i]->trackIndex);
	}
	return ret;
}

void Spine::reset()
{
	if (skeleton == NULL) {
		return;
	}
	spSkeleton_setToSetupPose(skeleton);
	_animation_process(0.0);
}

void Spine::seek(float p_delta) {
	ERR_FAIL_COND(state == NULL);
	_animation_process(p_delta);
}

float Spine::tell() const
{
	ERR_FAIL_COND_V(skeleton == NULL, 0.0);
	return skeleton->time;
}

String Spine::get_attachment_node_bone(const Object* p_node) const
{
	if (!p_node->has_meta("spine_meta"))
		return "";
	AttachmentNode* info = (AttachmentNode *)((uint64_t)p_node->get_meta("spine_meta"));
	return info->bone->data->name;
}

Array Spine::get_attachment_nodes(const String& p_bone_name) const
{
	Array ret = Array();
	for (const AttachmentNodes::Element* E = attachment_nodes.front(); E; E = E->next())
	{
		if (E->get().bone->data->name == p_bone_name)
		{
			ret.append(E->get().ref->get_ref());
		}
	}
	return ret;
}

bool Spine::add_attachment_node(const String &p_bone_name, Object* p_node, const Transform2D& p_transform, int p_flag)
{
	ERR_FAIL_COND_V(skeleton == NULL, false);
	spBone *bone = spSkeleton_findBone(skeleton, p_bone_name.utf8().get_data());
	ERR_FAIL_COND_V(bone == NULL, false);
	ERR_FAIL_COND_V(p_node == NULL, false);
	Node2D *node = Object::cast_to<Node2D>(p_node);
	ERR_FAIL_COND_V(node == NULL, false);

	if (p_node->has_meta("spine_meta")) {

		AttachmentNode *info = (AttachmentNode *)((uint64_t)p_node->get_meta("spine_meta"));
		if (info->bone != bone) {
			// add to different bone, remove first
			remove_attachment_node(p_node);
		} else {
			// add to same bone, update params
			info->ofs = p_transform.get_origin();
			info->scale = p_transform.get_scale();
			info->rot = p_transform.get_rotation();
			info->flag = p_flag;
			return true;
		}
	}
	attachment_nodes.push_back(AttachmentNode());
	AttachmentNode &info = attachment_nodes.back()->get();
	info.E = attachment_nodes.back();
	info.bone = bone;
	info.ref = memnew(WeakRef);
	info.ref->set_obj(p_node);
	info.ofs = p_transform.get_origin();
	info.scale = p_transform.get_scale();
	info.rot = p_transform.get_rotation();
	info.flag = p_flag;
	p_node->set_meta("spine_meta", (uint64_t)&info);

	return true;
}

bool Spine::remove_attachment_node(Object* p_node)
{
	ERR_FAIL_COND_V(p_node == NULL, false);

	if (!p_node->has_meta("spine_meta"))
		return false;

	AttachmentNode *info = (AttachmentNode *)((uint64_t)p_node->get_meta("spine_meta"));
	p_node->remove_meta("spine_meta");
	memdelete(info->ref);
	attachment_nodes.erase(info->E);

	return true;
}

int Spine::get_attachment_node_flag(const Object* p_node) const
{
	ERR_FAIL_COND_V(p_node == NULL, 0);
	ERR_FAIL_COND_V(!p_node->has_meta("spine_meta"), 0);

	AttachmentNode *info = (AttachmentNode *)((uint64_t)p_node->get_meta("spine_meta"));
	return info->flag;
}

Vector2 Spine::get_attachment_node_offset(const Object* p_node) const
{
	ERR_FAIL_COND_V(p_node == NULL, Vector2());
	ERR_FAIL_COND_V(!p_node->has_meta("spine_meta"), Vector2());

	AttachmentNode *info = (AttachmentNode *)((uint64_t)p_node->get_meta("spine_meta"));
	return info->ofs;
}

float Spine::get_attachment_node_rotation(const Object* p_node) const
{
	ERR_FAIL_COND_V(p_node == NULL, 0.0);
	ERR_FAIL_COND_V(!p_node->has_meta("spine_meta"), 0.0);

	AttachmentNode *info = (AttachmentNode *)((uint64_t)p_node->get_meta("spine_meta"));
	return info->rot;
}

Vector2 Spine::get_attachment_node_scale(const Object* p_node) const
{
	ERR_FAIL_COND_V(p_node == NULL, Vector2());
	ERR_FAIL_COND_V(!p_node->has_meta("spine_meta"), Vector2());

	AttachmentNode *info = (AttachmentNode *)((uint64_t)p_node->get_meta("spine_meta"));
	return info->scale;
}

void Spine::set_attachment_node_flag(Object* p_node, int p_flag)
{
	ERR_FAIL_COND(p_node == NULL);
	ERR_FAIL_COND(!p_node->has_meta("spine_meta"));

	AttachmentNode *info = (AttachmentNode *)((uint64_t)p_node->get_meta("spine_meta"));
	info->flag = p_flag;
	_update_attachment_node();
}

void Spine::set_attachment_node_offset(Object* p_node, Vector2 p_offset)
{
	ERR_FAIL_COND(p_node == NULL);
	ERR_FAIL_COND(!p_node->has_meta("spine_meta"));

	AttachmentNode *info = (AttachmentNode *)((uint64_t)p_node->get_meta("spine_meta"));
	info->ofs = p_offset;
	_update_attachment_node();
}

void Spine::set_attachment_node_rotation(Object* p_node, real_t p_rotation)
{
	ERR_FAIL_COND(p_node == NULL);
	ERR_FAIL_COND(!p_node->has_meta("spine_meta"));

	AttachmentNode *info = (AttachmentNode *)((uint64_t)p_node->get_meta("spine_meta"));
	info->rot = p_rotation;
	_update_attachment_node();
}

void Spine::set_attachment_node_scale(Object* p_node, Vector2 p_scale)
{
	ERR_FAIL_COND(p_node == NULL);
	ERR_FAIL_COND(!p_node->has_meta("spine_meta"));

	AttachmentNode *info = (AttachmentNode *)((uint64_t)p_node->get_meta("spine_meta"));
	info->scale = p_scale;
	_update_attachment_node();
}

Array Spine::get_internal_animations() const
{
	return internal_animations;
}

void Spine::set_internal_animations(Array p_animations)
{
	for (int i = 0; i < internal_animations.size(); ++i)
	{
		Ref<SpineAnimationAttri>(internal_animations[i])->unregister_owner(this);
	}
	internal_animations.resize(p_animations.size());

	for (int i = 0; i < internal_animations.size(); ++i)
	{
		internal_animations[i] = p_animations[i];
		if (!internal_animations[i].is_ref() || Ref<SpineAnimationAttri>(internal_animations[i]).is_null())
		{
			Ref<SpineAnimationAttri> ref(memnew(SpineAnimationAttri));
			if (skeleton != NULL)
				ref->set_animation_name(skeleton->data->animations[rand() % skeleton->data->animationsCount]->name);
			internal_animations[i] = ref;
			//print_line("internal animation[i] is null!");
		}
		Ref<SpineAnimationAttri>(internal_animations[i])->register_owner(this);
	}
	_change_notify("internal_animations");
}

void Spine::set_animation_process_mode(AnimationProcessMode p_mode) {

	if (animation_process_mode == p_mode)
		return;

	if (playing)
	{
		if (animation_process_mode == AnimationProcessMode::ANIMATION_PROCESS_FIXED)
		{
			set_physics_process(false);
			set_process(true);
		}
		else
		{
			set_process(false);
			set_physics_process(true);
		}
	}	
	animation_process_mode = p_mode;
	_change_notify("process_mode");
}

Spine::AnimationProcessMode Spine::get_animation_process_mode() const
{
	return animation_process_mode;
}

void Spine::set_playing(bool p_playing)
{
	if (playing == p_playing)
		return;
	if (p_playing && is_end())
	{
		if (auto_play && internal_animations.size() > 0)
		{
			add_internal_animations();
			print_line("added internal animations");
		}
		else
			return;
	}
	playing = p_playing;
	
	if (animation_process_mode == AnimationProcessMode::ANIMATION_PROCESS_FIXED)
		set_physics_process(playing);
	else
		set_process(playing);
	_change_notify();
}

bool Spine::is_playing() const
{
	return playing;
}

void Spine::set_auto_play(bool p_auto)
{
	auto_play = p_auto;
	_change_notify("auto_play");
}

bool Spine::is_auto_play() const
{
	return auto_play;
}

void Spine::set_show_bone(bool p_show)
{
	if (p_show == show_bone)
		return;
	show_bone = p_show;
	update();
}

bool Spine::is_show_bone() const
{
	return show_bone;
}

void Spine::set_speed_scale(bool p_scale)
{
	speed_scale = p_scale;
	if (state != NULL)
		state->timeScale = speed_scale;
	_change_notify("speed_scale");
}

float Spine::get_speed_scale() const
{
	return speed_scale;
}

void Spine::set_spine_offset(Vector2 p_offset)
{
	spine_offset = p_offset;
	if (skeleton != NULL)
	{
		skeleton->x = spine_offset.x;
		skeleton->y = spine_offset.y;
		spSkeleton_updateWorldTransform(skeleton);
		update();
	}
	_change_notify("spine_offset");
}

Vector2 Spine::get_spine_offset() const
{
	return spine_offset;
}

void Spine::set_spine_scale(Vector2 p_scale)
{
	spine_scale = p_scale;
	if (skeleton != NULL)
	{
		skeleton->scaleX = spine_scale.x;
		skeleton->scaleY = spine_scale.y;
		spSkeleton_updateWorldTransform(skeleton);
		update();
	}
	_change_notify("spine_scale");
}

Vector2 Spine::get_spine_scale() const
{
	return spine_scale;
}

void Spine::set_default_mix(float p_mix)
{
	if (default_mix == p_mix)
		return;
	default_mix = p_mix;
	if (state != NULL)
	{
		state->data->defaultMix = default_mix;
	}
	_change_notify("default_mix");
}
float Spine::get_default_mix() const
{
	return default_mix;
}

bool Spine::set_mix(const String& p_from, const String& p_to, real_t p_value)
{
	ERR_FAIL_COND_V(!state, false);
	return spAnimationStateData_setMixByName(state->data, p_from.utf8().get_data(), p_to.utf8().get_data(), p_value);
}

real_t Spine::get_mix(const String& p_from, const String& p_to) const
{
	ERR_FAIL_COND_V(!state, 0.0);
	spAnimation* from = spSkeletonData_findAnimation(state->data->skeletonData, p_from.utf8().get_data());
	ERR_FAIL_COND_V(!from, 0.0);
	spAnimation* to = spSkeletonData_findAnimation(state->data->skeletonData, p_to.utf8().get_data());
	ERR_FAIL_COND_V(!to, 0.0);
	return spAnimationStateData_getMix(state->data, from, to);
}

//void Spine::set_collision_whitelist(bool p_value)
//{
//	if (collision_whitelist == p_value)
//		return;
//	collision_whitelist = p_value;
//	_change_notify("collision_whitelist");
//}
//
//bool Spine::is_collision_whitelist() const
//{
//	return collision_whitelist;
//}

//void Spine::set_collision_list(const PoolStringArray& p_list)
//{
//
//}
//
//PoolStringArray Spine::get_collision_list() const
//{
//
//}

void Spine::set_use_bounding_box(bool p_value)
{
	if (use_bounding_box == p_value)
		return;

	use_bounding_box = p_value;
	if (skeleton != NULL)
	{
		if (use_bounding_box)
			_update_bounding_box();
		else
			_bounding_boxs.clear();
	}
	
	_change_notify("use_bounding_box");
}

bool Spine::is_use_bounding_box() const
{
	return use_bounding_box;
}

void Spine::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_resource", "value"), &Spine::set_resource);
	ClassDB::bind_method(D_METHOD("get_resource"), &Spine::get_resource);
	//ClassDB::bind_method(D_METHOD("resource_changed", "spine_skeleton_data"), &Spine::resource_changed);

	ClassDB::bind_method(D_METHOD("has_skin", "name"), &Spine::has_skin);
	ClassDB::bind_method(D_METHOD("get_skins"), &Spine::get_skins);
	ClassDB::bind_method(D_METHOD("set_skin", "value"), &Spine::set_skin);
	ClassDB::bind_method(D_METHOD("get_skin"), &Spine::get_skin);

	ClassDB::bind_method(D_METHOD("set_internal_animations", "animations"), &Spine::set_internal_animations);
	ClassDB::bind_method(D_METHOD("get_internal_animations"), &Spine::get_internal_animations);
	ClassDB::bind_method(D_METHOD("set_animation_process_mode", "value"), &Spine::set_animation_process_mode);
	ClassDB::bind_method(D_METHOD("get_animation_process_mode"), &Spine::get_animation_process_mode);
	ClassDB::bind_method(D_METHOD("set_playing", "value"), &Spine::set_playing);
	ClassDB::bind_method(D_METHOD("is_playing"), &Spine::is_playing);
	ClassDB::bind_method(D_METHOD("set_auto_play", "value"), &Spine::set_auto_play);
	ClassDB::bind_method(D_METHOD("is_auto_play"), &Spine::is_auto_play);
	ClassDB::bind_method(D_METHOD("set_show_bone", "value"), &Spine::set_show_bone);
	ClassDB::bind_method(D_METHOD("is_show_bone"), &Spine::is_show_bone);
	ClassDB::bind_method(D_METHOD("set_speed_scale", "value"), &Spine::set_speed_scale);
	ClassDB::bind_method(D_METHOD("get_speed_scale"), &Spine::get_speed_scale);
	ClassDB::bind_method(D_METHOD("set_spine_offset", "value"), &Spine::set_spine_offset);
	ClassDB::bind_method(D_METHOD("get_spine_offset"), &Spine::get_spine_offset);
	ClassDB::bind_method(D_METHOD("set_spine_scale", "value"), &Spine::set_spine_scale);
	ClassDB::bind_method(D_METHOD("get_spine_scale"), &Spine::get_spine_scale);
	ClassDB::bind_method(D_METHOD("set_default_mix", "value"), &Spine::set_default_mix);
	ClassDB::bind_method(D_METHOD("get_default_mix"), &Spine::get_default_mix);
	ClassDB::bind_method(D_METHOD("set_mix", "from", "to", "mix"), &Spine::set_mix);
	ClassDB::bind_method(D_METHOD("get_mix", "from", "to"), &Spine::get_mix);
	//ClassDB::bind_method(D_METHOD("set_collision_whitelist", "value"), &Spine::set_collision_whitelist);
	//ClassDB::bind_method(D_METHOD("is_collision_whitelist"), &Spine::is_collision_whitelist);
	ClassDB::bind_method(D_METHOD("set_use_bounding_box", "value"), &Spine::set_use_bounding_box);
	ClassDB::bind_method(D_METHOD("is_use_bounding_box"), &Spine::is_use_bounding_box);
	
	ClassDB::bind_method(D_METHOD("add_internal_animations"), &Spine::add_internal_animations);
	ClassDB::bind_method(D_METHOD("add_animation_by_attribute", "attribute"), &Spine::add_animation_by_attribute);
	ClassDB::bind_method(D_METHOD("add_animation", "name", "loop", "delay", "track"), &Spine::add_animation, DEFVAL(0));
	ClassDB::bind_method(D_METHOD("add_empty_animation", "mix", "delay", "track"), &Spine::add_empty_animation, DEFVAL(0));
	ClassDB::bind_method(D_METHOD("set_animation", "name", "loop", "track"), &Spine::set_animation, DEFVAL(0));
	ClassDB::bind_method(D_METHOD("set_empty_animation", "mix", "track"), &Spine::set_empty_animation, DEFVAL(0));
	ClassDB::bind_method(D_METHOD("has_animation", "name"), &Spine::has_animation);
	ClassDB::bind_method(D_METHOD("get_animations"), &Spine::get_animations);

	ClassDB::bind_method(D_METHOD("get_animation_duration", "name"), &Spine::get_animation_duration);
	ClassDB::bind_method(D_METHOD("get_current_animation", "track"), &Spine::get_current_animation, DEFVAL(0));

	ClassDB::bind_method(D_METHOD("get_bones"), &Spine::get_bones);
	ClassDB::bind_method(D_METHOD("has_bone", "name"), &Spine::has_bone);
	ClassDB::bind_method(D_METHOD("get_bone_local_transform", "name"), &Spine::get_bone_local_transform);
	ClassDB::bind_method(D_METHOD("get_bone_world_transform", "name"), &Spine::get_bone_world_transform);

	ClassDB::bind_method(D_METHOD("get_slots"), &Spine::get_slots);
	ClassDB::bind_method(D_METHOD("get_slot_attachments", "name"), &Spine::get_slot_attachments);
	ClassDB::bind_method(D_METHOD("has_slot_attachment", "slot", "attachment"), &Spine::has_slot_attachment);
	ClassDB::bind_method(D_METHOD("set_slot_attachment", "slot", "attachment"), &Spine::set_slot_attachment);
	ClassDB::bind_method(D_METHOD("get_slot_attachment", "name"), &Spine::get_slot_attachment);
	ClassDB::bind_method(D_METHOD("set_slot_color", "name", "color"), &Spine::set_slot_color);
	ClassDB::bind_method(D_METHOD("get_slot_color", "name"), &Spine::get_slot_color);

	//ClassDB::bind_method(D_METHOD("is_end"), &Spine::is_end);
	ClassDB::bind_method(D_METHOD("clear", "track"), &Spine::clear, DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("get_tracks"), &Spine::get_tracks);

	ClassDB::bind_method(D_METHOD("get_attachment_node_bone", "node"), &Spine::get_attachment_node_bone);
	ClassDB::bind_method(D_METHOD("get_attachment_nodes", "name"), &Spine::get_attachment_nodes);
	ClassDB::bind_method(D_METHOD("add_attachment_node", "name", "node", "transform", "flag"), &Spine::add_attachment_node, DEFVAL(Transform2D()), DEFVAL(ATTACHMENT_NODE_FLAG_ALL));
	ClassDB::bind_method(D_METHOD("remove_attachment_node", "node"), &Spine::remove_attachment_node);
	ClassDB::bind_method(D_METHOD("get_attachment_node_flag", "node"), &Spine::get_attachment_node_flag);
	ClassDB::bind_method(D_METHOD("get_attachment_node_offset", "node"), &Spine::get_attachment_node_offset);
	ClassDB::bind_method(D_METHOD("get_attachment_node_rotation", "node"), &Spine::get_attachment_node_rotation);
	ClassDB::bind_method(D_METHOD("get_attachment_node_scale", "node"), &Spine::get_attachment_node_scale);
	ClassDB::bind_method(D_METHOD("set_attachment_node_flag", "node", "flag"), &Spine::set_attachment_node_flag);
	ClassDB::bind_method(D_METHOD("set_attachment_node_offset", "node", "offset"), &Spine::set_attachment_node_offset);
	ClassDB::bind_method(D_METHOD("set_attachment_node_rotation", "node", "rotation"), &Spine::set_attachment_node_rotation);
	ClassDB::bind_method(D_METHOD("set_attachment_node_scale", "node", "scale"), &Spine::set_attachment_node_scale);
	ClassDB::bind_method(D_METHOD("seek", "delta"), &Spine::seek);
	ClassDB::bind_method(D_METHOD("tell"), &Spine::tell);


	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "resource", PROPERTY_HINT_RESOURCE_TYPE, "SpineSkeletonData", PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_EDITOR), "set_resource", "get_resource");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "internal_animations", PROPERTY_HINT_TYPE_STRING, "17/17:SpineAnimationAttri"), "set_internal_animations", "get_internal_animations");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "process_mode", PROPERTY_HINT_ENUM, "Fixed,Idle"), "set_animation_process_mode", "get_animation_process_mode");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "playing"), "set_playing", "is_playing");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "auto_play"), "set_auto_play", "is_auto_play");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "show_bone"), "set_show_bone", "is_show_bone");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "speed_scale"), "set_speed_scale", "get_speed_scale");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "offset"), "set_spine_offset", "get_spine_offset");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "scale"), "set_spine_scale", "get_spine_scale");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "default_mix"), "set_default_mix", "get_default_mix");
	//ADD_PROPERTY(PropertyInfo(Variant::BOOL, "collision_whitelist"), "set_collision_whitelist", "is_collision_whitelist");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_bounding_box"), "set_use_bounding_box", "is_use_bounding_box");


	ADD_SIGNAL(MethodInfo("animation_started", PropertyInfo(Variant::OBJECT, "animation")));
	ADD_SIGNAL(MethodInfo("animation_interrupted", PropertyInfo(Variant::OBJECT, "animation")));
	ADD_SIGNAL(MethodInfo("animation_completed", PropertyInfo(Variant::OBJECT, "animation")));
	ADD_SIGNAL(MethodInfo("animation_ended", PropertyInfo(Variant::OBJECT, "animation")));
	ADD_SIGNAL(MethodInfo("animation_event", PropertyInfo(Variant::OBJECT, "animation"), PropertyInfo(Variant::STRING, "event"), PropertyInfo(Variant::DICTIONARY, "params")));
	ADD_SIGNAL(MethodInfo("ended"));


	BIND_ENUM_CONSTANT(ANIMATION_PROCESS_FIXED);
	BIND_ENUM_CONSTANT(ANIMATION_PROCESS_IDLE);

	BIND_ENUM_CONSTANT(ATTACHMENT_NODE_FLAG_OFFSET);
	BIND_ENUM_CONSTANT(ATTACHMENT_NODE_FLAG_SCALE);
	BIND_ENUM_CONSTANT(ATTACHMENT_NODE_FLAG_ROTATION);
	BIND_ENUM_CONSTANT(ATTACHMENT_NODE_FLAG_ALL);
}

#ifdef TOOLS_ENABLED
Rect2 Spine::_edit_get_rect() const
{
	if (skeleton == NULL)
		return Node2D::_edit_get_rect();

	//float minX = 65535, minY = 65535, maxX = -65535, maxY = -65535;
	Rect2 ret;
	bool attached = false;
	for (int i = 0; i < skeleton->slotsCount; ++i) {

		spSlot *slot = skeleton->slots[i];
		if (!slot->attachment) continue;
		int verticesCount;
		if (slot->attachment->type == SP_ATTACHMENT_REGION) {
			spRegionAttachment *attachment = (spRegionAttachment *)slot->attachment;
			spRegionAttachment_computeWorldVertices(attachment, slot->bone, world_verts.ptrw(), 0, 2);
			verticesCount = 8;
		} else if (slot->attachment->type == SP_ATTACHMENT_MESH) {
			spMeshAttachment *mesh = (spMeshAttachment *)slot->attachment;
			//if (mesh->super.verticesCount * 2 > world_verts.size())
			//	world_verts.resize(mesh->super.verticesCount * 2);
			spVertexAttachment_computeWorldVertices(SUPER(mesh), slot, 0, mesh->super.worldVerticesLength, world_verts.ptrw(), 0, 2);
			verticesCount = ((spVertexAttachment *)mesh)->worldVerticesLength;
		} else
			continue;

		if (!attached)
		{
			ret.position = Vector2(world_verts[0], world_verts[1]);
			attached = true;
		}
		

		for (int ii = 0; ii < verticesCount; ii += 2) {
			//float x = world_verts[ii] * 1, y = world_verts[ii + 1] * 1;
			ret.expand_to(Vector2(world_verts[ii], world_verts[ii + 1]));
			//minX = MIN(minX, x);
			//minY = MIN(minY, y);
			//maxX = MAX(maxX, x);
			//maxY = MAX(maxY, y);
		}
	}
	ret.position.y = -ret.position.y - ret.size.y;
	//int h = maxY - minY;
	//return attached ? Rect2(minX, -minY - h, maxX - minX, h) : Node2D::_edit_get_rect();
	return attached ? ret : Node2D::_edit_get_rect();
}

bool Spine::_edit_use_rect() const {
	return skeleton != NULL;
}
#endif	// TOOLS_ENABLED

void Spine::_update_verties_count()
{
	ERR_FAIL_COND(skeleton == NULL);

	int verties_count = 0;
	for (int i = 0, n = skeleton->slotsCount; i < n; i++) {

		spSlot *slot = skeleton->drawOrder[i];
		if (!slot->attachment)
			continue;

		switch (slot->attachment->type)
		{
			case SP_ATTACHMENT_MESH:
			case SP_ATTACHMENT_LINKED_MESH:
			case SP_ATTACHMENT_BOUNDING_BOX:
				verties_count = MAX(verties_count, ((spVertexAttachment *)slot->attachment)->verticesCount + 1);
				break;
			default:
				continue;
		}
	}

	if (verties_count > world_verts.size()) {
		world_verts.resize(verties_count);
		memset(world_verts.ptrw(), 0, world_verts.size() * sizeof(float));
	}
}

void Spine::_update_attachment_node()
{
	for (AttachmentNodes::Element *E = attachment_nodes.front(); E; )
	{
		AttachmentNode &info = E->get();
		WeakRef *ref = info.ref;
		Object *obj = ref->get_ref();
		Node2D *node = cast_to<Node2D>(obj);
		if (obj == NULL || node == NULL) {

			AttachmentNodes::Element *NEXT = E->next();
			memdelete(info.ref);
			attachment_nodes.erase(E);
			E = NEXT;
			if (E == NULL)
				break;
			continue;
		}
		spBone *bone = info.bone;
		if (info.flag & ATTACHMENT_NODE_FLAG_OFFSET)
			node->call("set_position", Vector2(bone->worldX + bone->skeleton->x, -bone->worldY + bone->skeleton->y) + info.ofs);
		else
			node->call("set_position", info.ofs);
		if (info.flag & ATTACHMENT_NODE_FLAG_SCALE)
			node->call("set_scale", Vector2(spBone_getWorldScaleX(bone), spBone_getWorldScaleY(bone)) * info.scale);
		else
			node->call("set_scale", info.scale);
		if (info.flag & ATTACHMENT_NODE_FLAG_ROTATION)
			node->call("set_rotation", Math::atan2(bone->c, bone->d) + Math::deg2rad(info.rot));
		else
			node->call("set_rotation", Math::deg2rad(info.rot));
		E = E->next();
	}
}

void Spine::_update_bounding_box()
{
	if (use_bounding_box)
	{
		spSlot* slot;
		CollisionPolygon2D* collision;
		BoundingBox::Element* E;
		BoundingBox::Element* next;
		E = _bounding_boxs.front();

		if (get_parent() && ClassDB::is_parent_class(get_parent()->get_class(), "CollisionObject2D"))
		{
			for (int i = 0; i < skeleton->slotsCount; ++i)
			{
				slot = skeleton->slots[i];
				if (!slot->attachment)
					continue;

				if (slot->attachment->type == SP_ATTACHMENT_BOUNDING_BOX)
				{
					if (!E)
					{
						_bounding_boxs.push_back(Polygon());
						E = _bounding_boxs.back();
					}
					collision = E->get().get_polygon();
					if (!collision)
					{
						collision = memnew(CollisionPolygon2D);
						E->get().set_polygon(collision);
						collision->set_name(slot->data->name);
						get_parent()->call_deferred("add_child", (Object*)collision);
					}
					else
					{
						collision->set_name(slot->data->name);
						if (get_parent() != collision->get_parent())
						{
							if (collision->get_parent())
								collision->get_parent()->call_deferred("remove_child", collision);
							get_parent()->call_deferred("add_child", (Node*)collision);
						}
					}

					spBoundingBoxAttachment* attachment = SUB_CAST(spBoundingBoxAttachment, slot->attachment);
					spVertexAttachment_computeWorldVertices(SUPER(attachment), slot, 0, attachment->super.verticesCount, world_verts.ptrw(), 0, 2);
					Vector<Vector2> temp = Vector<Vector2>();
					temp.resize(attachment->super.verticesCount / 2);
					for (int j = 0; j < temp.size(); ++j)
					{
						temp.set(j, Vector2(world_verts[j * 2], -world_verts[j * 2 + 1]));
					}
					collision->set_polygon(temp);
					E = E->next();
				}
			}
		}

		while (E)
		{
			next = E->next();
			_bounding_boxs.erase(E);
			E = next;
		}
	}
}

//bool PoolStringArray_has(const PoolStringArray& p_array, const String& p_target)
//{
//	for (int i = 0; i < p_array.size(); ++i)
//	{
//		if (p_array[i] == p_target)
//			return true;
//	}
//	return false;
//}

//void Spine::_update_polygons()
//{
//	if (skeleton == NULL)
//		return;
//	int loopTime;
//	if (collision_whitelist)
//	{
//		loopTime = collision_list.size();
//	}
//	else
//	{
//		loopTime = skeleton->slotsCount;
//	}
//
//	Polygon::Element* e = _polygons.front();
//	for (int i = 0; i < collision_list.size(); ++i)
//	{
//		spSlot* slot;
//		if (collision_whitelist)
//		{
//			slot = spSkeleton_findSlot(skeleton, collision_list[i].utf8().get_data());
//			if (slot == NULL)
//				continue;
//		}
//		else
//		{
//			slot = skeleton->slots[i];
//			if (PoolStringArray_has(collision_list, slot->data->name))
//				continue;
//		}
//		
//
//		Object* obj;
//		if (!e)
//		{
//			_polygons.push_back(WeakRef());
//			e = _polygons.back();
//		}
//		obj = e->get().get_ref();
//		if (!obj)
//		{
//			obj = memnew(CollisionPolygon2D);
//			e->get().set_obj(obj);
//			add_child(cast_to<CollisionPolygon2D>(obj));
//		}
//		obj->set_meta("spine_meta", (uint64_t)slot);
//		e = e->next;
//	}
//	Polygon::Element* next;
//	while (e)
//	{
//		Object* obj = e->get().get_ref();
//		if (obj)
//		{
//			obj->call_deferred("free");
//		}
//		next = e->next;
//		_polygons.erase(e);
//		e = next;
//	}
//	_animation_process(0.0);
//}

Spine::Spine() :
		batcher(this), skeleton(nullptr), state(nullptr), animation_process_mode(ANIMATION_PROCESS_FIXED)
		, playing(false), auto_play(false), show_bone(false), speed_scale(1.0)
		, spine_offset(0.0, 0.0), spine_scale(1.0, 1.0), default_mix(0.0)/*, collision_whitelist(true)*/
		, use_bounding_box(true)
{
	res = RES();
	//world_verts.resize(1000); // Max number of vertices per mesh.
	world_verts.resize(8);
	memset(world_verts.ptrw(), 0, world_verts.size() * sizeof(float));
}

Spine::~Spine()
{
	// cleanup
	for (int i = 0; i < internal_animations.size(); ++i)
	{
		Ref<SpineAnimationAttri>(internal_animations[i])->unregister_owner(this);
	}
	_spine_dispose();
	if (res.is_valid())
		res->unregister_owner(this);
	res.unref();
}

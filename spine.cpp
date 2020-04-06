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
//#include "core/io/resource_loader.h"
//#include "core/version.h"
//#include "scene/2d/collision_object_2d.h"
//#include "scene/resources/convex_polygon_shape_2d.h"
//#include <core/engine.h>
#include <spine/extension.h>
#include <spine/spine.h>
#include "core/array.h"
#include "scene/resources/convex_polygon_shape_2d.h"
//#include "core/math/a_star.h"
//#include <core/method_bind_ext.gen.inc>


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
	//if (!animation)
	//	print_line("callback:animation is null");
	switch (p_type) {
		case SP_ANIMATION_START:
			if (animation)
			{
				animation->trackEntry = p_track;
				//print_line(String(animation->get_name()) + " started!");
			}
			//else
				//print_line("unknow animation started!");
			emit_signal("animation_started", animation);
			if (animation)
				animation->emit_signal("started");
			break;
		case SP_ANIMATION_INTERRUPT:
			//if (animation)
			//	print_line(String(animation->get_name()) + " interrupted!");
			//else
			//	print_line("unknow animation interrupted!");
			emit_signal("animation_interrupted", animation);
			if (animation)
				animation->emit_signal("interrupted");
			break;
		case SP_ANIMATION_COMPLETE:
			//if (animation)
			//	print_line(String(animation->get_name()) + " completed!");
			//else
			//	print_line("unknow animation completed!");
			emit_signal("animation_completed", animation);

			if (animation)
				animation->emit_signal("completed");
			if (!p_track->loop && p_track->next == NULL)
			{
				clear_track(p_track->trackIndex);
				//print_line("cleared track!");
			}
			break;
		case SP_ANIMATION_END:
			//if (animation)
			//	print_line(String(animation->get_name()) + " ended!");
			//else
			//	print_line("unknow animation ended!");
			emit_signal("animation_ended", animation);

			if (animation)
				animation->emit_signal("ended");

			if (playing && is_end())
			{
				if (!auto_play || internal_animations.size() == 0)
				{
					set_playing(false);
					emit_signal("ended");
					//_change_notify();
				}
			}
			break;
		case SP_ANIMATION_EVENT:
			//if (animation)
			//	print_line(String(animation->get_name()) + " event!");
			//else
			//	print_line("unknow animation event!");
			emit_signal("animation_event", animation, p_event->data->name, p_event->intValue, p_event->floatValue, p_event->stringValue ? p_event->stringValue : "");
			if (animation)
				animation->emit_signal("event", p_event->data->name, p_event->intValue, p_event->floatValue, p_event->stringValue ? p_event->stringValue : "");
			break;
		case SP_ANIMATION_DISPOSE:
			//if (animation)
			//	print_line(String(animation->get_name()) + " disposed!");
			//else
			//	print_line("unknow animation disposed!");
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

	//if (playing) {
	//	// stop first
	//	stop();
	//}

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
	
	//root_bone = skeleton->bones[0];

	state = spAnimationState_create(spAnimationStateData_create(skeleton->data));
	state->rendererObject = this;
	state->timeScale = speed_scale;
	state->data->defaultMix = default_mix;

	state->listener = spine_animation_callback;

	_update_verties_count();

	/*if (skin != "")
	set_skin(skin);
	if (current_animation != "[stop]")
	play(current_animation, 1, loop);
	else*/
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

//void Spine::_on_fx_draw() {
//
//	if (skeleton == NULL)
//		return;
//	fx_batcher.reset();
//	RID eci = fx_node->get_canvas_item();
//	// VisualServer::get_singleton()->canvas_item_add_set_blend_mode(eci, VS::MaterialBlendMode(fx_node->get_blend_mode()));
//	fx_batcher.flush();
//}
//
void Spine::_animation_draw() {

	if (skeleton == NULL)
		return;

	//spColor_setFromFloats(&skeleton->color, modulate.r, modulate.g, modulate.b, modulate.a);

	//int additive = 0;
	//int fx_additive = 0;
	Color color;
	const float *uvs = NULL;
	int verties_count = 0;
	unsigned short *triangles = NULL;
	int triangles_count = 0;
	float r = 0, g = 0, b = 0, a = 0;

	RID ci = this->get_canvas_item();
	batcher.reset();
	// VisualServer::get_singleton()->canvas_item_add_set_blend_mode(ci, VS::MaterialBlendMode(get_blend_mode()));

	//const char *fx_prefix = fx_slot_prefix.get_data();

	for (int i = 0, n = skeleton->slotsCount; i < n; i++)
	{
		spSlot *slot = skeleton->drawOrder[i];
		if (!slot->attachment) continue;
		//bool is_fx = false;
		Ref<Texture> texture;
		switch (slot->attachment->type)
		{
			case SP_ATTACHMENT_REGION:
			{
				spRegionAttachment *attachment = (spRegionAttachment *)slot->attachment;
				//is_fx = strstr(attachment->path, fx_prefix) != NULL;
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
				//is_fx = strstr(attachment->path, fx_prefix) != NULL;
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
			case SP_ATTACHMENT_BOUNDING_BOX:
			{
				spBoundingBoxAttachment* attachment = SUB_CAST(spBoundingBoxAttachment, slot->attachment);
				spVertexAttachment_computeWorldVertices(SUPER(attachment), slot, 0, attachment->super.worldVerticesLength, world_verts.ptrw(), 0, 2);
			}
		}
		if (texture.is_null())
			continue;
		/*
		if (is_fx && slot->data->blendMode != fx_additive) {

			fx_batcher.add_set_blender_mode(slot->data->additiveBlending
				? VisualServer::MATERIAL_BLEND_MODE_ADD
				: get_blend_mode()
			);
			fx_additive = slot->data->additiveBlending;
		}
		else if (slot->data->additiveBlending != additive) {

			batcher.add_set_blender_mode(slot->data->additiveBlending
				? VisualServer::MATERIAL_BLEND_MODE_ADD
				: fx_node->get_blend_mode()
			);
			additive = slot->data->additiveBlending;
		}
		 */

		color.a = skeleton->color.a * slot->color.a * a;
		color.r = skeleton->color.r * slot->color.r * r;
		color.g = skeleton->color.g * slot->color.g * g;
		color.b = skeleton->color.b * slot->color.b * b;

		//if (is_fx)
		//	fx_batcher.add(texture, world_verts.ptr(), uvs, verties_count, triangles, triangles_count, &color, flip_x, flip_y);
		//else
		batcher.add(texture, world_verts.ptr(), uvs, verties_count, triangles, triangles_count, &color);
	}
	batcher.flush();
	//fx_node->update();

	// Slots.
	/*if (debug_attachment_region || debug_attachment_mesh || debug_attachment_skinned_mesh || debug_attachment_bounding_box) {

		Color color(0, 0, 1, 1);
		for (int i = 0, n = skeleton->slotsCount; i < n; i++) {

			spSlot *slot = skeleton->drawOrder[i];
			if (!slot->attachment)
				continue;
			switch (slot->attachment->type) {

				case SP_ATTACHMENT_REGION: {
					if (!debug_attachment_region)
						continue;
					spRegionAttachment *attachment = (spRegionAttachment *)slot->attachment;
					verties_count = 8;
					spRegionAttachment_computeWorldVertices(attachment, slot->bone, world_verts.ptrw(), 0, 2);
					color = Color(0, 0, 1, 1);
					triangles = NULL;
					triangles_count = 0;
					break;
				}
				case SP_ATTACHMENT_MESH: {

					if (!debug_attachment_mesh)
						continue;
					spMeshAttachment *attachment = (spMeshAttachment *)slot->attachment;
					spVertexAttachment_computeWorldVertices(SUPER(attachment), slot, 0, attachment->super.worldVerticesLength, world_verts.ptrw(), 0, 2);
					verties_count = ((spVertexAttachment *)attachment)->verticesCount;
					color = Color(0, 1, 1, 1);
					triangles = attachment->triangles;
					triangles_count = attachment->trianglesCount;
					break;
				}
				case SP_ATTACHMENT_BOUNDING_BOX: {

					if (!debug_attachment_bounding_box)
						continue;
					spBoundingBoxAttachment *attachment = (spBoundingBoxAttachment *)slot->attachment;
					spVertexAttachment_computeWorldVertices(SUPER(attachment), slot, 0, ((spVertexAttachment *)attachment)->verticesCount, world_verts.ptrw(), 0, 2);
					verties_count = ((spVertexAttachment *)attachment)->verticesCount;
					color = Color(0, 1, 0, 1);
					triangles = NULL;
					triangles_count = 0;
					break;
				}
			}

			Point2 *points = (Point2 *)world_verts.ptr();
			int points_size = verties_count / 2;

			for (int idx = 0; idx < points_size; idx++) {

				Point2 &pt = points[idx];
				if (flip_x)
					pt.x = -pt.x;
				if (!flip_y)
					pt.y = -pt.y;
			}

			if (triangles == NULL || triangles_count == 0) {

				for (int idx = 0; idx < points_size; idx++) {

					if (idx == points_size - 1)
						draw_line(points[idx], points[0], color);
					else
						draw_line(points[idx], points[idx + 1], color);
				}
			} else {

				for (int idx = 0; idx < triangles_count - 2; idx += 3) {

					int a = triangles[idx];
					int b = triangles[idx + 1];
					int c = triangles[idx + 2];

					draw_line(points[a], points[b], color);
					draw_line(points[b], points[c], color);
					draw_line(points[c], points[a], color);
				}
			}
		}
	}*/

	if (show_bone) {
		// Bone lengths.
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
	/*if (speed_scale == 0)
		return;
	p_delta *= speed_scale;
	process_delta += p_delta;
	if (skip_frames) {
		frames_to_skip--;
		if (frames_to_skip >= 0) {
			return;
		} else {
			frames_to_skip = skip_frames;
		}
	}*/
	//spAnimationState_update(state, forward ? p_delta : -p_delta);
	spAnimationState_update(state, p_delta);
	spAnimationState_apply(state, skeleton);
	spSkeleton_updateWorldTransform(skeleton);
	//print_line(String("process(") + String(Variant(p_delta)) + ")");
	for (AttachmentNodes::Element *E = attachment_nodes.front(); E; ) {

		AttachmentNode &info = E->get();
		WeakRef *ref = info.ref;
		Object *obj = ref->get_ref();
		Node2D *node = (obj != NULL) ? Object::cast_to<Node2D>(obj) : NULL;
		if (obj == NULL || node == NULL) {

			AttachmentNodes::Element *NEXT = E->next();
			attachment_nodes.erase(E);
			E = NEXT;
			if (E == NULL)
				break;
			continue;
		}
		spBone *bone = info.bone;
		if (info.flag & ATTACHMENT_BONE_FLAG_OFFSET)
			node->call("set_position", Vector2(bone->worldX + bone->skeleton->x, -bone->worldY + bone->skeleton->y) + info.ofs);
		if (info.flag & ATTACHMENT_BONE_FLAG_SCALE)
			node->call("set_scale", Vector2(spBone_getWorldScaleX(bone), spBone_getWorldScaleY(bone)) * info.scale);
		if (info.flag & ATTACHMENT_BONE_FLAG_ROTATION)
			node->call("set_rotation", Math::atan2(bone->c, bone->d) + Math::deg2rad(info.rot));
		E = E->next();
	}
	update();
	//process_delta = 0;
}
//
//void Spine::_set_process(bool p_process, bool p_force) {
//
//	if (processing == p_process && !p_force)
//		return;
//
//	switch (animation_process_mode) {
//
//		case ANIMATION_PROCESS_FIXED: set_physics_process(p_process && active); break;
//		case ANIMATION_PROCESS_IDLE: set_process(p_process && active); break;
//	}
//
//	processing = p_process;
//}

bool Spine::_set(const StringName &p_name, const Variant &p_value) {
	/*if (p_name == "playback/play") {
		print_line("_set: playback/play > " + (String)p_value);
		return true;
	}
	else if (p_name == "forward")
	{
		forward = p_value;
		if (forward)
			print_line("_set: forward > true");
		else
			print_line("_set: forward > false");
		return true;
	}
	else *///if (p_name == "internal_animations")
	//{
		//ERR_FAIL_COND_V(skeleton == NULL || state == NULL || skeleton->data->animationsCount <= 0, true);
		// 如果不是SpineAnimationAttri引用就轉換新的SpineAnimationAttri引用
		/*internal_animations = p_value;
		for (int i = 0; i < internal_animations.size(); ++i)
		{
			if (!internal_animations[i].is_ref() || Ref<SpineAnimationAttri>(internal_animations[i]).is_null())
			{
				Ref<SpineAnimationAttri> ref(memnew(SpineAnimationAttri));
				if (skeleton != NULL)
					ref->set_animation_name(skeleton->data->animations[rand() % skeleton->data->animationsCount]->name);
				internal_animations[i] = ref;
			}				
		}*/
		/*spAnimationState_clearTracks(state);
		for (int i = 0; i < anims.size(); ++i)
		{
			SpineAnimationAttri* pAttri = cast_to<SpineAnimationAttri>(anims[i].operator Object*());
			if (pAttri)
			{
				add_animation(pAttri);
			}	
		}*/
	//}
	if (p_name == "skin")
	{
		//print_line("before set skin(" + String(p_value) + ")!");
		ERR_FAIL_COND_V(skeleton == NULL, true);
		set_skin(p_value);
		//print_line("setted skin(" + String(p_value) + ")!");
		return true;
	}
	else if (p_name == "clear_tracks")
	{
		if ((bool)p_value)
		{
			clear_tracks();
			_change_notify();
		}
		return true;
	}
	
	//String name = p_name;

	//if (name == "playback/play") {

	//	String which = p_value;
	//	if (skeleton != NULL) {

	//		if (which == "[stop]")
	//			stop();
	//		else if (has_animation(which)) {
	//			reset();
	//			play(which, 1, loop);
	//		}
	//	} else
	//		current_animation = which;

	//	if (current_animation == "[stop]")
	//		actual_duration = 0.0;
	//	else
	//		actual_duration = get_animation_length(current_animation);
	//	// Call this immediately to make the duration visible
	//	set_duration(actual_duration);
	//} else if (name == "playback/loop") {

	//	loop = p_value;
	//	if (skeleton != NULL && has_animation(current_animation))
	//		play(current_animation, 1, loop);
	//} else if (name == "playback/forward") {

	//	forward = p_value;
	//} else if (name == "playback/skin") {

	//	skin = p_value;
	//	if (skeleton != NULL)
	//		set_skin(skin);
	//} else if (name == "debug/region")
	//	set_debug_attachment(DEBUG_ATTACHMENT_REGION, p_value);
	//else if (name == "debug/mesh")
	//	set_debug_attachment(DEBUG_ATTACHMENT_MESH, p_value);
	//else if (name == "debug/skinned_mesh")
	//	set_debug_attachment(DEBUG_ATTACHMENT_SKINNED_MESH, p_value);
	//else if (name == "debug/bounding_box")
	//	set_debug_attachment(DEBUG_ATTACHMENT_BOUNDING_BOX, p_value);
	return false;
}

bool Spine::_get(const StringName &p_name, Variant &r_ret) const
{
	//if (p_name == "internal_animations")
	//{
		//print_line("_get animations");
		//ERR_FAIL_COND_V(state == NULL, true);
		//for (int i = 0; i < state->tracksCount; ++i)
		//{
		//	for (spTrackEntry* j = state->tracks[i]; j; j = j->next)
		//	{
		//		Ref<SpineAnimationAttri> ref = ((SpineAnimation*)(j->userData))->get_attri();
		//		ret.append(ref);
		//		//print_line(((Ref<SpineAnimationAttri>)ret.back())->get_animation_name());
		//		//Ref<SpineAnimationAttri> attri = Ref<SpineAnimationAttri>(memnew(SpineAnimationAttri));
		//		//ret.append(attri);
		//	}
		//}
	//	r_ret = internal_animations;
	//	return true;
	//}
	if (p_name == "skin")
	{
		ERR_FAIL_COND_V(skeleton == NULL, true);
		r_ret = get_skin();
		//print_line("getted skin(" + r_ret.operator String() + ").");
		return true;
	}
	else if (p_name == "clear_tracks")
	{
		ERR_FAIL_COND_V(skeleton == NULL, true);
		r_ret = false;
		return true;
	}

	//String name = p_name;

	//if (p_name == "playback/play") {
	//	r_ret = "[stop]";
	//	print_line("_get: playback/play");
	////	r_ret = current_animation;
	//	return true;
	//}
	//else if (p_name == "playback/loop")
	//{
	//	//r_ret = loop;
	//	r_ret = true;
	//	print_line("_get: playback/loop");
	//	return true;
	//}
	//else if (p_name == "forward")
	//{
	//	r_ret = forward;
	//	print_line("_get: forward");
	//	return true;
	//}
	//else if (name == "playback/skin")
	//	r_ret = skin;
	//else if (name == "debug/region")
	//	r_ret = is_debug_attachment(DEBUG_ATTACHMENT_REGION);
	//else if (name == "debug/mesh")
	//	r_ret = is_debug_attachment(DEBUG_ATTACHMENT_MESH);
	//else if (name == "debug/skinned_mesh")
	//	r_ret = is_debug_attachment(DEBUG_ATTACHMENT_SKINNED_MESH);
	//else if (name == "debug/bounding_box")
	//	r_ret = is_debug_attachment(DEBUG_ATTACHMENT_BOUNDING_BOX);
	return false;
}

//float Spine::get_animation_length(String p_animation) const {
//	if (state == NULL) return 0;
//	for (int i = 0; i < state->data->skeletonData->animationsCount; i++) {
//		spAnimation *anim = state->data->skeletonData->animations[i];
//		if (anim->name == p_animation) {
//			return anim->duration;
//		}
//	}
//	return 0;
//}

void Spine::_get_property_list(List<PropertyInfo> *p_list) const {
	//print_line("_get_property_list");
	//List<String> names;

	//if (state != NULL) {

	//	for (int i = 0; i < state->data->skeletonData->animationsCount; i++) {
	//		names.push_back(state->data->skeletonData->animations[i]->name);
	//	}
	//}
	//{
	//	names.sort();
	//	names.push_front("[stop]");
	//	String hint;
	//	for (List<String>::Element *E = names.front(); E; E = E->next()) {

	//		if (E != names.front())
	//			hint += ",";
	//		hint += E->get();
	//	}

	//	p_list->push_back(PropertyInfo(Variant::STRING, "playback/play", PROPERTY_HINT_ENUM, hint));
	//	//p_list->push_back(PropertyInfo(Variant::BOOL, "playback/loop", PROPERTY_HINT_NONE));
	//	p_list->push_back(PropertyInfo(Variant::BOOL, "forward", PROPERTY_HINT_NONE));
	//}
	if (skeleton != NULL)
	{
		//p_list->push_back(PropertyInfo(Variant::ARRAY, "animations", PROPERTY_HINT_NONE, "AStar2D:AStar2D"));
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
		//print_line("_get_property_list");
	} 
	//p_list->push_back(PropertyInfo(Variant::ARRAY, "internal_animations", PROPERTY_HINT_TYPE_STRING, "17/17:SpineAnimationAttri"));
	//names.clear();
	/*{
		if (state != NULL) {

			for (int i = 0; i < state->data->skeletonData->skinsCount; i++) {

				names.push_back(state->data->skeletonData->skins[i]->name);
			}
		}

		String hint;
		for (List<String>::Element *E = names.front(); E; E = E->next()) {

			if (E != names.front())
				hint += ",";
			hint += E->get();
		}

		p_list->push_back(PropertyInfo(Variant::STRING, "playback/skin", PROPERTY_HINT_ENUM, hint));
	}
	p_list->push_back(PropertyInfo(Variant::BOOL, "debug/region", PROPERTY_HINT_NONE));
	p_list->push_back(PropertyInfo(Variant::BOOL, "debug/mesh", PROPERTY_HINT_NONE));
	p_list->push_back(PropertyInfo(Variant::BOOL, "debug/skinned_mesh", PROPERTY_HINT_NONE));
	p_list->push_back(PropertyInfo(Variant::BOOL, "debug/bounding_box", PROPERTY_HINT_NONE));*/
}

void Spine::_notification(int p_what) {
	switch (p_what)
	{
		case NOTIFICATION_ENTER_TREE:
		{
			//if (!processing) {
			//	//make sure that a previous process state was not saved
			//	//only process if "processing" is set
			//	set_physics_process(false);
			//	set_process(false);
			//}
		} break;
		//case NOTIFICATION_READY:
		//{
		//	// add fx node as child
		//	fx_node->connect("draw", this, "_on_fx_draw");
		//	fx_node->set_z_index(1);
		//	fx_node->set_z_as_relative(false);
		//	add_child(fx_node);

		//	if (!Engine::get_singleton()->is_editor_hint() && has_animation(autoplay)) {
		//		play(autoplay);
		//	}
		//} break;
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

		/*case NOTIFICATION_EXIT_TREE:
		{
			stop_all();
		} break;*/
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
	//ERR_FAIL_COND(!res->data);
	
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
		//print_line("SpineAnimationAttri type changed!");
	}
	//else
	//{
	//	print_line("wrong resource type changed!");
	//}
	
	
	return;
}

PoolStringArray Spine::get_skins()
{
	if (skeleton == NULL)
		return PoolStringArray();
	PoolStringArray ret;
	for (int i = 0; i < skeleton->data->skinsCount; ++i)
	{
		ret.append(skeleton->data->skins[i]->name);
	}
	
	return ret;
}

bool Spine::has_skin(const String& p_skin)
{
	if (skeleton == NULL)
		return false;
	return spSkeletonData_findSkin(skeleton->data, p_skin.utf8().get_data()) != NULL;
}

String Spine::get_skin() const
{
	if (skeleton == NULL)
		return "";
	if (skeleton->skin)
		return skeleton->skin->name;
	else
		return skeleton->data->defaultSkin->name;
}

void Spine::set_skin(const String& p_skin)
{
	if (skeleton == NULL)
		return;
	if (p_skin == skeleton->data->defaultSkin->name)
	{
		if (skeleton->skin == NULL)
			return;
		spSkeleton_setSkin(skeleton, 0);
	}
	else if (has_skin(p_skin))
	{
		if (skeleton->skin && skeleton->skin->name == p_skin)
			return;
		spSkeleton_setSkinByName(skeleton, String(p_skin).utf8().get_data());
	}
	else
		return;
	spSkeleton_setSlotsToSetupPose(skeleton);
	spAnimationState_apply(state, skeleton);
	update();
}

void Spine::add_internal_animations()
{
	for (int i = 0; i < internal_animations.size(); ++i)
	{
		add_animation_by_attribute(cast_to<SpineAnimationAttri>(internal_animations[i]));
	}
}

void Spine::add_animation_by_attribute(Ref<SpineAnimationAttri> p_anim)
{
	ERR_FAIL_COND(state == NULL);
	SpineAnimation* anim = add_animation(p_anim->get_animation_name(), p_anim->get_track(), p_anim->get_loop(), p_anim->get_delay());
	if (!anim)
		return;
	anim->set_mix_duration(p_anim->get_mix());
	if (p_anim->get_alpha() < 1.0)
		anim->set_alpha(p_anim->get_alpha());
	if (p_anim->get_start() > 0)
		anim->set_start(p_anim->get_start());
	if (!p_anim->is_default_end())
		anim->set_end(p_anim->get_end());
	if (p_anim->get_speed_scale() != 1.0)
		anim->set_speed_scale(p_anim->get_speed_scale());
}

SpineAnimation* Spine::add_animation(const String& p_name, int p_track, bool p_loop, float p_delay)
{
	ERR_FAIL_COND_V(state == NULL, nullptr);
	bool ended = is_end();
	SpineAnimation* ret = memnew(SpineAnimation);
	ret->state = state;
	spTrackEntry* trackEntry = spAnimationState_addAnimationByNameWithData(state, p_track, p_name.utf8().get_data(), p_loop, p_delay, ret);
	if (!trackEntry)
	{
		memdelete(ret);
		return nullptr;
	}
	if (ended)
	{
		_change_notify();
	}
	ret->trackEntry = trackEntry;
	/*trackEntry->userData = ret;*/
	//print_line("setted track user data");
	return ret;
}

SpineAnimation* Spine::add_empty_animation(int p_track, float p_mix, float p_delay)
{
	ERR_FAIL_COND_V(state == NULL, nullptr);
	bool ended = is_end();
	SpineAnimation* ret = memnew(SpineAnimation);
	ret->state = state;
	spTrackEntry* trackEntry = spAnimationState_addEmptyAnimationWithData(state, p_track, p_mix, p_delay, ret);
	if (!trackEntry)
	{
		memdelete(ret);
		return nullptr;
	}
	if (ended)
	{
		_change_notify();
	}
	else
	{
		ret->trackEntry = trackEntry;
	}
	/*trackEntry->userData = ret;*/
	//print_line("setted track user data");
	return ret;
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

void Spine::clear_tracks()
{
	if (state == NULL)
		return;
	spAnimationState_clearTracks(state);
}

void Spine::clear_track(int p_track)
{
	if (state == NULL)
		return;
	spAnimationState_clearTrack(state, p_track);
}

//Array Spine::get_animation_names() const {
//
//	Array names;
//
//	if (state != NULL) {
//		for (int i = 0; i < state->data->skeletonData->animationsCount; i++) {
//			spAnimation *anim = state->data->skeletonData->animations[i];
//			names.push_back(anim->name);
//		}
//	}
//
//	return names;
//}
//
//bool Spine::has_animation(const String &p_name) {
//
//	if (skeleton == NULL) return false;
//	spAnimation *animation = spSkeletonData_findAnimation(skeleton->data, p_name.utf8().get_data());
//	return animation != NULL;
//}
//
//void Spine::set_default_mix(real_t p_duration) {
//
//	ERR_FAIL_COND(state == NULL);
//	ERR_FAIL_COND(p_duration <= 0.0f);
//	state->data->defaultMix = p_duration;
//}
//
//void Spine::mix(const String &p_from, const String &p_to, real_t p_duration) {
//
//	ERR_FAIL_COND(state == NULL);
//	spAnimationStateData_setMixByName(state->data, p_from.utf8().get_data(), p_to.utf8().get_data(), p_duration);
//}
//
//bool Spine::play(const String &p_name, bool p_loop, int p_track, int p_delay) {
//
//	ERR_FAIL_COND_V(skeleton == NULL, false);
//	spAnimation *animation = spSkeletonData_findAnimation(skeleton->data, p_name.utf8().get_data());
//	ERR_FAIL_COND_V(animation == NULL, false);
//	spTrackEntry *entry = spAnimationState_setAnimation(state, p_track, animation, p_loop);
//	entry->delay = p_delay;
//	current_animation = p_name;
//	if (skip_frames) {
//		frames_to_skip = 0;
//	}
//
//	_set_process(true);
//	playing = true;
//	// update frame
//	if (!is_active())
//		_animation_process(0);
//
//	return true;
//}
//
//bool Spine::add(const String &p_name, bool p_loop, int p_track, int p_delay) {
//
//	ERR_FAIL_COND_V(skeleton == NULL, false);
//	spAnimation *animation = spSkeletonData_findAnimation(skeleton->data, p_name.utf8().get_data());
//	ERR_FAIL_COND_V(animation == NULL, false);
//	spTrackEntry *entry = spAnimationState_addAnimation(state, p_track, animation, p_loop, p_delay);
//
//	_set_process(true);
//	playing = true;
//
//	return true;
//}
//
//void Spine::clear(int p_track) {
//
//	ERR_FAIL_COND(state == NULL);
//	if (p_track == -1)
//		spAnimationState_clearTracks(state);
//	else
//		spAnimationState_clearTrack(state, p_track);
//}
//
//void Spine::stop() {
//
//	_set_process(false);
//	playing = false;
//	current_animation = "[stop]";
//	reset();
//}
//
//bool Spine::is_playing(int p_track) const {
//
//	return playing && spAnimationState_getCurrent(state, p_track) != NULL;
//}
//

//
//void Spine::set_skip_frames(int p_skip_frames) {
//	skip_frames = p_skip_frames;
//	frames_to_skip = 0;
//}
//
//int Spine::get_skip_frames() const {
//	return skip_frames;
//}
//
//String Spine::get_current_animation(int p_track = 0) {
//
//	ERR_FAIL_COND_V(state == NULL, "");
//	spTrackEntry *entry = spAnimationState_getCurrent(state, p_track);
//	if (entry == NULL || entry->animation == NULL)
//		return "";
//	return entry->animation->name;
//}
//
//void Spine::stop_all() {
//
//	stop();
//
//	_set_process(false); // always process when starting an animation
//}
//
void Spine::reset()
{
	if (skeleton == NULL) {
		return;
	}
	spSkeleton_setToSetupPose(skeleton);
	_animation_process(0.0);
}

void Spine::seek(float p_delta) {

	if (state == NULL)
		return;
	_animation_process(p_delta);
}

float Spine::get_play_time() const
{
	if (skeleton == NULL)
		return 0.0f;
	return skeleton->time;
}

//float Spine::tell() const {
//
//	return current_pos;
//}
//
//void Spine::set_active(bool p_active) {
//
//	if (active == p_active)
//		return;
//
//	active = p_active;
//	_set_process(processing, true);
//}
//
//bool Spine::is_active() const {
//
//	return active;
//}
//
//void Spine::set_speed(float p_speed) {
//
//	speed_scale = p_speed;
//}
//
//float Spine::get_speed() const {
//
//	return speed_scale;
//}
//
//void Spine::set_autoplay(const String &p_name) {
//
//	autoplay = p_name;
//}
//
//String Spine::get_autoplay() const {
//
//	return autoplay;
//}
//
//void Spine::set_modulate(const Color &p_color) {
//
//	modulate = p_color;
//	update();
//}
//
//Color Spine::get_modulate() const {
//
//	return modulate;
//}
//
//void Spine::set_flip_x(bool p_flip) {
//
//	flip_x = p_flip;
//	update();
//}
//
//void Spine::set_flip_y(bool p_flip) {
//
//	flip_y = p_flip;
//	update();
//}
//
//bool Spine::is_flip_x() const {
//
//	return flip_x;
//}
//
//bool Spine::is_flip_y() const {
//
//	return flip_y;
//}
//
//bool Spine::set_skin(const String &p_name) {
//
//	ERR_FAIL_COND_V(skeleton == NULL, false);
//	return spSkeleton_setSkinByName(skeleton, p_name.utf8().get_data()) ? true : false;
//}
//
//void Spine::set_duration(float p_duration) {
//	// Ignore p_duration, because it can't actually be affected and this should be read-only
//	duration = actual_duration;
//	update();
//	_change_notify("playback/duration");
//}
//
//float Spine::get_duration() const {
//	return duration;
//}
//
//Dictionary Spine::get_skeleton() const {
//
//	ERR_FAIL_COND_V(skeleton == NULL, Variant());
//	Dictionary dict;
//
//	dict["bonesCount"] = skeleton->bonesCount;
//	dict["slotCount"] = skeleton->slotsCount;
//	dict["ikConstraintsCount"] = skeleton->ikConstraintsCount;
//	dict["time"] = skeleton->time;
//	dict["flipX"] = skeleton->scaleX;
//	dict["flipY"] = skeleton->scaleY;
//	dict["x"] = skeleton->x;
//	dict["y"] = skeleton->y;
//
//	return dict;
//}
//
//Dictionary Spine::get_attachment(const String &p_slot_name, const String &p_attachment_name) const {
//
//	ERR_FAIL_COND_V(skeleton == NULL, Variant());
//	spAttachment *attachment = spSkeleton_getAttachmentForSlotName(skeleton, p_slot_name.utf8().get_data(), p_attachment_name.utf8().get_data());
//	ERR_FAIL_COND_V(attachment == NULL, Variant());
//
//	Dictionary dict;
//	dict["name"] = attachment->name;
//
//	switch (attachment->type) {
//		case SP_ATTACHMENT_REGION: {
//
//			spRegionAttachment *info = (spRegionAttachment *)attachment;
//			dict["type"] = "region";
//			dict["path"] = info->path;
//			dict["x"] = info->x;
//			dict["y"] = info->y;
//			dict["scaleX"] = info->scaleX;
//			dict["scaleY"] = info->scaleY;
//			dict["rotation"] = info->rotation;
//			dict["width"] = info->width;
//			dict["height"] = info->height;
//			dict["color"] = Color(info->color.r, info->color.g, info->color.b, info->color.a);
//			dict["region"] = Rect2(info->regionOffsetX, info->regionOffsetY, info->regionWidth, info->regionHeight);
//			dict["region_original_size"] = Size2(info->regionOriginalWidth, info->regionOriginalHeight);
//
//			Vector<Vector2> offset, uvs;
//			for (int idx = 0; idx < 4; idx++) {
//				offset.push_back(Vector2(info->offset[idx * 2], info->offset[idx * 2 + 1]));
//				uvs.push_back(Vector2(info->uvs[idx * 2], info->uvs[idx * 2 + 1]));
//			}
//			dict["offset"] = offset;
//			dict["uvs"] = uvs;
//
//		} break;
//
//		case SP_ATTACHMENT_BOUNDING_BOX: {
//
//			spVertexAttachment *info = (spVertexAttachment *)attachment;
//			dict["type"] = "bounding_box";
//
//			Vector<Vector2> vertices;
//			for (int idx = 0; idx < info->verticesCount / 2; idx++)
//				vertices.push_back(Vector2(info->vertices[idx * 2], -info->vertices[idx * 2 + 1]));
//			dict["vertices"] = vertices;
//		} break;
//
//		case SP_ATTACHMENT_MESH: {
//
//			spMeshAttachment *info = (spMeshAttachment *)attachment;
//			dict["type"] = "mesh";
//			dict["path"] = info->path;
//			dict["color"] = Color(info->color.r, info->color.g, info->color.b, info->color.a);
//		} break;
//	}
//	return dict;
//}
//
//Dictionary Spine::get_bone(const String &p_bone_name) const {
//
//	ERR_FAIL_COND_V(skeleton == NULL, Variant());
//	spBone *bone = spSkeleton_findBone(skeleton, p_bone_name.utf8().get_data());
//	ERR_FAIL_COND_V(bone == NULL, Variant());
//	Dictionary dict;
//	dict["x"] = bone->x;
//	dict["y"] = bone->y;
//	dict["rotation"] = bone->rotation;
//	dict["rotationIK"] = 0; //bone->rotationIK;
//	dict["scaleX"] = bone->scaleX;
//	dict["scaleY"] = bone->scaleY;
//	dict["flipX"] = 0; //bone->flipX;
//	dict["flipY"] = 0; //bone->flipY;
//	dict["m00"] = bone->a; //m00;
//	dict["m01"] = bone->b; //m01;
//	dict["m10"] = bone->c; //m10;
//	dict["m11"] = bone->d; //m11;
//	dict["worldX"] = bone->worldX;
//	dict["worldY"] = bone->worldY;
//	dict["worldRotation"] = spBone_getWorldRotationX(bone); //->worldRotation;
//	dict["worldScaleX"] = spBone_getWorldScaleX(bone); //->worldScaleX;
//	dict["worldScaleY"] = spBone_getWorldScaleY(bone); //->worldScaleY;
//	dict["worldFlipX"] = 0; //bone->worldFlipX;
//	dict["worldFlipY"] = 0; //bone->worldFlipY;
//
//	return dict;
//}
//
//Dictionary Spine::get_slot(const String &p_slot_name) const {
//
//	ERR_FAIL_COND_V(skeleton == NULL, Variant());
//	spSlot *slot = spSkeleton_findSlot(skeleton, p_slot_name.utf8().get_data());
//	ERR_FAIL_COND_V(slot == NULL, Variant());
//	Dictionary dict;
//	dict["color"] = Color(slot->color.r, slot->color.g, slot->color.b, slot->color.a);
//	return dict;
//}
//
//bool Spine::set_attachment(const String &p_slot_name, const Variant &p_attachment) {
//
//	ERR_FAIL_COND_V(skeleton == NULL, false);
//	if (p_attachment.get_type() == Variant::STRING)
//		return spSkeleton_setAttachment(skeleton, p_slot_name.utf8().get_data(), ((const String)p_attachment).utf8().get_data()) != 0;
//	else
//		return spSkeleton_setAttachment(skeleton, p_slot_name.utf8().get_data(), NULL) != 0;
//}
//
String Spine::get_attachment_node_bone(const Variant &p_node) const
{
	Object *obj = p_node;
	if (!obj->has_meta("spine_meta"))
		return "";
	AttachmentNode* info = (AttachmentNode *)((uint64_t)obj->get_meta("spine_meta"));
	return info->bone->data->name;
}

Array Spine::get_attachment_nodes(const String& p_bone) const
{
	Array ret = Array();
	for (const AttachmentNodes::Element* E = attachment_nodes.front(); E; E = E->next)
	{
		if (E->get().bone->data->name == p_bone)
		{
			ret.append(E->get().ref->get_ref());
		}
	}
	return ret;
}

bool Spine::add_attachment_node(const String &p_bone_name, const Variant &p_node, Vector2 p_ofs, Vector2 p_scale, real_t p_rot, int p_flag)
{
	ERR_FAIL_COND_V(skeleton == NULL, false);
	spBone *bone = spSkeleton_findBone(skeleton, p_bone_name.utf8().get_data());
	ERR_FAIL_COND_V(bone == NULL, false);
	Object *obj = p_node;
	ERR_FAIL_COND_V(obj == NULL, false);
	Node2D *node = Object::cast_to<Node2D>(obj);
	ERR_FAIL_COND_V(node == NULL, false);

	if (obj->has_meta("spine_meta")) {

		AttachmentNode *info = (AttachmentNode *)((uint64_t)obj->get_meta("spine_meta"));
		if (info->bone != bone) {
			// add to different bone, remove first
			remove_attachment_node(p_node);
		} else {
			// add to same bone, update params
			info->ofs = p_ofs;
			info->scale = p_scale;
			info->rot = p_rot;
			info->flag = p_flag;
			return true;
		}
	}
	attachment_nodes.push_back(AttachmentNode());
	AttachmentNode &info = attachment_nodes.back()->get();
	info.E = attachment_nodes.back();
	info.bone = bone;
	info.ref = memnew(WeakRef);
	info.ref->set_obj(node);
	info.ofs = p_ofs;
	info.scale = p_scale;
	info.rot = p_rot;
	info.flag = p_flag;
	obj->set_meta("spine_meta", (uint64_t)&info);

	return true;
}

bool Spine::remove_attachment_node(const Variant &p_node) {

	//ERR_FAIL_COND_V(skeleton == NULL, false);
	//spBone *bone = spSkeleton_findBone(skeleton, p_bone_name.utf8().get_data());
	//ERR_FAIL_COND_V(bone == NULL, false);
	Object *obj = p_node;
	ERR_FAIL_COND_V(obj == NULL, false);
	//Node2D *node = Object::cast_to<Node2D>(obj);
	//ERR_FAIL_COND_V(node == NULL, false);

	if (!obj->has_meta("spine_meta"))
		return false;

	AttachmentNode *info = (AttachmentNode *)((uint64_t)obj->get_meta("spine_meta"));
	//ERR_FAIL_COND_V(info->bone != bone, false);
	//obj->set_meta("spine_meta", Variant());
	obj->remove_meta("spine_meta");
	memdelete(info->ref);
	attachment_nodes.erase(info->E);

	return true;
}

Vector2 Spine::get_attachment_node_offset(const Variant& p_node) const
{
	Object *obj = p_node;
	ERR_FAIL_COND_V(obj == NULL, Vector2());
	ERR_FAIL_COND(!obj->has_meta("spine_meta"));

	AttachmentNode *info = (AttachmentNode *)((uint64_t)obj->get_meta("spine_meta"));
	return info->ofs;
}

real_t Spine::get_attachment_node_rotation(const Variant& p_node) const
{
	Object *obj = p_node;
	ERR_FAIL_COND_V(obj == NULL, 0.0);
	ERR_FAIL_COND(!obj->has_meta("spine_meta"));

	AttachmentNode *info = (AttachmentNode *)((uint64_t)obj->get_meta("spine_meta"));
	return info->rot;
}

Vector2 Spine::get_attachment_node_scale(const Variant& p_node) const
{
	Object *obj = p_node;
	ERR_FAIL_COND_V(obj == NULL, Vector2());
	ERR_FAIL_COND(!obj->has_meta("spine_meta"));

	AttachmentNode *info = (AttachmentNode *)((uint64_t)obj->get_meta("spine_meta"));
	return info->scale;
}

void Spine::set_attachment_node_offset(Variant& p_node, Vector2 p_offset)
{
	Object *obj = p_node;
	ERR_FAIL_COND(obj == NULL);
	ERR_FAIL_COND(!obj->has_meta("spine_meta"));

	AttachmentNode *info = (AttachmentNode *)((uint64_t)obj->get_meta("spine_meta"));
	info->ofs = p_offset;
	_animation_process(0.0f);
}

void Spine::set_attachment_node_rotation(Variant& p_node, real_t p_rotation)
{
	Object *obj = p_node;
	ERR_FAIL_COND(obj == NULL);
	ERR_FAIL_COND(!obj->has_meta("spine_meta"));

	AttachmentNode *info = (AttachmentNode *)((uint64_t)obj->get_meta("spine_meta"));
	info->rot = p_rotation;
	_animation_process(0.0f);
}

void Spine::set_attachment_node_scale(Variant& p_node, Vector2 p_scale)
{
	Object *obj = p_node;
	ERR_FAIL_COND(obj == NULL);
	ERR_FAIL_COND(!obj->has_meta("spine_meta"));


	AttachmentNode *info = (AttachmentNode *)((uint64_t)obj->get_meta("spine_meta"));
	info->scale = p_scale;
	_animation_process(0.0f);
}

//
//Ref<Shape2D> Spine::get_bounding_box(const String &p_slot_name, const String &p_attachment_name) {
//
//	ERR_FAIL_COND_V(skeleton == NULL, Ref<Shape2D>());
//	spAttachment *attachment = spSkeleton_getAttachmentForSlotName(skeleton, p_slot_name.utf8().get_data(), p_attachment_name.utf8().get_data());
//	ERR_FAIL_COND_V(attachment == NULL, Ref<Shape2D>());
//	ERR_FAIL_COND_V(attachment->type != SP_ATTACHMENT_BOUNDING_BOX, Ref<Shape2D>());
//	spVertexAttachment *info = (spVertexAttachment *)attachment;
//
//	Vector<Vector2> points;
//	points.resize(info->verticesCount / 2);
//	for (int idx = 0; idx < info->verticesCount / 2; idx++)
//#if (VERSION_MAJOR >= 3)
//		points.write[idx] = Vector2(info->vertices[idx * 2], -info->vertices[idx * 2 + 1]);
//#else
//		points[idx] = Vector2(info->vertices[idx * 2], -info->vertices[idx * 2 + 1]);
//#endif
//
//	ConvexPolygonShape2D *shape = memnew(ConvexPolygonShape2D);
//	shape->set_points(points);
//
//	return shape;
//}
//
//bool Spine::add_bounding_box(const String &p_bone_name, const String &p_slot_name, const String &p_attachment_name, const Variant &p_node, const Vector2 &p_ofs, const Vector2 &p_scale, const real_t p_rot) {
//
//	ERR_FAIL_COND_V(skeleton == NULL, false);
//	Object *obj = p_node;
//	ERR_FAIL_COND_V(obj == NULL, false);
//	CollisionObject2D *node = Object::cast_to<CollisionObject2D>(obj);
//	ERR_FAIL_COND_V(node == NULL, false);
//	Ref<Shape2D> shape = get_bounding_box(p_slot_name, p_attachment_name);
//	if (shape.is_null())
//		return false;
//	node->shape_owner_add_shape(0, shape);
//
//	return add_attachment_node(p_bone_name, p_node);
//}
//
//bool Spine::remove_bounding_box(const String &p_bone_name, const Variant &p_node) {
//
//	return remove_attachment_node(p_bone_name, p_node);
//}
//

Array Spine::get_internal_animations() const
{
	return internal_animations;
}

void Spine::set_internal_animations(Array p_animations)
{
	//print_line("[set_internal_animations] p_animations size = " + String(Variant(p_animations.size())));
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

//void Spine::set_forward(bool p_forward)
//{
//	forward = p_forward;
//}
//
//bool Spine::is_forward() const
//{
//	return forward;
//}

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

void Spine::set_collision_whitelist(bool p_value)
{
	if (collision_whitelist == p_value)
		return;
	collision_whitelist = p_value;
	_change_notify("collision_whitelist");
}

bool Spine::is_collision_whitelist() const
{
	return collision_whitelist;
}

void Spine::set_collision_layer(uint32_t p_layer)
{
	if (collision_layer == p_layer)
		return;
	collision_layer = p_layer;
	_change_notify("collision_layer");
}

uint32_t Spine::get_collision_layer() const
{
	return collision_layer;
}

void Spine::set_collision_mask(uint32_t p_mask)
{
	if (collision_mask == p_mask)
		return;
	collision_mask = p_mask;
	_change_notify("collision_mask");
}

uint32_t Spine::get_collision_mask() const
{
	return collision_mask;
}


//
//void Spine::set_fx_slot_prefix(const String &p_prefix) {
//
//	fx_slot_prefix = p_prefix.utf8();
//	update();
//}
//
//String Spine::get_fx_slot_prefix() const {
//
//	String s;
//	s.parse_utf8(fx_slot_prefix.get_data());
//	return s;
//}
//
//void Spine::set_debug_bones(bool p_enable) {
//
//	debug_bones = p_enable;
//	update();
//}
//
//bool Spine::is_debug_bones() const {
//
//	return debug_bones;
//}
//
//void Spine::set_debug_attachment(DebugAttachmentMode p_mode, bool p_enable) {
//
//	switch (p_mode) {
//
//		case DEBUG_ATTACHMENT_REGION:
//			debug_attachment_region = p_enable;
//			break;
//		case DEBUG_ATTACHMENT_MESH:
//			debug_attachment_mesh = p_enable;
//			break;
//		case DEBUG_ATTACHMENT_SKINNED_MESH:
//			debug_attachment_skinned_mesh = p_enable;
//			break;
//		case DEBUG_ATTACHMENT_BOUNDING_BOX:
//			debug_attachment_bounding_box = p_enable;
//			break;
//	};
//	update();
//}
//
//bool Spine::is_debug_attachment(DebugAttachmentMode p_mode) const {
//
//	switch (p_mode) {
//
//		case DEBUG_ATTACHMENT_REGION:
//			return debug_attachment_region;
//		case DEBUG_ATTACHMENT_MESH:
//			return debug_attachment_mesh;
//		case DEBUG_ATTACHMENT_SKINNED_MESH:
//			return debug_attachment_skinned_mesh;
//		case DEBUG_ATTACHMENT_BOUNDING_BOX:
//			return debug_attachment_bounding_box;
//	};
//	return false;
//}
//
//void Spine::set_debug_attachment_region(bool p_enable) {
//	set_debug_attachment(DEBUG_ATTACHMENT_REGION, p_enable);
//}
//
//bool Spine::is_debug_attachment_region() const {
//	return is_debug_attachment(DEBUG_ATTACHMENT_REGION);
//}
//
//void Spine::set_debug_attachment_mesh(bool p_enable) {
//	set_debug_attachment(DEBUG_ATTACHMENT_MESH, p_enable);
//}
//
//bool Spine::is_debug_attachment_mesh() const {
//	return is_debug_attachment(DEBUG_ATTACHMENT_MESH);
//}
//
//void Spine::set_debug_attachment_skinned_mesh(bool p_enable) {
//	set_debug_attachment(DEBUG_ATTACHMENT_SKINNED_MESH, p_enable);
//}
//
//bool Spine::is_debug_attachment_skinned_mesh() const {
//	return is_debug_attachment(DEBUG_ATTACHMENT_SKINNED_MESH);
//}
//
//void Spine::set_debug_attachment_bounding_box(bool p_enable) {
//	set_debug_attachment(DEBUG_ATTACHMENT_BOUNDING_BOX, p_enable);
//}
//
//bool Spine::is_debug_attachment_bounding_box() const {
//	return is_debug_attachment(DEBUG_ATTACHMENT_BOUNDING_BOX);
//}

void Spine::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("set_resource", "value"), &Spine::set_resource);
	ClassDB::bind_method(D_METHOD("get_resource"), &Spine::get_resource);
	ClassDB::bind_method(D_METHOD("resource_changed", "spine_skeleton_data"), &Spine::resource_changed);

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
	//ClassDB::bind_method(D_METHOD("set_forward", "forward"), &Spine::set_forward);
	//ClassDB::bind_method(D_METHOD("is_forward"), &Spine::is_forward);
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
	ClassDB::bind_method(D_METHOD("set_collision_whitelist", "value"), &Spine::set_collision_whitelist);
	ClassDB::bind_method(D_METHOD("is_collision_whitelist"), &Spine::is_collision_whitelist);
	ClassDB::bind_method(D_METHOD("set_collision_layer", "collision_layer"), &Spine::set_collision_layer);
	ClassDB::bind_method(D_METHOD("get_collision_layer"), &Spine::get_collision_layer);
	ClassDB::bind_method(D_METHOD("set_collision_mask", "collision_mask"), &Spine::set_collision_mask);
	ClassDB::bind_method(D_METHOD("get_collision_mask"), &Spine::get_collision_mask);
	

	ClassDB::bind_method(D_METHOD("add_internal_animations"), &Spine::add_internal_animations);
	ClassDB::bind_method(D_METHOD("add_animation_by_attribute", "animation_attri"), &Spine::add_animation_by_attribute);
	ClassDB::bind_method(D_METHOD("add_animation", "name", "track", "loop", "delay"), &Spine::add_animation);
	ClassDB::bind_method(D_METHOD("add_empty_animation", "track", "mix", "delay"), &Spine::add_empty_animation);
	ClassDB::bind_method(D_METHOD("get_animations"), &Spine::get_animations);

	ClassDB::bind_method(D_METHOD("is_end"), &Spine::is_end);
	ClassDB::bind_method(D_METHOD("clear_tracks"), &Spine::clear_tracks);
	ClassDB::bind_method(D_METHOD("clear_track", "track"), &Spine::clear_track);
	/*ClassDB::bind_method(D_METHOD("get_animation_names"), &Spine::get_animation_names);
	ClassDB::bind_method(D_METHOD("has_animation", "name"), &Spine::has_animation);

	ClassDB::bind_method(D_METHOD("set_default_mix", "duration"), &Spine::set_default_mix);
	ClassDB::bind_method(D_METHOD("mix", "from", "to", "duration"), &Spine::mix, 0);
	ClassDB::bind_method(D_METHOD("play", "name", "loop", "track", "delay"), &Spine::play, 1.0f, false, 0, 0);
	ClassDB::bind_method(D_METHOD("add", "name", "loop", "track", "delay"), &Spine::add, 1.0f, false, 0, 0);
	ClassDB::bind_method(D_METHOD("clear", "track"), &Spine::clear);
	ClassDB::bind_method(D_METHOD("stop"), &Spine::stop);
	ClassDB::bind_method(D_METHOD("is_playing", "track"), &Spine::is_playing, DEFVAL(0));

	ClassDB::bind_method(D_METHOD("get_current_animation", "p_track"), &Spine::get_current_animation, DEFVAL(0));
	ClassDB::bind_method(D_METHOD("stop_all"), &Spine::stop_all);
	ClassDB::bind_method(D_METHOD("reset"), &Spine::reset);
	ClassDB::bind_method(D_METHOD("seek", "pos"), &Spine::seek);
	ClassDB::bind_method(D_METHOD("tell"), &Spine::tell);
	ClassDB::bind_method(D_METHOD("set_active", "active"), &Spine::set_active);
	ClassDB::bind_method(D_METHOD("is_active"), &Spine::is_active);
	ClassDB::bind_method(D_METHOD("get_animation_length", "animation"), &Spine::get_animation_length);
	ClassDB::bind_method(D_METHOD("set_speed", "speed"), &Spine::set_speed);
	ClassDB::bind_method(D_METHOD("get_speed"), &Spine::get_speed);
	ClassDB::bind_method(D_METHOD("set_skip_frames", "frames"), &Spine::set_skip_frames);
	ClassDB::bind_method(D_METHOD("get_skip_frames"), &Spine::get_skip_frames);
	ClassDB::bind_method(D_METHOD("set_flip_x", "fliped"), &Spine::set_flip_x);
	ClassDB::bind_method(D_METHOD("is_flip_x"), &Spine::is_flip_x);
	ClassDB::bind_method(D_METHOD("set_flip_y", "fliped"), &Spine::set_flip_y);
	ClassDB::bind_method(D_METHOD("is_flip_y"), &Spine::is_flip_y);
	ClassDB::bind_method(D_METHOD("set_skin", "skin"), &Spine::set_skin);
	ClassDB::bind_method(D_METHOD("set_duration", "p_duration"), &Spine::set_duration);
	ClassDB::bind_method(D_METHOD("get_duration"), &Spine::get_duration);
	
	ClassDB::bind_method(D_METHOD("get_skeleton"), &Spine::get_skeleton);
	ClassDB::bind_method(D_METHOD("get_attachment", "slot_name", "attachment_name"), &Spine::get_attachment);
	ClassDB::bind_method(D_METHOD("get_bone", "bone_name"), &Spine::get_bone);
	ClassDB::bind_method(D_METHOD("get_slot", "slot_name"), &Spine::get_slot);
	ClassDB::bind_method(D_METHOD("set_attachment", "slot_name", "attachment"), &Spine::set_attachment);*/

	ClassDB::bind_method(D_METHOD("get_attachment_node_bone", "node"), &Spine::get_attachment_node_bone);
	ClassDB::bind_method(D_METHOD("get_attachment_nodes", "bone_name"), &Spine::get_attachment_nodes);
	ClassDB::bind_method(D_METHOD("add_attachment_node", "bone_name", "node", "offset", "scale", "rotation", "flag"), &Spine::add_attachment_node, DEFVAL(Vector2(0, 0)), DEFVAL(Vector2(1, 1)), DEFVAL(0), DEFVAL(ATTACHMENT_BONE_FLAG_ALL));
	ClassDB::bind_method(D_METHOD("remove_attachment_node", "node"), &Spine::remove_attachment_node);
	ClassDB::bind_method(D_METHOD("get_attachment_node_offset", "node"), &Spine::get_attachment_node_offset);
	ClassDB::bind_method(D_METHOD("get_attachment_node_rotation", "node"), &Spine::get_attachment_node_rotation);
	ClassDB::bind_method(D_METHOD("get_attachment_node_scale", "node"), &Spine::get_attachment_node_scale);
	ClassDB::bind_method(D_METHOD("set_attachment_node_offset", "node", "offset"), &Spine::set_attachment_node_offset);
	ClassDB::bind_method(D_METHOD("set_attachment_node_rotation", "node", "rotation"), &Spine::set_attachment_node_rotation);
	ClassDB::bind_method(D_METHOD("set_attachment_node_scale", "node", "scale"), &Spine::set_attachment_node_scale);
	ClassDB::bind_method(D_METHOD("seek", "delta"), &Spine::seek);
	ClassDB::bind_method(D_METHOD("get_play_time"), &Spine::get_play_time);

	/*ClassDB::bind_method(D_METHOD("get_bounding_box", "slot_name", "attachment_name"), &Spine::get_bounding_box);
	ClassDB::bind_method(D_METHOD("add_bounding_box", "bone_name", "slot_name", "attachment_name", "collision_object_2d", "ofs", "scale", "rot"), &Spine::add_bounding_box, Vector2(0, 0), Vector2(1, 1), 0);
	ClassDB::bind_method(D_METHOD("remove_bounding_box", "bone_name", "collision_object_2d"), &Spine::remove_bounding_box);

	ClassDB::bind_method(D_METHOD("set_fx_slot_prefix", "prefix"), &Spine::set_fx_slot_prefix);
	ClassDB::bind_method(D_METHOD("get_fx_slot_prefix"), &Spine::get_fx_slot_prefix);

	ClassDB::bind_method(D_METHOD("set_debug_bones", "enable"), &Spine::set_debug_bones);
	ClassDB::bind_method(D_METHOD("is_debug_bones"), &Spine::is_debug_bones);
	ClassDB::bind_method(D_METHOD("set_debug_attachment", "mode", "enable"), &Spine::set_debug_attachment);
	ClassDB::bind_method(D_METHOD("is_debug_attachment", "mode"), &Spine::is_debug_attachment);

	ClassDB::bind_method(D_METHOD("set_debug_attachment_region", "enable"), &Spine::set_debug_attachment_region);
	ClassDB::bind_method(D_METHOD("is_debug_attachment_region"), &Spine::is_debug_attachment_region);
	ClassDB::bind_method(D_METHOD("set_debug_attachment_mesh", "enable"), &Spine::set_debug_attachment_mesh);
	ClassDB::bind_method(D_METHOD("is_debug_attachment_mesh"), &Spine::is_debug_attachment_mesh);
	ClassDB::bind_method(D_METHOD("set_debug_attachment_skinned_mesh", "enable"), &Spine::set_debug_attachment_skinned_mesh);
	ClassDB::bind_method(D_METHOD("is_debug_attachment_skinned_mesh"), &Spine::is_debug_attachment_skinned_mesh);
	ClassDB::bind_method(D_METHOD("set_debug_attachment_bounding_box", "enable"), &Spine::set_debug_attachment_bounding_box);
	ClassDB::bind_method(D_METHOD("is_debug_attachment_bounding_box"), &Spine::is_debug_attachment_bounding_box);

	ClassDB::bind_method(D_METHOD("_on_fx_draw"), &Spine::_on_fx_draw);*/

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "resource", PROPERTY_HINT_RESOURCE_TYPE, "SpineSkeletonData", PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_EDITOR), "set_resource", "get_resource");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "internal_animations", PROPERTY_HINT_TYPE_STRING, "17/17:SpineAnimationAttri"), "set_internal_animations", "get_internal_animations");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "process_mode", PROPERTY_HINT_ENUM, "Fixed,Idle"), "set_animation_process_mode", "get_animation_process_mode");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "playing"), "set_playing", "is_playing");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "auto_play"), "set_auto_play", "is_auto_play");
	//ADD_PROPERTY(PropertyInfo(Variant::BOOL, "forward"), "set_forward", "is_forward");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "show_bone"), "set_show_bone", "is_show_bone");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "speed_scale"), "set_speed_scale", "get_speed_scale");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "offset"), "set_spine_offset", "get_spine_offset");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "scale"), "set_spine_scale", "get_spine_scale");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "default_mix"), "set_default_mix", "get_default_mix");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "collision_whitelist"), "set_collision_whitelist", "is_collision_whitelist");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_layer", PROPERTY_HINT_LAYERS_2D_PHYSICS), "set_collision_layer", "get_collision_layer");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_mask", PROPERTY_HINT_LAYERS_2D_PHYSICS), "set_collision_mask", "get_collision_mask");
	/*ADD_PROPERTY(PropertyInfo(Variant::REAL, "speed", PROPERTY_HINT_RANGE, "-64,64,0.01"), "set_speed", "get_speed");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "active"), "set_active", "is_active");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "skip_frames", PROPERTY_HINT_RANGE, "0, 100, 1"), "set_skip_frames", "get_skip_frames");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug_bones"), "set_debug_bones", "is_debug_bones");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug_attachment_region"), "set_debug_attachment_region", "is_debug_attachment_region");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug_attachment_mesh"), "set_debug_attachment_mesh", "is_debug_attachment_mesh");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug_attachment_skinned_mesh"), "set_debug_attachment_skinned_mesh", "is_debug_attachment_skinned_mesh");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "debug_attachment_bounding_box"), "set_debug_attachment_bounding_box", "is_debug_attachment_bounding_box");

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "flip_x"), "set_flip_x", "is_flip_x");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "flip_y"), "set_flip_y", "is_flip_y");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "fx_prefix"), "set_fx_slot_prefix", "get_fx_slot_prefix");*/
	
	//ADD_PROPERTY(PropertyInfo(Variant::STRING, "skin", PROPERTY_HINT_ENUM, "default"), "set_skin", "get_skin");
	//ADD_PROPERTY(PropertyInfo(Variant::REAL, "playback/duration", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_EDITOR), "set_duration", "get_duration");

	ADD_SIGNAL(MethodInfo("animation_started", PropertyInfo(Variant::OBJECT, "animation")));
	ADD_SIGNAL(MethodInfo("animation_interrupted", PropertyInfo(Variant::OBJECT, "animation")));
	ADD_SIGNAL(MethodInfo("animation_completed", PropertyInfo(Variant::OBJECT, "animation")));
	ADD_SIGNAL(MethodInfo("animation_ended", PropertyInfo(Variant::OBJECT, "animation")));
	ADD_SIGNAL(MethodInfo("animation_event", PropertyInfo(Variant::OBJECT, "animation"), PropertyInfo(Variant::STRING, "event"), PropertyInfo(Variant::INT, "intParam"), PropertyInfo(Variant::REAL, "floatParam"), PropertyInfo(Variant::STRING, "strParam")));
	ADD_SIGNAL(MethodInfo("ended"));


	/*BIND_ENUM_CONSTANT(ANIMATION_PROCESS_FIXED);
	BIND_ENUM_CONSTANT(ANIMATION_PROCESS_IDLE);

	BIND_ENUM_CONSTANT(DEBUG_ATTACHMENT_REGION);
	BIND_ENUM_CONSTANT(DEBUG_ATTACHMENT_MESH);
	BIND_ENUM_CONSTANT(DEBUG_ATTACHMENT_SKINNED_MESH);
	BIND_ENUM_CONSTANT(DEBUG_ATTACHMENT_BOUNDING_BOX);*/
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
#endif
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
			//case SP_ATTACHMENT_BOUNDING_BOX:
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

Spine::Spine() :
		batcher(this), skeleton(nullptr), state(nullptr)
		, playing(false), auto_play(false), /*forward(true), */show_bone(false), speed_scale(1.0)
		, spine_offset(0.0, 0.0), spine_scale(1.0, 1.0), default_mix(0.0), collision_whitelist(true)
		, collision_layer(1), collision_mask(1)
//		, root_bone(nullptr)
//		, fx_node(memnew(Node2D)), fx_batcher(fx_node)
{
	res = RES();
	//world_verts.resize(1000); // Max number of vertices per mesh.
	world_verts.resize(8);
	memset(world_verts.ptrw(), 0, world_verts.size() * sizeof(float));
	/*speed_scale = 1;
	autoplay = "";
	animation_process_mode = ANIMATION_PROCESS_IDLE;
	processing = false;
	active = true;
	playing = false;
	forward = true;
	process_delta = 0;
	skip_frames = 0;
	frames_to_skip = 0;

	debug_bones = false;
	debug_attachment_region = false;
	debug_attachment_mesh = false;
	debug_attachment_skinned_mesh = false;
	debug_attachment_bounding_box = false;

	skin = "";
	current_animation = "[stop]";
	duration = 0.0;
	actual_duration = 0.0;
	loop = true;
	fx_slot_prefix = String("fx/").utf8();

	modulate = Color(1, 1, 1, 1);
	flip_x = false;
	flip_y = false;*/
}

Spine::~Spine() {

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

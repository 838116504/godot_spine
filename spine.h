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
#ifndef SPINE_H
#define SPINE_H

#include "core/array.h"
#include "scene/2d/node_2d.h"
#include "spine_batcher.h"
#include "spine_animation.h"
#include "spine_animation_attri.h"
#include "spine_resource.h"
#include "scene/2d/collision_polygon_2d.h"

class Spine : public Node2D {

	GDCLASS(Spine, Node2D);

public:
	enum AnimationProcessMode {
		ANIMATION_PROCESS_FIXED,
		ANIMATION_PROCESS_IDLE,
	};
	enum AttachmentBoneFlag {
		ATTACHMENT_BONE_FLAG_OFFSET = 1,
		ATTACHMENT_BONE_FLAG_SCALE = 2,
		ATTACHMENT_BONE_FLAG_ROTATION = 4,
		ATTACHMENT_BONE_FLAG_ALL = ATTACHMENT_BONE_FLAG_OFFSET | ATTACHMENT_BONE_FLAG_SCALE | ATTACHMENT_BONE_FLAG_ROTATION
	};
	/*enum DebugAttachmentMode {
		DEBUG_ATTACHMENT_REGION,
		DEBUG_ATTACHMENT_MESH,
		DEBUG_ATTACHMENT_SKINNED_MESH,
		DEBUG_ATTACHMENT_BOUNDING_BOX,
	};*/

	
	

private:
	Ref<SpineSkeletonData> res;

	spSkeleton *skeleton;
	//spBone *root_bone;
	spAnimationState *state;
	mutable Vector<float> world_verts;
	
	AnimationProcessMode animation_process_mode;
	bool playing;
	bool auto_play;
	//bool forward;
	Array internal_animations;
	SpineBatcher batcher;
	bool show_bone;
	float speed_scale;
	Vector2 spine_offset;
	Vector2 spine_scale;
	float default_mix;
	bool collision_whitelist;
	uint32_t collision_layer;
	uint32_t collision_mask;
	PoolStringArray collision_list
	//String autoplay;
	//bool processing;
	//bool active;
	
	//int skip_frames;
	//int frames_to_skip;
	//float process_delta;
	//bool debug_bones;
	//bool debug_attachment_region;
	//bool debug_attachment_mesh;
	//bool debug_attachment_skinned_mesh;
	//bool debug_attachment_bounding_box;
	//String current_animation;
	//float duration; // Handled as a property, but never set in the setter
	//float actual_duration; // Store the actual length of the animation
	//bool loop;
	//String skin;

	//Color modulate;
	//bool flip_x, flip_y;
	

	//// fx slots (always show on top)
	//Node2D *fx_node;
	//SpineBatcher fx_batcher;
	//CharString fx_slot_prefix;

	//float current_pos;

	typedef struct AttachmentNode {
		List<AttachmentNode>::Element *E;
		spBone *bone;
		WeakRef *ref;
		Vector2 ofs;
		Vector2 scale;
		real_t rot;
		int flag;
	} AttachmentNode;
	typedef List<AttachmentNode> AttachmentNodes;
	AttachmentNodes attachment_nodes;


	static void spine_animation_callback(spAnimationState *p_state, spEventType p_type, spTrackEntry *p_track, spEvent *p_event);
	void _on_animation_state_event(spTrackEntry * p_track, spEventType p_type, spEvent *p_event);

	void _animation_process(float p_delta);
	void _animation_draw();
	//void _set_process(bool p_process, bool p_force = false);
	//void _on_fx_draw();
	void _update_verties_count();

	void _spine_dispose();
	void _spine_create();
protected:
	//static Array *invalid_names;

	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;
	void _notification(int p_what);

	static void _bind_methods();

public:
	//static Array get_invalid_names();

	// set/get spine resource
	void set_resource(Ref<SpineSkeletonData> p_data);
	Ref<SpineSkeletonData> get_resource();
	void resource_changed(Ref<Resource> p_res);
	PoolStringArray get_skins();
	bool has_skin(const String& p_skin);
	String get_skin() const;
	void set_skin(const String& p_skin);

	Array get_internal_animations() const;
	void set_internal_animations(Array p_animations);
	void set_animation_process_mode(AnimationProcessMode p_mode);
	AnimationProcessMode get_animation_process_mode() const;
	void set_playing(bool p_playing);
	bool is_playing() const;
	void set_auto_play(bool p_auto);
	bool is_auto_play() const;
	//oid set_forward(bool p_forward);
	//bool is_forward() const;
	void set_show_bone(bool p_show);
	bool is_show_bone() const;
	void set_speed_scale(bool p_scale);
	float get_speed_scale() const;
	void set_spine_offset(Vector2 p_offset);
	Vector2 get_spine_offset() const;
	void set_spine_scale(Vector2 p_scale);
	Vector2 get_spine_scale() const;
	void set_default_mix(float p_mix);
	float get_default_mix() const;
	void set_collision_whitelist(bool p_value);
	bool is_collision_whitelist() const;
	void set_collision_layer(uint32_t p_layer);
	uint32_t get_collision_layer() const;
	void set_collision_mask(uint32_t p_mask);
	uint32_t get_collision_mask() const;
	

	void add_internal_animations();
	void add_animation_by_attribute(Ref<SpineAnimationAttri> p_anim);
	SpineAnimation* add_animation(const String& p_name, int p_track, bool p_loop, float p_delay);
	SpineAnimation* add_empty_animation(int p_track, float p_mix, float p_delay);
	PoolStringArray get_animations() const;

	bool is_end() const;
	void clear_tracks();
	void clear_track(int p_track);
	//Array get_animation_names() const;

	//bool has_animation(const String &p_name);
	//void set_default_mix(real_t p_duration);
	//void mix(const String &p_from, const String &p_to, real_t p_duration);

	//bool play(const String &p_name, bool p_loop = false, int p_track = 0, int p_delay = 0);
	//bool add(const String &p_name, bool p_loop = false, int p_track = 0, int p_delay = 0);
	//void clear(int p_track = -1);
	//void stop();
	//bool is_playing(int p_track = 0) const;
	//float get_animation_length(String p_animation) const;
	
	//void set_skip_frames(int p_skip_frames);
	//int get_skip_frames() const;
	//String get_current_animation(int p_track);
	//void stop_all();
	void reset();
	//void seek(float p_pos);
	//float tell() const;

	//void set_active(bool p_active);
	//bool is_active() const;

	//void set_speed(float p_speed);
	//float get_speed() const;

	//void set_autoplay(const String &p_name);
	//String get_autoplay() const;

	//void set_modulate(const Color &p_color);
	//Color get_modulate() const;

	//void set_flip_x(bool p_flip);
	//void set_flip_y(bool p_flip);
	//bool is_flip_x() const;
	//bool is_flip_y() const;

	//void set_duration(float p_duration);
	//float get_duration() const;

	

	///* Sets the skin used to look up attachments not found in the SkeletonData defaultSkin. Attachments from the new skin are
	//* attached if the corresponding attachment from the old skin was attached. If there was no old skin, each slot's setup mode
	//* attachment is attached from the new skin. Returns false if the skin was not found.
	//* @param skin May be 0.*/
	//bool set_skin(const String &p_name);

	////spAttachment* get_attachment(const char* slotName, const char* attachmentName) const;
	//Dictionary get_skeleton() const;
	///* Returns null if the slot or attachment was not found. */
	//Dictionary get_attachment(const String &p_slot_name, const String &p_attachment_name) const;
	///* Returns null if the bone was not found. */
	//Dictionary get_bone(const String &p_bone_name) const;
	///* Returns null if the slot was not found. */
	//Dictionary get_slot(const String &p_slot_name) const;
	///* Returns false if the slot or attachment was not found. */
	//bool set_attachment(const String &p_slot_name, const Variant &p_attachment);
	//// bind node to bone, auto update pos/rotate/scale
	String get_attachment_node_bone(const Variant &p_node) const;
	Array get_attachment_nodes(const String &p_node) const;
	bool add_attachment_node(const String &p_bone_name, const Variant &p_node, Vector2 p_ofs = Vector2(0, 0), Vector2 p_scale = Vector2(1, 1), real_t p_rot = 0, int p_flag = ATTACHMENT_BONE_FLAG_ALL);
	bool remove_attachment_node(const Variant &p_node);
	Vector2 get_attachment_node_offset(const Variant& p_node) const;
	float get_attachment_node_rotation(const Variant& p_node) const;
	Vector2 get_attachment_node_scale(const Variant& p_node) const;
	void set_attachment_node_offset(Variant& p_node, Vector2 p_offset);
	void set_attachment_node_rotation(Variant& p_node, real_t p_rotation);
	void set_attachment_node_scale(Variant& p_node, Vector2 p_scale);
	//// get spine bounding box
	//Ref<Shape2D> get_bounding_box(const String &p_slot_name, const String &p_attachment_name);
	//// bind collision object 2d to spine bounding box
	//bool add_bounding_box(const String &p_bone_name, const String &p_slot_name, const String &p_attachment_name, const Variant &p_node, const Vector2 &p_ofs = Vector2(0, 0), const Vector2 &p_scale = Vector2(1, 1), const real_t p_rot = 0);
	//bool remove_bounding_box(const String &p_bone_name, const Variant &p_node);

	//void set_fx_slot_prefix(const String &p_prefix);
	//String get_fx_slot_prefix() const;

	//void set_debug_bones(bool p_enable);
	//bool is_debug_bones() const;
	//void set_debug_attachment(DebugAttachmentMode p_mode, bool p_enable);
	//bool is_debug_attachment(DebugAttachmentMode p_mode) const;

	//void set_debug_attachment_region(bool p_enable);
	//bool is_debug_attachment_region() const;
	//void set_debug_attachment_mesh(bool p_enable);
	//bool is_debug_attachment_mesh() const;
	//void set_debug_attachment_skinned_mesh(bool p_enable);
	//bool is_debug_attachment_skinned_mesh() const;
	//void set_debug_attachment_bounding_box(bool p_enable);
	//bool is_debug_attachment_bounding_box() const;

	void seek(float p_time);
	float get_play_time() const;
	//void seek_delta(float p_time, float p_delta);
	//float get_current_animation_pos() const;
	//float get_current_animation_length() const;

	//void advance(float p_time);
#ifdef TOOLS_ENABLED
	virtual Rect2 _edit_get_rect() const;
	virtual bool _edit_use_rect() const;
#endif
	Spine();
	virtual ~Spine();
};
//
VARIANT_ENUM_CAST(Spine::AnimationProcessMode);
//VARIANT_ENUM_CAST(Spine::DebugAttachmentMode);

#endif // SPINE_H

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
		ANIMATION_PROCESS_FIXED = 0,
		ANIMATION_PROCESS_IDLE,
	};
	enum AttachmentNodeFlag {
		ATTACHMENT_NODE_FLAG_OFFSET = 1,
		ATTACHMENT_NODE_FLAG_SCALE = 2,
		ATTACHMENT_NODE_FLAG_ROTATION = 4,
		ATTACHMENT_NODE_FLAG_ALL = ATTACHMENT_NODE_FLAG_OFFSET | ATTACHMENT_NODE_FLAG_SCALE | ATTACHMENT_NODE_FLAG_ROTATION
	};
	
	

private:
	Ref<SpineSkeletonData> res;

	spSkeleton *skeleton;
	spAnimationState *state;
	mutable Vector<float> world_verts;
	
	AnimationProcessMode animation_process_mode;
	bool playing;
	bool auto_play;
	Array internal_animations;
	SpineBatcher batcher;
	bool show_bone;
	float speed_scale;
	Vector2 spine_offset;
	Vector2 spine_scale;
	float default_mix;
	/*bool collision_whitelist;*/
	/*PoolStringArray collision_list;*/
	bool use_bounding_box;

	class Polygon
	{
	private:
		WeakRef ref;
	public:
		Polygon& operator= (const Polygon& rhs) {
			if (this != &rhs)
				ref.set_obj(rhs.ref.get_ref());
			return *this;
		};
		_FORCE_INLINE_ CollisionPolygon2D* get_polygon() { return cast_to<CollisionPolygon2D>(ref.get_ref()); };
		_FORCE_INLINE_ void set_polygon(CollisionPolygon2D* p_polygon) { ref.set_obj(p_polygon); };
		_FORCE_INLINE_ Polygon() :ref() {};
		_FORCE_INLINE_ ~Polygon() {
			Object* obj = ref.get_ref();
			if (obj) obj->call_deferred("free");
		};
	};
	typedef List<Polygon> BoundingBox;
	BoundingBox _bounding_boxs;
	/*typedef struct {
		spSlot* slot;
		BoundingBox polygons;
	} SlotCollisionPolygons;
	typedef List<SlotCollisionPolygons> SlotPolygons;
	SlotPolygons _polygons;*/

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
	void _update_verties_count();
	void _update_attachment_node();
	void _update_bounding_box();
	typedef struct {
		bool isAdd;
		String anim;
		int track;
		union {
			bool loop;
			float mix;
		};
		float delay;
	} AddAnimationData;
	SpineAnimation* _add_animation(const AddAnimationData& p_data);
	//void _update_polygons();

	bool is_end() const;

	void _spine_dispose();
	void _spine_create();

	void reset();
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
	bool set_skin(const String& p_skin);

	Array get_internal_animations() const;
	void set_internal_animations(Array p_animations);
	void set_animation_process_mode(AnimationProcessMode p_mode);
	AnimationProcessMode get_animation_process_mode() const;
	void set_playing(bool p_playing);
	bool is_playing() const;
	void set_auto_play(bool p_auto);
	bool is_auto_play() const;
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
	bool set_mix(const String& p_from, const String& p_to, real_t p_value);
	real_t get_mix(const String& p_from, const String& p_to) const;
	//void set_collision_whitelist(bool p_value);
	//bool is_collision_whitelist() const;
	//void set_collision_list(const PoolStringArray& p_list);
	//PoolStringArray get_collision_list() const;
	void set_use_bounding_box(bool p_value);
	bool is_use_bounding_box() const;

	void add_internal_animations();
	SpineAnimation* add_animation_by_attribute(Ref<SpineAnimationAttri> p_attribute);
	SpineAnimation* add_animation(const String& p_name, bool p_loop, float p_delay, int p_track = 0);
	SpineAnimation* add_empty_animation(float p_mix, float p_delay, int p_track = 0);
	SpineAnimation* set_animation(const String& p_name, bool p_loop, int p_track = 0);
	SpineAnimation* set_empty_animation(float p_mix, int p_track = 0);
	bool has_animation(const String& p_name) const;
	PoolStringArray get_animations() const;
	
	real_t get_animation_duration(const String& p_name) const;
	SpineAnimation* get_current_animation(int p_track = 0);

	PoolStringArray get_bones() const;
	bool has_bone(const String& p_name) const;
	Transform2D get_bone_local_transform(const String& p_name) const;
	Transform2D get_bone_world_transform(const String& p_name) const;

	PoolStringArray get_slots() const;
	PoolStringArray get_slot_attachments(const String& p_name) const;
	bool has_slot_attachment(const String& p_slot, const String& p_attachment) const;
	bool set_slot_attachment(const String& p_slot, const String& p_attachment);
	String get_slot_attachment(const String& p_name) const;
	bool set_slot_color(const String& p_name, const Color& p_color);
	Color get_slot_color(const String& p_name) const;
	
	void clear(int p_track = -1);
	PoolIntArray get_tracks() const;
	
	// bind node to bone, auto update pos/rotate/scale
	String get_attachment_node_bone(const Object* p_node) const;
	Array get_attachment_nodes(const String &p_bone_name) const;
	bool add_attachment_node(const String &p_bone_name, Object* p_node, const Transform2D& p_transform = Transform2D(), int p_flag = ATTACHMENT_NODE_FLAG_ALL);
	bool remove_attachment_node(Object* p_node);
	int get_attachment_node_flag(const Object* p_node) const;
	Vector2 get_attachment_node_offset(const Object* p_node) const;
	real_t get_attachment_node_rotation(const Object* p_node) const;
	Vector2 get_attachment_node_scale(const Object* p_node) const;
	void set_attachment_node_flag(Object* p_node, int p_flag);
	void set_attachment_node_offset(Object* p_node, Vector2 p_offset);
	void set_attachment_node_rotation(Object* p_node, real_t p_rotation);
	void set_attachment_node_scale(Object* p_node, Vector2 p_scale);

	void seek(float p_time);
	float tell() const;

#ifdef TOOLS_ENABLED
	virtual Rect2 _edit_get_rect() const;
	virtual bool _edit_use_rect() const;
#endif
	Spine();
	virtual ~Spine();
};

VARIANT_ENUM_CAST(Spine::AnimationProcessMode);
VARIANT_ENUM_CAST(Spine::AttachmentNodeFlag);

#endif // SPINE_H

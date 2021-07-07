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

#ifdef TOOL_ENABLED
#include "spine.h"

#include "spine_animation_inspector_plugin.h"

#include "editor/editor_plugin.h"
#include "editor/editor_properties.h"

/*void SpineAnimationInspectorPlugin::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("_on_spine_resource_changed"), &SpineAnimationInspectorPlugin::_on_spine_resource_changed);
}*/

bool SpineAnimationInspectorPlugin::can_handle(Object *p_object)
{
	return true;
}

bool SpineAnimationInspectorPlugin::parse_property(Object *p_object, Variant::Type p_type, const String &p_path, PropertyHint p_hint, const String &p_hint_text, int p_usage)
{
	if (!p_object->is_class("SpineAnimationAttri") || p_type != Variant::STRING || p_path != "animation_name")
		return false;

	Spine* spine = Object::cast_to<Spine>(EditorInterface::get_singleton()->get_inspector()->get_edited_object());
	if (spine == NULL)
		return false;


	EditorPropertyTextEnum *editor = memnew(EditorPropertyTextEnum);
	PoolStringArray anims = spine->get_animation_list();
	Vector<String> options = anims.join(",").split(",");
	editor->setup(options);
	//editor->set_object_and_property(p_object, p_path);
	add_property_editor(p_path, editor);

	/*Vector<Variant> args;
	Vector<Variant> editors;

	if (spine->is_connected("resource_changed", this, "_on_spine_resource_changed"))
	{
		List<Connection> signalList;
		spine->get_signal_connection_list("resource_changed", &signalList);

		for (List<Connection>::Element* e = signalList.front(); e; e = e->next())
		{
			if (e->get().target == this)
			{
				Vector<Variant> binds = e->get().binds[1];
				for (int i = 0; i < binds.size(); ++i)
				{
					EditorPropertyTextEnum* temp = Object::cast_to<EditorPropertyTextEnum>(binds[i]);
					if (temp == NULL || !ObjectDB::instance_validate(temp))
					{
						continue;
					}

					editors.push_back(temp);
				}

				break;
			}
		}
		spine->disconnect("resource_changed", this, "_on_spine_resource_changed");
	}

	editors.push_back(editor);

	args.push_back(spine);
	args.push_back(editors);
	spine->connect("resource_changed", this, "_on_spine_resource_changed", args);*/

	return true;
}

/*void SpineAnimationInspectorPlugin::_on_spine_resource_changed(Node* p_spine, const Vector<Variant>& p_propEditors)
{
	Spine* spine = Object::cast_to<Spine>(p_spine);
	if (spine == NULL)
		return;


	for (int i = 0; i < p_propEditors.size(); ++i)
	{
		EditorPropertyTextEnum* editor = Object::cast_to<EditorPropertyTextEnum>(p_propEditors[i]);
		if (editor == NULL || !ObjectDB::instance_validate(editor))
			continue;

		//editor->get_child(0)->call("clear");

		//PoolStringArray anims = spine->get_animation_list();
		//Vector<String> options = anims.join(",").split(",");
		//editor->setup(options);
		//editor->update_property();
	}
}*/

#endif // TOOL_ENABLED


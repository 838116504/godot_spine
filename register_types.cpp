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

#include "register_types.h"

#include "core/class_db.h"
//#include "core/os/os.h"


#include <spine/extension.h>
//#include <spine/spine.h>

//#include "spine_animation.h"
#include "spine.h"

#ifdef TOOL_ENABLED
#include "spine_resource_format_loader.h"
#endif // TOOL_ENABLED

#include "core/os/file_access.h"
#include "scene/resources/texture.h"

#ifdef TOOL_ENABLED
#include "spine_animation_inspector_plugin.h"


static Ref<ResourceFormatLoaderSpineSkel> resource_loader_spine_skel;
static Ref<ResourceFormatLoaderSpineAtlas> resource_loader_spine_atlas;
static Ref<SpineAnimationInspectorPlugin> spine_animation_inspector_plugin;
#endif // TOOL_ENABLED

typedef Ref<Texture> TextureRef;

static void *spine_malloc(size_t p_size) {

	if (p_size == 0)
		return NULL;
	return memalloc(p_size);
}

static void *spine_realloc(void *ptr, size_t p_size) {

	if (p_size == 0)
		return NULL;
	return memrealloc(ptr, p_size);
}

static void spine_free(void *ptr) {

	if (ptr == NULL)
		return;
	memfree(ptr);
}
#ifdef DEBUG_ENABLED
static void spine_print_line(const char* str)
{
	print_line(str);
}
#endif

void register_spine_types()
{
	ClassDB::register_class<Spine>();
	ClassDB::register_class<SpineAtlas>();
	//ClassDB::register_class<SpineAnimation>();
	ClassDB::register_class<SpineAnimationAttri>();
	ClassDB::register_virtual_class<SpineSkeleton>();
	ClassDB::register_class<SpineJsonSkeleton>();
	ClassDB::register_class<SpineBinarySkeleton>();
	ClassDB::register_class<SpineSkeletonData>();
	ClassDB::register_class<SpineEvent>();

#ifdef TOOLS_ENABLED
	resource_loader_spine_skel.instance();
	ResourceLoader::add_resource_format_loader(resource_loader_spine_skel);
	resource_loader_spine_atlas.instance();
	ResourceLoader::add_resource_format_loader(resource_loader_spine_atlas);
	spine_animation_inspector_plugin.instance();
	EditorInspector::add_inspector_plugin(spine_animation_inspector_plugin);
#endif // TOOLS_ENABLED

	_spSetMalloc(spine_malloc);
	_spSetRealloc(spine_realloc);
	_spSetFree(spine_free);
//#ifdef DEBUG_ENABLED
//	_spSetPrintLine(spine_print_line);
//#endif
}

void unregister_spine_types()
{
#ifdef TOOLS_ENABLED
	ResourceLoader::remove_resource_format_loader(resource_loader_spine_skel);
	resource_loader_spine_skel.unref();
	ResourceLoader::remove_resource_format_loader(resource_loader_spine_atlas);
	resource_loader_spine_atlas.unref();
	EditorInspector::remove_inspector_plugin(spine_animation_inspector_plugin);
	spine_animation_inspector_plugin.unref();
#endif // TOOLS_ENABLED
}


void _spAtlasPage_createTexture(spAtlasPage* self, const char* path) {

	TextureRef *ref = memnew(TextureRef);
	*ref = ResourceLoader::load(path);
	ERR_FAIL_COND(ref->is_null());
	self->rendererObject = ref;
	self->width = (*ref)->get_width();
	self->height = (*ref)->get_height();
}

void _spAtlasPage_disposeTexture(spAtlasPage* self) {

	if(TextureRef *ref = static_cast<TextureRef *>(self->rendererObject))
		memdelete(ref);
}


char* _spUtil_readFile(const char* p_path, int* p_length) {

	String str_path = String::utf8(p_path);
	FileAccess *f = FileAccess::open(p_path, FileAccess::READ);
	if(!f) {
		ERR_PRINTS(String("Unable to read file :") + String(p_path));
	}
	ERR_FAIL_COND_V(!f, NULL);

	*p_length = f->get_len();

	char *data = (char *)_spMalloc(*p_length, __FILE__, __LINE__);
	//data = (char *)_spMalloc(*p_length, __FILE__, __LINE__);
	ERR_FAIL_COND_V(data == NULL, NULL);

	f->get_buffer((uint8_t *)data, *p_length);

	/*
	 * A pretty hacky part of patching Spine files
	 * Preface: If I create spine atlas from images with some deep
	 *			foldres structure (e.g. head/eyes.png, head/face.png, body/torso.png, ets)
	 *			I can get a bit buggy ordering in animations.
	 *			This mostly because of names with slashes in resulting atlas and attachments
	 *			(head/eyes, head/face, body/torso, etc.)
	 * I'm to lazy to make any spine research and have no time for waiting this issue
	 * to be resolved, so I just remember all names with slashes in atlas, replace slashes with
	 * '-' and make same thing with skeleton (.json or .skel) for matching names.
	 *
	 * It works good for me, but may be buggy in your case.
	 * I promice:
	 * - to create issue about this github
	 * - add option to avoid this hacky part available in scons (eg hacky_spine=no)
	 * - may be find actual reason of my buggy animations
	 *
	 */
	/*
	Array _sp_invalid_names = Spine::get_invalid_names();
	if (str_path.ends_with(".atlas")){
		Array current_string = Array();
		bool is_invalid = false;
		for (int i=0; i<*p_length; i++){
			if (data[i] == '\r') continue;
			if (data[i] == '\n'){
				if (is_invalid) {
					//print_line("Add invalid index: " + current_string);
					_sp_invalid_names.append(current_string);
				}
				current_string = Array();
				is_invalid = false;
			} else {
				current_string.append(data[i]);
			}
			if (data[i]=='/') {
				is_invalid = true;
				data[i] = '-';
			}
		}
	}
	if (_sp_invalid_names.size() && (str_path.ends_with(".json") || str_path.ends_with(".skel"))){
		for (int i=0; i<*p_length; i++){
			if (data[i] != '/') continue;
			for (int j=0; j<_sp_invalid_names.size(); j++){
				Array in = _sp_invalid_names[j];
				int idx = in.find('/');
				bool match = true;
				for (int k=0;k<idx;k++){
					if (i<k) { match=false; break; }
					if (data[i-k] != (int)(in[idx-k])){ match=false; break; }
				}
				if (!match) continue;

				Vector<int> extra_replaces;
				for (int k=1; k<in.size()-idx;k++){
					if (i+k >= *p_length){ match=false; break; }
					if (data[i+k] != (int)(in[idx+k])) { match=false; break; }
					if (data[i+k] == '/') extra_replaces.push_back(i+k);
				}
				if (match){
					//print_line("Fix invalid index: " + in + ", replaces: " + itos(extra_replaces.size()));
					data[i] = '-';
					for (int k=0; k<extra_replaces.size(); k++) data[extra_replaces[k]] = '-';
					break;
				}
			}
		}

		_sp_invalid_names.resize(0);
	}*/
	memdelete(f);
	return data;
}



This branch changed many things.

I dont test any thing, so it maybe have bugs.

Current Spine runtime version for this branch ** 3.8 ** ([Github link](https://github.com/EsotericSoftware/spine-runtimes/tree/3.8/spine-c)).
This branch support for godot game engine 3.2.1

## Spine Runtime changed by myself

Fix the problem when load json that have skin no attachment(SkeletonJson.c >> spSkeletonJson_readSkeletonData function changed).

Modify spAnimationStateData_setMixByName function return type to int for check whether set success(AnimationStateData.c and AnimationStateData.h changed).

Added 6 add/set animation function for set spTrackEntry's userData before animation start.(AnimationState.h and AnimationState.c >> spAnimationState_setAnimationByNameWithData, spAnimationState_setAnimationByName, spAnimationState_setAnimationWithData, spAnimationState_setAnimation, spAnimationState_addAnimationByName, spAnimationState_addAnimationByNameWithData, spAnimationState_addAnimation, spAnimationState_addAnimationWithData functions changed.).

## Chinese doc(unfinished)
[Spine](https://shimo.im/docs/y8Pwd36PwxkJTjw3/)
[SpineAnimationAttri](https://shimo.im/docs/qvyHdkRQ9tyctWhc/)
[SpineAnimation](https://shimo.im/docs/pVjY9hGQr36pCG6c/)
[SpineSkeletonData](https://shimo.im/docs/JWD6y866DT9wyCxC/)
[SpineAtlas](https://shimo.im/docs/q8qhR3yKDccdXCWp/)
[SpineBinarySkeleton](https://shimo.im/docs/jWVvccdtv6PKVDrk/)
[SpineJsonSkeleton](https://shimo.im/docs/xXYQrr9GW6TkHHCv/)
[SpineSkeleton](https://shimo.im/docs/3YtjHxKTW6KdXwJG/)

## About the license

This module is forked from [sanikoyes's godot branch](https://github.com/sanikoyes/godot/tree/develop/modules/spine) and some of the code is forked from [godot-spine-module](https://github.com/jjay/godot-spine-module). Both of the code are declared as MIT license.

The license of this module is under the [Spine Runtimes Software License](https://github.com/EsotericSoftware/spine-runtimes/blob/3.6/LICENSE).

## Usage

Add this code under `modules/spine` in your Godot source tree. You may either copy it or use `git submodule add`.

Build Godot using `scons platform=x11 tools=yes target=release_debug` or whatever [build options](http://docs.godotengine.org/en/latest/development/compiling/) you prefer.

Use the `Spine` type in your scene tree and load your animation into it as a resource.

## Further reference

The [Spine API Reference](http://esotericsoftware.com/spine-api-reference) is useful to learning more about how the code works.


/**
 * Copyright 2023 wmbat
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#pragma once

// Normally we don't export class templates (but do complete specializations),
// inline functions, and classes with only inline member functions. Exporting
// classes that inherit from non-exported/imported bases (e.g., std::string)
// will end up badly. The only known workarounds are to not inherit or to not
// export. Also, MinGW GCC doesn't like seeing non-exported functions being
// used before their inline definition. The workaround is to reorder code. In
// the end it's all trial and error.

#if defined(LIBPHYSENG_STATIC)         // Using static.
#  define LIBPHYSENG_SYMEXPORT
#elif defined(LIBPHYSENG_STATIC_BUILD) // Building static.
#  define LIBPHYSENG_SYMEXPORT
#elif defined(LIBPHYSENG_SHARED)       // Using shared.
#  ifdef _WIN32
#    define LIBPHYSENG_SYMEXPORT __declspec(dllimport)
#  else
#    define LIBPHYSENG_SYMEXPORT
#  endif
#elif defined(LIBPHYSENG_SHARED_BUILD) // Building shared.
#  ifdef _WIN32
#    define LIBPHYSENG_SYMEXPORT __declspec(dllexport)
#  else
#    define LIBPHYSENG_SYMEXPORT
#  endif
#else
// If none of the above macros are defined, then we assume we are being used
// by some third-party build system that cannot/doesn't signal the library
// type. Note that this fallback works for both static and shared libraries
// provided the library only exports functions (in other words, no global
// exported data) and for the shared case the result will be sub-optimal
// compared to having dllimport. If, however, your library does export data,
// then you will probably want to replace the fallback with the (commented
// out) error since it won't work for the shared case.
//
#  define LIBPHYSENG_SYMEXPORT         // Using static or shared.
//#  error define LIBPHYSENG_STATIC or LIBPHYSENG_SHARED preprocessor macro to signal libphyseng library type being linked
#endif

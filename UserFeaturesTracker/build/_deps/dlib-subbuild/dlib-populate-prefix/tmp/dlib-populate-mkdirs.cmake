# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/zavel/Desktop/GItHubProjects/UserFeaturesTracker/UserFeaturesTracker/build/_deps/dlib-src"
  "C:/Users/zavel/Desktop/GItHubProjects/UserFeaturesTracker/UserFeaturesTracker/build/_deps/dlib-build"
  "C:/Users/zavel/Desktop/GItHubProjects/UserFeaturesTracker/UserFeaturesTracker/build/_deps/dlib-subbuild/dlib-populate-prefix"
  "C:/Users/zavel/Desktop/GItHubProjects/UserFeaturesTracker/UserFeaturesTracker/build/_deps/dlib-subbuild/dlib-populate-prefix/tmp"
  "C:/Users/zavel/Desktop/GItHubProjects/UserFeaturesTracker/UserFeaturesTracker/build/_deps/dlib-subbuild/dlib-populate-prefix/src/dlib-populate-stamp"
  "C:/Users/zavel/Desktop/GItHubProjects/UserFeaturesTracker/UserFeaturesTracker/build/_deps/dlib-subbuild/dlib-populate-prefix/src"
  "C:/Users/zavel/Desktop/GItHubProjects/UserFeaturesTracker/UserFeaturesTracker/build/_deps/dlib-subbuild/dlib-populate-prefix/src/dlib-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/zavel/Desktop/GItHubProjects/UserFeaturesTracker/UserFeaturesTracker/build/_deps/dlib-subbuild/dlib-populate-prefix/src/dlib-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/zavel/Desktop/GItHubProjects/UserFeaturesTracker/UserFeaturesTracker/build/_deps/dlib-subbuild/dlib-populate-prefix/src/dlib-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()

# Changelog
All notable changes to this package will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [2.0.1] - 2022-02-28

### Changed
  - Disable video streaming on Apple silicon.

### Fixed
  - Make sure the Virtual Camera recordings always include the initial lens values.

## [2.0.0-pre.3] - 2021-10-26

### Added
  - Timecode synchronization
  - The client can select the Take to play, and edit its metadata.  
  
### Changed
  - TakeRecorder improvements to better handle device status.
  - Instead of using AnimationJobs, the LiveCaptureDevice now uses LiveUpdate to set the actor's properties.
  - Editor UI improvements 
  
### Removed
  -  LiveCaptureDevice IsLive/SetLive API.

### Fixed
  - Creating actors in Prefab mode doesn't work.  
  - Bug where the camera actor can be left in an indeterminate state if lens postprocessor is reset.

## [1.0.1] - 2021-08-26

### Changed
  - Minor update to the documentation 
  - Unity minimum version is now 2020.3.16f1
  
### Fixed
  - Shader related errors on Unity 2021.2

## [1.0.1-pre.585] - 2021-07-27

### Changed
  - Update to the documentation

### Fixed
  - Null reference exception when a user resizes a clip inside a TakeRecorderTrack

## [1.0.1-pre.570] - 2021-07-22

### Added
  - [Companion Apps] Privacy policy link
  - Button to open project settings from Take Recorder  
  - Help button in Video Server settings
  - "Create & Assign New Actor" button
  - New editor icons
  
### Changed
  - Show reticle in manual mode.
  - Virtual Camera Actor can be positioned manually.
  - "Align with Actor" button removed.
  - Damping related fields are grayed out when damping is disabled
  - Auto Horizon option is grayed out when roll is enabled
  - Improved network connectivity
  - Blend shape values have sliders to modify them 
  - Faster focus change when distance goes from infinity to something smaller and damping is enabled

### Fixed
  - AABB errors when using Virtual Camera with URP
  - Divide by zero errors when resizing Virtual Camera device inspector
  - [Virtual Camera] Reticle not showing on phone
  - Crash when connecting a virtual camera app on MacOS
  - [Virtual Camera] Reticle visible in playback mode on tablet
  - Video streaming when multiple network interfaces are present
  - Video streaming not working with the built-in render pipeline
  - [Companion apps] Multiple issues with the connection view
  - [Face Capture] Issue when using the "Flip Horizontally" option

## [1.0.1-pre.525] - 2021-07-06

### Added
  - [Face Capture] Audio and Video recording options.
  - Fill gate fit mode for frame lines.  
  
### Changed
  - [Companion apps] Minimum iOS version is now 14.5.
  - Rename "Review" to "Playback".
  - Custom Pass Manager is not editable anymore.
  - Networking memory allocations improvements. 

### Fixed
 - [Virtual Camera] It was easy to accidentally open the system menus or close the app.
 - Null references in virtual camera metadata inspector.
 - Warning thrown when attempting to track non-readable meshes.
 - [Virtual Camera] Non-responsive when scrolling settings.
 - Firewall configuration would sometimes freeze Unity.
 - Protocol warnings in 2021.2.
 - RendererList compilation error in 2021.2.0b2 and up.

## [1.0.1-pre.465] - 2021-06-02

### Added
- Component documentation links.
- [Face Capture] Default evaluator asset.
- [Face Capture] Global blend shape smoothing.
- [Virtual Camera] Damping for lens values.
- New lens presets and 1.78 aspect ratio

### Changed
- [Face Capture] Removed frame digit in time code
- [Virtual Camera] Inspector is enabled when actor is not live.
- Separate control for gate mask.
- URP and HDRP face samples have been replaced by a single face sample for the built-in pipeline.
- Improved documentation.   
- App icons are back to the default Unity icon

### Fixed
- TakeRecorder binding is not refreshed without changing take selection.
- [Face Capture] Screen diming interfers with UI.
- [Virtual Camera] App doesn't properly reconnect to server.
- Alignment for tracking faces was using gravity instead of camera world alignment.
- Recording closes dialogs.
- Reticle animation and click-through issue.
- Flickering in the game view, especially when focus reticle was displayed.
- Slider on mobile phone would sometimes open iOS notifications. 
- [Virtual Camera] Could not exit preview mode on the device when entered from the editor.
- [Virtual Camera] Lens and rig settings were hidden by default.
- Thumbnails were identical for different snapshots. 
- Changing lenses intrinsics did not refresh the client. 
- Some properties could not be excluded from presets.
- Error when adding a CinemachineCameraDriver.
- App and servers connectivity issues on 2021.12.0a18 and above.
- Warning in TakeBuilder.
- First lens added in a new lens kit was "unnamed".

## [1.0.0-pre.400] - 2021-05-13

### Added
- [Virtual Camera App] New settings.

### Fixed
- Warnings when importing the HDRP and URP face samples in the same project
- [Virtual Camera] Usage of the Resources folder.
- Documentation references to experimental package.
- [Virtual Camera] Focus Mode Display.
- [Face Capture App] Timecode layout.

## [1.0.0-pre.360] - 2021-05-12

### Added
- Mechanism to align virtual camera device with actor.
- [Face Capture] Send and apply head position.
- Frame lines.
- Vcam snapshots.
- Default Lens kits.
- Default aspect ratio and sensor presets.

### Changed
- Rename Joystick Speed to Joystick Sensitivity.
- Rename QuadroSync API to GPUFrameSync.
- [Virtual Camera] Focus distance dial goes up to infinity.
- A lens does not have a max focus distance.
- Depth of field is disabled in auto focus modes when nothing is tracked.
- [Virtual Camera] UI layout improvements.
- Allow to manually refine an automatically determined focus distance.
- [Virtual Camera] Move record button to the left.
- Package description.
- Non-linear scaling for focus distance slider.
- Improved face samples.
- Slate change logic.
 
### Fixed
- Crash when using NvEnc on some computers.
- [Companion App] Server Scan no longer overriden by manually entered server address.
- Support for focal length on vanilla built-in renderer.
- Exception when recording multiple devices.
- Video stream is no longer cropped on device.
- Flicker when changing slate in timeline while live.
- Fix error when removing a running server.
- Lens properties not being recorded. 
- Verbose video stream logs.
- Jump when starting to record with joysticks.
- Connection view error on phone.

## [1.0.0-pre.1] - 2021-05-03

### Added
- Legacy Render Pipeline support.
- Support for GPU Encoding via NvEnc.
- Lens Kit to group a collection of LensAssets.
- Support for Mac OS video streaming.
- SwapBarrier and GPUSync plug-in.
- Metadata on Take, Lens and VirtualCameraTrack.
- FrameRate field in TakeRecorder.

### Changed
- Face rotations recorded as Euler angles instead of quaternions.
- Internalized APIs that were previously public.
- CameraState renamed to Settings.
- Improvements to the TakeRecorder.
- Takes playback use the crop aspect used during recording.
- New names for some camera movements and focus modes.
- [Virtual Camera] iPhone UI.
 
### Fixed
- Bug where a take iteration longer than its parent base track would be shortened. 
- Memory leaks on HDRP 10.2

## [1.0.0-exp.235] - 2021-04-22

### Added
- [Face Capture] Toggle to mirror the rotation data of the head and eyes
- Play/Stop button in preview mode
- [Companion App] Scan button to discover running servers
- Assisted firewall configuration
- [Virtual Camera] Shows the list of available takes
- Recording API
- Timecode API

### Changed
- Record iterations as track overrides
- Improvements to the server's UX
- CustomPassManager gameObject is not hidden anymore
- In Add Component, Live Capture components now appear under their own section
- Add Device button now allows to add a path as well
- Downgraded Timeline dependency to 1.4.7
- A client can only be assigned to a single device at a time.

### Fixed
- [Face Capture] Fixed head and eyes rotation not being correctly applied in the DefaultMapper
- Unnessary animation output rebuild when a LiveLink was set active
- Issue where CompactList search loses focus
- Time jump on start recording/previewing
- Incorrect time offset in recordings

## [0.3.10-preview.166] - 2021-03-12

### Added
- Global Live/Preview button in TakeRecorder. Allows to preview the recorded take for all active devices.
- New format for the take names and asset names. Can be customized with ',' and wildcards.  
- 'Auto Start server on play' option.
- Face Capture App with samples.

### Changed
- New and improved companion app protocol.
- Use OS assigned ports for video server.
- Precise Ray-Mesh Intersection Tracking For Spatial Focus Mode
- Slate names have been renamed to Shot names

### Fixed
- A regression where recordings stop if the selected take is null.
- The TakeRecorder was not updated on creation until selection was changed.
- When device was removed, the TakeRecorder was throwing an exception and list was in inconsistent state.
- Package was not compiling if the optional inputsystem was not present.
- Virtual Camera record button would not appear after upgrade. 
- UX regressions around preview playback. 

## [0.3.9-preview.1] - 2021-02-12

### Added
- Video streaming support for URP.
- URP support for Film Format. 
- Add the ability to change damping from the client.
- Graphics Raycaster supports object picking.
- Spatial Focus Mode support.
- Focus Plane Visualization Feature.
- Support for custom camera data sources.
- Introduces the TakeRecorder component and slate track

### Changed
- Improved default face mapper inspector.
- Shots, takes and playback are now integrated with Timeline's tracks.
- Rename the package to Live-Capture.
- Reset versioning to a major in 0 because the package is still experimental.
- Ergonomic tilt is sent to server on client initialization.
- New UI to create a Companion App Server. 
- Updated package minimum version to Unity 2020.2 and compatible with URP/HDRP 10. 

### Fixed
- [Companion App] Issue where editing ergonomic tilt would set the pedestal space to global.  
- [Companion App] Damping issue with Cinemachine.
- [Companion App] Ergonomic tilt issue where the setting was not updated without a pose update.  
- VideoServer is disposed on device disable in order to avoid running out of ports
- [Companion App] Rebase rotation on Y axis lock to avoid rotation snapping back upon unlock.
- [Companion App] Fixed record button not being hidden by the setting to hide it.
- Issue where server could not be restarted for a while after being stopped
- A server discovery bug where the sending sockets would be recreated constantly when connected to a VPN or network that has broadcasting disabled, throwing many errors.
- A server discovery bug where it stopped polling the network interface updates. In some cases it was extremely slow and causing performance issues.

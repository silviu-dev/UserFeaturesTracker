# UserFeaturesTracker


## Description
This project is face main features tracker and gaze estimator

## Key Features
- Gaze estimation
- Blink detection
- Tracking the user’s face in real time
- Mouth opening detection
- Developed in C++ within OpenCV and DLib

## User Manual

### Installation
1. Clone this repository:
```shell
  git clone https://github.com/silviu-dev/UserFeaturesTracker.git
```
2. Navigate to the project directory: 
```shell
  cd UserFeaturesTracker/UserFeaturesTracker
```
3. Build and run the project using build.sh script:
```shell
  ./build.sh
```

### Gaze estimation optional calibration step

The plugin is build as an .dll which exports an simple interface:
```cpp
extern "C" __declspec(dllexport)
void getUserFeatures(UserPositionCallback, UserGazeCallback, UserMouthCallback, UserBlinkCallback, bool needCalibration);
```
There are for callbacks to handle the information from the .dll and one flag which sets the calibration step.
If the calibration is enabled then befor the first call to the .dll, the user is promted with an window and asked to look to a series of read dots in order to better calibrate the underleaing models.
  
   <img src="https://github.com/silviu-dev/UserFeaturesTracker/assets/56478018/98208d77-bacf-4040-be9a-3c9d5108cc37" alt="Human Imitator" width="300">

### Unity games example
This plugin demonstrates versatile applicability.

In the main folder ./UserFeaturesTracker/, there are two additional Unity-based applications that leverage this plugin:

1. The first application, named **Human Imitator**, features an avatar capable of accurately mimicking the user's movements and the camera angle is calculated based on user relative position to the screen.
  
   <img src="https://github.com/silviu-dev/UserFeaturesTracker/assets/56478018/810ac40e-fd31-4a16-b6b9-7d9494117154" alt="Human Imitator" width="300">

2. The second application, **CubeEyeMover**, introduces a unique interaction mechanic where users can manipulate a cube solely through gaze control.

   <img src="https://github.com/silviu-dev/UserFeaturesTracker/assets/56478018/3794eb39-ef70-4bab-920f-3e2154ff3f53" alt="CubeEyeMover" width="300">

Both examples highlight the diverse capabilities facilitated by the integration of this plugin with Unity (version 2020.3.22f1 was used).

## System Requirements
- OpenCV 4.8.0
- Cmake 3.14 or newer
- MSVC 19 compiler
- Dlib 19.24.0 or newer

## Contributing
Contributions are welcome! If you'd like to contribute to this project, you can fork it and then submit a pull request with your improvements.


## License
This project is licensed under the GNU Lesser General Public License v3.0. See the [LICENSE](LICENSE) file for details.

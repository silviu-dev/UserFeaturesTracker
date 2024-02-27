#pragma once
typedef void(__stdcall* UserPositionCallback)(int x, int y);
typedef void(__stdcall* UserGazeCallback)(int x, int y);
typedef void(__stdcall* UserMouthCallback)(double opening);
typedef void(__stdcall* UserBlinkCallback)(bool rightEyeBlinked, bool leftEyeBlinked);

extern "C" __declspec(dllexport)
void getUserFeatures(UserPositionCallback, UserGazeCallback, UserMouthCallback, UserBlinkCallback, bool);
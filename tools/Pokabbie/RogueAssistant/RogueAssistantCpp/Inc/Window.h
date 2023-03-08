#pragma once
#include <functional>

typedef std::function<bool()> WindowCallback;

int LaunchWindowLoop(WindowCallback callback);
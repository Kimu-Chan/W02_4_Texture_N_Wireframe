#pragma once

#define NOMINMAX

#define WIN32_LEAN_AND_MEAN

// STL include
#include <windows.h>
#include <tchar.h>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <set>
#include <cmath>
#include <cstring>
#include <cwchar>
#include <cctype>
#include <type_traits>
#include <utility>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <atomic>
#include <cstdint>

// D3D include
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;

template <typename T>
using TQueue = std::queue<T>;

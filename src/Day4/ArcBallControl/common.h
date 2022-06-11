#pragma once
#ifndef _COMMON_H_
#define _COMMON_H_

#include <filesystem>
#include <string>

static std::string        VERT_SHADER_FILE = std::filesystem::current_path().string() + "/render.vert";
static std::string        FRAG_SHADER_FILE = std::filesystem::current_path().string() + "/render.frag";
static const std::string DATA_DIRECTORY = std::filesystem::current_path().string() + "/../../../data/12_2/";

#endif  // _COMMON_H_
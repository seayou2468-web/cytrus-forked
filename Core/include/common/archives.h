// Copyright Citra Emulator Project / Azahar Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

// Serialization removed for libretro core
// Using dummy types to prevent compilation errors
struct DummyArchive {};
using iarchive = DummyArchive;
using oarchive = DummyArchive;

#define SERIALIZE_IMPL(A) // Serialization removed

#define SERIALIZE_EXPORT_IMPL(A) // Serialization removed

#define DEBUG_SERIALIZATION_POINT // Debug serialization removed

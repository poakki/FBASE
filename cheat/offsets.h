#pragma once

// --- OFFSETS UWORLD & ENGINE ---
const auto offset_persistent_level = 0x30;
const auto offset_game_instance = 0x1F8; // Confirmado no dump
const auto offset_gamestate = 0x1A0;
const auto offset_local_players_array = 0x38;

// PlayerController (Geralmente 0x30 dentro da classe Player)
const auto offset_player_controller = 0x30;

const auto offset_camera_manager = 0x390; // Confirmado no dump
const auto offset_apawn = 0x378;          // AcknowledgedPawn

const auto offset_camera_cache = 0x2E60;  // CameraCachePrivate

// Componentes do Ator
const auto offset_root_component = 0x1B0;
const auto offsets_instigator_actor = 0x198;
const auto offset_actor_mesh = 0x340;
const auto offset_player_state = 0x300;

const auto offset_relative_location = 0x14C;

// Array de Atores
const auto offset_actor_array = 0x98;
const auto offset_actor_count = offset_actor_array + 0x8;

// --- DADOS ESPECÍFICOS ---
// Ajusta a vida se souberes o offset correto, caso contrário deixa a 0
const auto offset_currenthealth = 0x0;
const auto offset_maxhealth = offset_currenthealth + 0x4;

const auto offset_bonearray = 0x598;
const auto offset_bonecount = 0x10;
const auto offset_compent = 0x250;
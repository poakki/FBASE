#pragma once
#include <Windows.h>
#include <iostream>
#include "offsets.h"
#include "func.h" 
#include "draw.h"

// [CORREÇÃO] 'extern' evita o erro 'redefinição; várias inicializações'
extern uintptr_t uworld_sig;
extern int width;
extern int height;
extern bool cornerbox;
extern bool box;
extern bool draw_line;
extern bool aimbot;
extern int boneidselect;
extern float aimfov;
extern float smooth_;

// [IMPORTANTE] 'inline' permite que a função esteja no .h sem erro de duplicação
inline void initcheat()
{
	if (!uworld_sig) return;

	auto u_world = read<uintptr_t>(uworld_sig);
	if (!u_world) return;

	auto game_instance = read<uintptr_t>(u_world + offset_game_instance);
	if (!game_instance) return;

	auto local_player_array = read<uintptr_t>(game_instance + offset_local_players_array);
	if (!local_player_array) return;

	auto local_player = read<uintptr_t>(local_player_array);
	if (!local_player) return;

	auto local_player_controller = read<uintptr_t>(local_player + offset_player_controller);
	if (!local_player_controller) return;

	screensize(local_player_controller, width, height);
	if (width <= 0 || height <= 0) return;

	auto local_player_pawn = read<uintptr_t>(local_player_controller + offset_apawn);

	auto persistent_level = read<uintptr_t>(u_world + offset_persistent_level);
	if (!persistent_level) return;

	auto actors = read<uintptr_t>(persistent_level + offset_actor_array);
	if (!actors) return;

	auto actors_count = read<int>(persistent_level + offset_actor_count);

	if (actors_count <= 0 || actors_count > 10000) return;

	for (int i = 0; i < actors_count; i++)
	{
		auto actor_pawn = read<uintptr_t>(actors + i * 0x8);
		if (!actor_pawn) continue;

		if (local_player_pawn && actor_pawn == local_player_pawn) continue;

		auto instigator = read<uintptr_t>(actor_pawn + offsets_instigator_actor);
		if (!instigator) continue;

		auto root = read<uintptr_t>(instigator + offset_root_component);
		if (!root) continue;

		auto mesh = read<uintptr_t>(instigator + offset_actor_mesh);
		if (!mesh) continue;

		if (offset_currenthealth != 0) {
			auto _health = read<float>(actor_pawn + offset_currenthealth);
			if (_health <= 0) continue;
		}

		if (draw_line)
		{
			vec2 screen;
			auto root_loc = read<vec3>(root + offset_relative_location);
			if (worldtoscreen(local_player_controller, root_loc, &screen, false))
			{
				DrawLine(ImVec2(width / 2, height), ImVec2(screen.x, screen.y), ImColor(255, 255, 255));
			}
		}

		if (box || cornerbox) {
			ImVec4 boxcolor = ImColor(255, 255, 255);
			vec2 basew2s, headw2s;

			auto vBaseBone = getbone(mesh, 0);

			if (worldtoscreen(local_player_controller, vBaseBone, &basew2s, false)) {
				vec3 headPos = vBaseBone;
				headPos.z += 160.f;

				if (worldtoscreen(local_player_controller, headPos, &headw2s, false)) {
					float BoxHeight = abs(headw2s.y - basew2s.y);
					float BoxWidth = BoxHeight * 0.65f;

					if (cornerbox)
						DrawCornerBox(headw2s.x - (BoxWidth / 2), headw2s.y, BoxWidth, BoxHeight, ImGui::ColorConvertFloat4ToU32(boxcolor), 1.f);
					else if (box)
						DrawBox(headw2s.x - (BoxWidth / 2), headw2s.y, BoxWidth, BoxHeight, ImGui::ColorConvertFloat4ToU32(boxcolor), 1.f);
				}
			}
		}

		if (aimbot)
		{
			if (!steam_keybind(VK_RBUTTON)) continue;
			if (!visible_check(local_player_controller, instigator)) continue;

			int target_bone_idx = 0;
			if (boneidselect == 1) target_bone_idx = 66;
			else if (boneidselect == 2) target_bone_idx = 65;
			else target_bone_idx = 4;

			vec3 target_pos = getbone(mesh, target_bone_idx);
			aimbot_function(local_player_controller, width, height, target_pos, mesh);
		}
	}
}
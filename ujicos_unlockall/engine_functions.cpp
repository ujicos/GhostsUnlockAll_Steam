#include "pch.h"

StructuredDataDef_GetAsset_t StructuredDataDef_GetAsset = StructuredDataDef_GetAsset_t(0x1404E6560);
SL_ConvertToString_t SL_ConvertToString = SL_ConvertToString_t(0x1404317F0);
SL_GetStringForInt_t SL_GetStringForInt = SL_GetStringForInt_t(0x140431CF0);
SL_FindString_t SL_FindString = SL_FindString_t(0x140431A90);
LiveStorage_PlayerDataSetIntByNameArray_t LiveStorage_PlayerDataSetIntByNameArray = LiveStorage_PlayerDataSetIntByNameArray_t(0x140404640);
StringTable_GetAsset_t StringTable_GetAsset = StringTable_GetAsset_t(0x1404E6170);
StringTable_GetColumnValueForRow_t StringTable_GetColumnValueForRow = StringTable_GetColumnValueForRow_t(0x1404E61A0);
LiveStorage_GetPersistentDataBuffer_t LiveStorage_GetPersistentDataBuffer = LiveStorage_GetPersistentDataBuffer_t(0x140400170);
LiveStorage_PlayerDataSetReservedInt_t LiveStorage_PlayerDataSetReservedInt = LiveStorage_PlayerDataSetReservedInt_t(0x140404820);
LiveStorage_PlayerDataSetIntByName_t LiveStorage_PlayerDataSetIntByName = LiveStorage_PlayerDataSetIntByName_t(0x140404730);
Cbuf_AddText_t Cbuf_AddText = Cbuf_AddText_t(0x1403F6B50);


const int controller_index = 0;

//Checks if it's a valid reserved lookup string otherwise it crashes the client.
bool is_reserved_lookup_string(const char* lookup_string)
{
	const std::vector<std::string> reserved_lookup_strings
			{
				//EXTINCTION MODE RESERVED LOOKUP STRINGS
				"extinction_purchase_flags",
				"extinction_tokens",
				"extinction_crafted_flags",
				"upgrades_enabled_flags",
				"relics_enabled_flags",
				"bonus_pool_size",
				"bonus_pool_deadline",
				//MULTIPLAYER
				"prestigeLevel",
				//COMMON
				"mp_announcer_type"
			};

			return std::find(reserved_lookup_strings.begin(), reserved_lookup_strings.end(), lookup_string)
				!= reserved_lookup_strings.end();
}

		StatsGroup get_statsgroup_for_reserved_lookup_string(const char* lookup_string)
		{
			enum StatsGroup statsgroup { STATSGROUP_COOP };

			if (!strcmp(lookup_string, "prestigeLevel"))
			{
				statsgroup = STATSGROUP_RANKED;
			}
			else if (!strcmp(lookup_string, "mp_announcer_type"))
			{
				statsgroup = STATSGROUP_COMMON;
			}

			return statsgroup;
		}

		StatsGroup get_statsgroup_for_lookup_string(const char* lookup_string)
		{
			const auto asset{ StructuredDataDef_GetAsset("mp/playerdata.def", 0x93FCu) };

			bool found_param{ false };
			enum StatsGroup statsgroup { STATSGROUP_RANKED };

			for (int i = 0; i < asset->structs[23].propertyCount; ++i)
			{
				if (!strcmp(SL_ConvertToString(asset->structs[23].properties[i].name), lookup_string))
				{
					statsgroup = STATSGROUP_COMMON;
					found_param = true;
					break;
				}
			}

			//no need to search it again if we already found it at the common strings
			if (!found_param)
			{
				for (int i = 0; i < asset->structs[22].propertyCount; ++i)
				{
					if (!strcmp(SL_ConvertToString(asset->structs[22].properties[i].name), lookup_string))
					{
						statsgroup = STATSGROUP_COOP;
						break;
					}
				}
			}

			return statsgroup;
		}

		bool is_numeric(const std::string& text)
		{
			return std::to_string(atoi(text.data())) == text;
		}

		/*std::vector<unsigned int> parse_params_to_lookup_strings(const command::params& params, bool setData)
		{
			std::vector<unsigned int> lookup_strings_vector{};
			int amount_lookup_strings{ params.size() };

			//last param is a value if we're setting data.
			if (setData)
			{
				--amount_lookup_strings;
			}

			for (int i = 1; i < amount_lookup_strings; ++i)
			{
				if (is_numeric(params.get(i)))
				{
					lookup_strings_vector.push_back(SL_GetStringForInt(atoi(params.get(i))));
				}
				else
				{
					lookup_strings_vector.push_back(SL_FindString(params.get(i)));
				}
			}

			return lookup_strings_vector;
		}*/

		void set_player_data_for_lookup_string(const char* lookup_string,
			const std::vector<std::tuple<const char*, const int>>& data, StatsGroup statsgroup)
		{
			unsigned int nav_strings[2]{};

			nav_strings[0] = SL_FindString(lookup_string);
			for (int i = 0; i < data.size(); ++i)
			{
				nav_strings[1] = SL_FindString(std::get<0>(data[i]));
				LiveStorage_PlayerDataSetIntByNameArray(
					controller_index, nav_strings, 2, std::get<1>(data[i]), statsgroup);
			}
		}

		void register_squadmembers_purchase()
		{
			unsigned int nav_strings[2]{};
			const StringTable* unlock_table{ nullptr };
			StringTable_GetAsset("mp/unlocktable.csv", &unlock_table);

			if (unlock_table)
			{
				//squad purchases challenge is defined at line 112 and ends at 152.
				//it's defined 4 times each hence the jumps of 4
				for (int i = 112; i < 152; i += 4)
				{
					// Register squad purchases for calling cards
					auto squadmember_purchase = StringTable_GetColumnValueForRow(unlock_table, i, 3);

					nav_strings[0] = SL_FindString("challengeState");
					nav_strings[1] = SL_FindString(squadmember_purchase);
					LiveStorage_PlayerDataSetIntByNameArray(
						controller_index, nav_strings, 2, 2, STATSGROUP_RANKED);

					nav_strings[0] = SL_FindString("challengeProgress");
					LiveStorage_PlayerDataSetIntByNameArray(
						controller_index, nav_strings, 2, 1, STATSGROUP_RANKED);
				}
			}
		}

		void unlock_all_squadmember(const int squadmember_index)
		{
			unsigned int nav_strings[5]{};
			const int amount_regular_loadouts{ 6 };
			const StringTable* squad_unlocktable{ nullptr };
			StringTable_GetAsset("mp/squadunlocktable.csv", &squad_unlocktable);

			//select specific squadmember
			nav_strings[0] = SL_FindString("squadMembers");
			nav_strings[1] = SL_GetStringForInt(squadmember_index);

			//unlocks squadmember
			nav_strings[2] = SL_FindString("inUse");
			LiveStorage_PlayerDataSetIntByNameArray(
				controller_index, nav_strings, 3, 1, STATSGROUP_RANKED);

			//choose a default loadout otherwise unlocking loadouts won't work
			nav_strings[2] = SL_FindString("defaultSet");
			LiveStorage_PlayerDataSetIntByNameArray(
				controller_index, nav_strings, 3, 1, STATSGROUP_RANKED);

			//set max xp squad member
			nav_strings[2] = SL_FindString("squadMemXP");
			LiveStorage_PlayerDataSetIntByNameArray(
				controller_index, nav_strings, 3, 1230080, STATSGROUP_RANKED);

			//unlocks loadouts
			nav_strings[2] = SL_FindString("loadouts");
			nav_strings[4] = SL_FindString("inUse");

			for (int j = 0; j < amount_regular_loadouts; ++j)
			{
				nav_strings[3] = SL_GetStringForInt(j);
				LiveStorage_PlayerDataSetIntByNameArray(
					controller_index, nav_strings, 5, 2, STATSGROUP_RANKED);
			}

			//unlocks all weapons, reticles and killstreaks 
			nav_strings[2] = SL_FindString("challengeState");
			if (squad_unlocktable)
			{
				for (int j = 0; j < squad_unlocktable->rowCount; ++j)
				{
					auto unlock_item = StringTable_GetColumnValueForRow(squad_unlocktable, j, 3);

					if (unlock_item[0] == '\0')
						continue;

					nav_strings[3] = SL_FindString(unlock_item);
					LiveStorage_PlayerDataSetIntByNameArray(
						controller_index, nav_strings, 4, 2, STATSGROUP_RANKED);
				}
			}
		}

		void unlock_all_extinction()
		{
			printf("[Extinction] Setting Extinction stats...\n");
			const auto persistent_data_buffer{ LiveStorage_GetPersistentDataBuffer(controller_index) };
			const std::vector<std::tuple<const char*, const int>> data{
				{"prestige", 25},
				{"experience", 1845000},
				//not sure if necessary 
				{"rank", 31},
				{"revives", 100},
				{"kills", 10000},
				{"escaped", 10},
				//unlocks background with the challenges "Beat any Extinction map with x Relic(s) Active"
				{"headShots", 5}
			};

			set_player_data_for_lookup_string("alienPlayerStats", data, STATSGROUP_COOP);

			LiveStorage_PlayerDataSetReservedInt(
				persistent_data_buffer, "extinction_purchase_flags", -1, 0, STATSGROUP_COOP);
			LiveStorage_PlayerDataSetReservedInt(
				persistent_data_buffer, "extinction_tokens", 5000, 0, STATSGROUP_COOP);
			printf("[Extinction] Done setting Extinction stats!\n");
		}

		void unlock_all_multiplayer()
		{
			printf("[Multiplayer] Setting Multiplayer stats...\n");
			const int amount_squad_members{ 10 };
			const auto persistent_data_buffer{ LiveStorage_GetPersistentDataBuffer(controller_index) };

			for (int i = 0; i < amount_squad_members; ++i)
			{
				unlock_all_squadmember(i);
			}

			register_squadmembers_purchase();

			//not the correct way to set each squadmembers prestige
			//but i couldn't figure it out how it's done.
			for (int i = 0; i < amount_squad_members; ++i)
			{
				hook::set<int>(0x1445A2B20 + (i * 4), i);

			}

			//set the players prestige to 10th prestige
			LiveStorage_PlayerDataSetReservedInt(
				persistent_data_buffer, "prestigeLevel", 10, 0, STATSGROUP_RANKED);

			//gives unlock points but not really necassary since everything is already unlocked
			LiveStorage_PlayerDataSetIntByName(controller_index, SL_FindString("unlockPoints"), 5000, StatsGroup::STATSGROUP_RANKED);
			printf("[Multiplayer] Done setting Multiplayer stats!\n");
		}

		void unlock_all_challenges()
		{
			printf("[Challenges] Unlocking all challenges...\n");
			unsigned int nav_strings[2]{};
			const StringTable* challenges_table{ nullptr };
			StringTable_GetAsset("mp/allchallengestable.csv", &challenges_table);

			if (challenges_table)
			{
				for (int i = 0; i < challenges_table->rowCount; ++i)
				{
					// Find challenge
					auto challenge{ StringTable_GetColumnValueForRow(challenges_table, i, 0) };

					int max_state{ 0 };
					int max_progress{ 0 };

					// Find correct tier and progress
					for (int j = 0; j < 8; ++j)
					{
						int progress{ atoi(StringTable_GetColumnValueForRow(challenges_table, i, 9 + j * 2)) };
						if (!progress) break;

						max_state = j + 2;
						max_progress = progress;
					}

					nav_strings[0] = SL_FindString("challengeState");
					nav_strings[1] = SL_FindString(challenge);
					LiveStorage_PlayerDataSetIntByNameArray(
						controller_index, nav_strings, 2, max_state, STATSGROUP_RANKED);

					nav_strings[0] = SL_FindString("challengeProgress");
					LiveStorage_PlayerDataSetIntByNameArray(
						controller_index, nav_strings, 2, max_progress, STATSGROUP_RANKED);
				}
			}
			printf("[Challenges] Done unlocking all challenges!\n");
		}

		void unlock_past_title_backgrounds()
		{
			printf("[Additional] Unlocking Black Ops / Modern Warfare exclusive items...\n");
			const std::vector<std::tuple<const char*, const int>> data{
				{"playedmw3", 1},
				{"playedblackops2", 1},
				{"mw3prestige", 20},
				{"blackops2prestige", 11}
			};

			set_player_data_for_lookup_string("pastTitleData", data, STATSGROUP_COMMON);
			printf("[Additional] Done unlocking exclusive items!\n");
		}

		int wait;

		void keyBinds()
		{
			wait++;
			if (wait > 20);
			{
				if (GetAsyncKeyState(VK_DELETE))
				{
					printf("test\n");
					ShowWindow(GetConsoleWindow(), SW_HIDE);
					//FreeConsole();
					//PostMessage(GetConsoleWindow(), WM_QUIT, 0, 0);
				}
			}
			wait = 0;
		}
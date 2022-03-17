#include "components.hpp"
#include <reload_manager.hpp>
#include <common.hpp>
#include <tiny_ecs_registry.hpp>

ReloadManager::ReloadManager() {

}

ReloadManager::~ReloadManager() {
    clear();
}

std::string get_init_level_data_file_path(int level) {
    return data_path() + "/levels" + 
        "/level" + std::to_string(level) + 
        "/init.json";
}

std::string get_saved_level_data_file_path(int level) {
    return data_path() + "/levels" + 
        "/level" + std::to_string(level) + 
        "/saved.json";
}

#include <iostream>
void ReloadManager::load(int level) {
    std::string init_datafile_path = get_init_level_data_file_path(level);
    std::string saved_datafile_path = get_saved_level_data_file_path(level);    

    std::ifstream iifs(init_datafile_path);
    std::ifstream sifs(saved_datafile_path);

    // if saved.json exists under curr level, it means the game was paused
    // otherwise we can start a level from scratch
    nlohmann::json js;
    if (sifs.good()) {
        js = nlohmann::json::parse(sifs);
    } else {
        js = nlohmann::json::parse(iifs);
    }


    vec2 camera_lower_limit_delta = vec2(js["camera"]["lower_limit"]["x"],
            js["camera"]["lower_limit"]["y"]);
    vec2 camera_upper_limit_delta = vec2(js["camera"]["upper_limit"]["x"],
            js["camera"]["upper_limit"]["y"]);
    vec2 camera_pos = vec2(js["camera"]["pos"]["x"], js["camera"]["pos"]["y"]);

    camera_data.pos = camera_pos;
    camera_data.lower_limit_delta = camera_lower_limit_delta;
    camera_data.upper_limit_delta = camera_upper_limit_delta;

    vec2 background_size = vec2(js["background"]["size"]["w"], 
            js["background"]["size"]["h"]);
    background_data.size = background_size;

    auto players_data = js["players"];
    for (auto& player_data: players_data) {
        vec2 player_pos = vec2(player_data["pos"]["x"], player_data["pos"]["y"]);
        vec2 player_size = vec2(player_data["size"]["w"], player_data["size"]["h"]);
        float player_health = player_data["health"];
        float player_energy = player_data["energy"];
        int advanced_attack_cooldown = player_data["advanced_attack_cooldown"];
        PlayerData pd {
            player_pos,
            player_size,
            player_health,
            player_energy,
            advanced_attack_cooldown
        };
        player_data_vector.push_back(pd);
    }

    auto enemies_data = js["enemies"];
    for (auto& enemy_data: enemies_data) {
        vec2 enemy_pos = vec2(enemy_data["pos"]["x"], enemy_data["pos"]["y"]);
        vec2 enemy_size = vec2(enemy_data["size"]["w"], enemy_data["size"]["h"]);
        float enemy_health = enemy_data["health"];
        float enemy_energy = enemy_data["energy"];
        int advanced_attack_cooldown = enemy_data["advanced_attack_cooldown"];
        EnemyData ed {
            enemy_pos,
            enemy_size,
            enemy_health,
            enemy_energy,
            advanced_attack_cooldown
        };
        enemy_data_vector.push_back(ed);
    }

    auto terrains_data = js["terrains"];
    for (auto& terrain_data: terrains_data) {
        vec2 terrain_pos = vec2(terrain_data["pos"]["x"], terrain_data["pos"]["y"]);
        vec2 terrain_size = vec2(terrain_data["size"]["w"], terrain_data["size"]["h"]);
        TerrainData td {
            terrain_pos,
            terrain_size
        };
        terrain_data_vector.push_back(td);
    }


    auto ladders_data = js["ladders"];
    for (auto& ladder_data : ladders_data) {
        vec2 ladder_pos = vec2(ladder_data["pos"]["x"], ladder_data["pos"]["y"]);
        vec2 ladder_size = vec2(ladder_data["size"]["w"], ladder_data["size"]["h"]);
        LadderData ld {
            ladder_pos,
            ladder_size
        };
        ladder_data_vector.push_back(ld);
    }

    curr_order_ind = js["curr_order_ind"];
}

CameraData ReloadManager::get_camera_data() {
    return camera_data;
}

BackgroundData ReloadManager::get_background_data() {
    return background_data;
}

std::vector<PlayerData> ReloadManager::get_player_data() {
    return player_data_vector;
}

std::vector<EnemyData> ReloadManager::get_enemy_data() {
    return enemy_data_vector;
}

std::vector<TerrainData> ReloadManager::get_terrain_data() {
    return terrain_data_vector;
}

std::vector<LadderData> ReloadManager::get_ladder_data() {
    return ladder_data_vector;
}

int ReloadManager::get_curr_order_ind() {
    return curr_order_ind;
}

void ReloadManager::update_camera_data(CameraData camera_data) {
    this->camera_data = camera_data;
}

void ReloadManager::update_background_data(BackgroundData background_data) {
    this->background_data = background_data;
}

void ReloadManager::update_player_data(std::vector<PlayerData> player_data_vector) {
    this->player_data_vector = player_data_vector;
}

void ReloadManager::update_enemy_data(std::vector<EnemyData> enemy_data_vector) {
    this->enemy_data_vector = enemy_data_vector;
}

void ReloadManager::update_terrain_data(std::vector<TerrainData> terrain_data_vector) {
    this->terrain_data_vector = terrain_data_vector;
}

void ReloadManager::update_ladder_data(std::vector<LadderData> ladder_data_vector) {
    this->ladder_data_vector = ladder_data_vector;
}

void ReloadManager::update_curr_order_ind(int curr_order_ind) {
    this->curr_order_ind = curr_order_ind;
}

void ReloadManager::save(int level) {
    nlohmann::json curr_level_data_json;

    curr_level_data_json["camera"]["lower_limit"]["x"] = camera_data.lower_limit_delta.x;
    curr_level_data_json["camera"]["lower_limit"]["y"] = camera_data.lower_limit_delta.y;
    curr_level_data_json["camera"]["upper_limit"]["x"] = camera_data.upper_limit_delta.x;
    curr_level_data_json["camera"]["upper_limit"]["y"] = camera_data.upper_limit_delta.y;
    curr_level_data_json["camera"]["pos"]["x"] = camera_data.pos.x;
    curr_level_data_json["camera"]["pos"]["y"] = camera_data.pos.y;

    curr_level_data_json["background"]["size"]["w"] = background_data.size.x;
    curr_level_data_json["background"]["size"]["h"] = background_data.size.y;

    std::vector<nlohmann::json> player_json;
    for (auto& player_data: player_data_vector) {
        nlohmann::json temp_json; 
        temp_json["pos"]["x"] = player_data.pos.x;
        temp_json["pos"]["y"] = player_data.pos.y;
        temp_json["size"]["w"] = player_data.size.x;
        temp_json["size"]["h"] = player_data.size.y;
        temp_json["health"] = player_data.health;
        temp_json["energy"] = player_data.energy;
        temp_json["advanced_attack_cooldown"] = player_data.advanced_attack_cooldown;
        player_json.push_back(temp_json);
    }
    curr_level_data_json["players"] = player_json;

    std::vector<nlohmann::json> enemy_json;
    for (auto& enemy_data: enemy_data_vector) {
        nlohmann::json temp_json; 
        temp_json["pos"]["x"] = enemy_data.pos.x;
        temp_json["pos"]["y"] = enemy_data.pos.y;
        temp_json["size"]["w"] = enemy_data.size.x;
        temp_json["size"]["h"] = enemy_data.size.y;
        temp_json["health"] = enemy_data.health;
        temp_json["energy"] = enemy_data.energy;
        temp_json["advanced_attack_cooldown"] = enemy_data.advanced_attack_cooldown;
        enemy_json.push_back(temp_json);
    }
    curr_level_data_json["enemies"] = enemy_json;

    std::vector<nlohmann::json> terrain_json;
    for (auto& terrain_data: terrain_data_vector) {
        nlohmann::json temp_json; 
        temp_json["pos"]["x"] = terrain_data.pos.x;
        temp_json["pos"]["y"] = terrain_data.pos.y;
        temp_json["size"]["w"] = terrain_data.size.x;
        temp_json["size"]["h"] = terrain_data.size.y;
        terrain_json.push_back(temp_json);
    }
    curr_level_data_json["terrains"] = terrain_json;

    std::vector<nlohmann::json> ladder_json;
    for (auto& ladder_data: ladder_data_vector) {
        nlohmann::json temp_json; 
        temp_json["pos"]["x"] = ladder_data.pos.x;
        temp_json["pos"]["y"] = ladder_data.pos.y;
        temp_json["size"]["w"] = ladder_data.size.x;
        temp_json["size"]["h"] = ladder_data.size.y;
        ladder_json.push_back(temp_json);
    }
    curr_level_data_json["ladders"] = ladder_json;

    // -1 because we advance order at the start of the game
    curr_level_data_json["curr_order_ind"] = curr_order_ind - 1;

    std::string datafile_path = get_saved_level_data_file_path(level);
    std::ofstream ofs(datafile_path);
    ofs << curr_level_data_json;
}

void ReloadManager::clear() {
    player_data_vector.clear();
    enemy_data_vector.clear();
    terrain_data_vector.clear();
    ladder_data_vector.clear();
}

void ReloadManager::destroy_saved_level_data_file(int level) {
    std::string saved_datafile_path = get_saved_level_data_file_path(level);    

    // remove only takes c strings
    // maybe we should upgrade to C++17
    const char *cstring = saved_datafile_path.c_str();
    remove(cstring);
}

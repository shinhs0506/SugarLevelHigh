#include <nlohmann/json.hpp>

#include <common.hpp>

struct CameraData {
    vec2 pos;
    vec2 lower_limit_delta;
    vec2 upper_limit_delta;
};

struct BackgroundData {
    vec2 size;
};

struct PlayerData {
    vec2 pos;
    vec2 size;
    float health;
    float energy;
    int advanced_attack_cooldown;
};

struct EnemyData {
    vec2 pos;
    vec2 size;
    float health;
    float energy;
    int cooldown;
    int advanced_attack_cooldown;
};

struct TerrainData {
    vec2 pos;
    vec2 size;
};

struct LadderData {
    vec2 pos;
    vec2 size;
};

class DataManager {
    
    private:
        // loaded data
        CameraData camera_data;
        BackgroundData background_data;
        std::vector<PlayerData> player_data_vector;
        std::vector<EnemyData> enemy_data_vector;
        std::vector<TerrainData> terrain_data_vector;
        std::vector<LadderData> ladder_data_vector;
        int curr_order_ind;

    public:
        DataManager();

        ~DataManager();

        void load(int level);

        CameraData get_camera_data();
        BackgroundData get_background_data();
        std::vector<PlayerData> get_player_data();
        std::vector<EnemyData> get_enemy_data();
        std::vector<TerrainData> get_terrain_data();
        std::vector<LadderData> get_ladder_data();
        int get_curr_order_ind();

        void update_camera_data(CameraData camera_data);
        void update_background_data(BackgroundData background_data);
        void update_player_data(std::vector<PlayerData> player_data);
        void update_enemy_data(std::vector<EnemyData> enemy_data);
        void update_terrain_data(std::vector<TerrainData> terrain_data);
        void update_ladder_data(std::vector<LadderData> ladder_data);
        void update_curr_order_ind(int curr_order_ind);

        void save(int level);

        void clear();

        void destroy_saved_level_data_file(int level);
};

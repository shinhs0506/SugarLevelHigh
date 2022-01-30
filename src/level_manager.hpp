#include <tiny_ecs.hpp>
#include <tiny_ecs_registry.hpp>

// Wraps all level logis and entities
class LevelManager
{
public:
	LevelManager();

	~LevelManager();

	void init();

	// Load all needed entities for the level
	void load_level(int level);

	// Restart the level
	void restart_level();

	// Abandon the level and directly go to results screen
	void abandon_level();

	// Should be called from GameSystem to step level content
	bool step(float elapsed_ms);
	void handle_collisions();
	
	// Whether this level ended
	bool level_ended();

	// Input callback functions, should be called within GameSystem input callbacks
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);

private:
	bool ended;
	int curr_level;
    
    // for turn order logic
    bool should_advance_turn_order;
    int curr_order_index;
    int num_playables;
};

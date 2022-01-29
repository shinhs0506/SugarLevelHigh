// Wraps all level logis and entities
class Level
{
public:
	Level();

	~Level();

	// Initialize a level with the level number
	void init(int level_num);

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
	int level_num;
};
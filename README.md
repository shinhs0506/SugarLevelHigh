# Sugar Level: High

Created by Team 3
- Karen Lau 
- Lemuel Pranoto 
- John Shin 
- Marcus Thian
- Galila Zhang 
- Henry Zeng 

## Mandatory Requirements

### Playability

- There are now three full levels and a tutorial for the player to play. The tutorial features on-screen prompts and removes the need to provide players with oral instruction. Apart from the tutorial, each level features different unique mechanics and takes at least several minutes to play through. These are all initialized in the respective folders inside of `data/levels`, with unique level features implemented throughout `level_manager.cpp`. 


### Robustness 

** Memory management**
- Game does not hog memory. 

**User input**
- Unexpected user inputs are ignored and do not crash the game.

**Runtime**
- There are no noticeable runtime bottlenecks. All areas of the game run without lag. 

### Stability

** Fixed bugs**
- We have fixed a bug related to our physics systems, which was discovered during the last crossplay session. Characters and their respective health bars are now in sync and react the same to gravity, rather than having slightly different positions due to be affected by gravity in different ways as before. This can be found in `PhysicsSystem::step`.
- Interpolation is now used to determine character movement in level 2. This can be found in `PhysicsSystem::step`.
- We have fixed a bug which led to unpredictable behaviour when the camera was moved in-level and the player returned to the menu screen. This can be found in `CameraManager::reset_camera_pos`.
- Attack previews now show the correct projectile direction after the camera has been moved

## Creative Components

**Parallax scrolling**
- Parallax scrolling effects have been added to the levels. The three background layers are initialized by calls to `createBackground` in `level_manager.cpp`. The movement of these backgrounds is controlled by `camera_manager.cpp` and `physics_system.cpp`, corresponding to the movement of the camera. They have different `proportion_velocity` values which allow them to move at different speeds, consistently proportional to the camera speed. 

**Audio feedback**
- Background music has been added in `game_system::create_window` function (background music code was already there so basically the .wav file was just changed)
- 4 Audio feedbacks were added. 
- "melee_attack.wav" and "advanced_attack.wav" sound effects were added to both the player and the enemy and plays (using the Mix_PlayChannel function) whenever they make the respective attacks. For the player, it can be found in `PlayerController::on_mouse_button` inside the `PERFORM_ABILITY_MANUAL` case and for the enemy it can be found in EnemyController::make_decision function. 
- "healing_ability.wav" sound effects were added to the healing ability that is exclusive to the player and plays when the player uses the healing ability. This sound effect can be found in `PlayerController::step` inside the `if(current_state == CharacterState::PERFORM_ABILITY_AUTO)` statement.
- "hurt.wav" sound effects were added and plays whenever any character (player or enemy) gets hurt. This can be found in `LevelManager::handle_collisions`.

**Basic integrated assets**
- More assets have been added to `data/textures`, including new textures for all buttons in the game, textures for ladders which tile according to the height of the ladder, visuals for character attacks, on-screen prompts for tutorials and level-endings, new backgrounds, storytelling elements at the start of the game, new spritesheet animations for the enemy characters, and an improved UI for a character's energy bar. 

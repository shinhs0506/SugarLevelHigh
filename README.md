# Sugar Level: High

Created by Team 3
- Karen Lau 
- Lemuel Pranoto 
- John Shin 
- Marcus Thian
- Galila Zhang 
- Henry Zeng 

## Mandatory Requirements

### Improved Gameplay

**Game Logic**
- The enemy has a simple AI which allows the enemy sprite to move to the attack range and attack the closest playable character, to use advanced attack ability when it is not in cd, and to keep distance from playable characters when it is low in health. The decision making logic is implemented in `EnemyController::make_decision`.

**Animation**
- The left and right movements of the playable character are animated using spritesheet animation. The implementation of the spritesheet animation is inside `textured.fs.glsl`.

**Assets**
- New assets such as the energy bar for movement (`level_init::createEnergyBar`), health bars for all characters (`level_init::createHealthBar`), and ladders for character to climb (`level_init::createLadder`) are created. There is also a new main menu with clickable buttons implemented in `MenuManager::init::19`.

**Help**
- A help page is implemented with an image (`menu_init::createHelpImage`).

### Playability

- There is a full level for players to play. The level design is initialized using `json` file: `levels/level0/init.json`.  

### Stability 

- [missing]

## Creative Components

### [missing]
- [missing]

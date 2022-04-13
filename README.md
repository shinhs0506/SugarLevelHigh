# Sugar Level: High

Created by Team 3
- Karen Lau 
- Lemuel Pranoto 
- John Shin 
- Marcus Thian
- Galila Zhang 
- Henry Zeng 

## Mandatory Requirements

### Stability

- In milestone 3, it was identified that our game would not scale correctly on displays with different screen resolutions. We have resolved this by adding a configuration screen to our game, which allows the user to adjust the scale based on their screen resolution.The entry point is `config_manager.hpp`.
- We identified and fixed a bug where the camera would not move properly after reloading the game from a save file.

### Playability 

- The game contains a tutorial level and four game levels. On average, each level takes 2 to 5 minutes to beat depending on the player's familiarity with the game strategy. On many playthroughs, it took us more than 10 minutes to complete the game. 

### User Experience

**Tutorial**
- Our tutorial level explains character movement, all the character abilities and walks the player through how to kill an enemy. In addition to this tutorial level, we also have title screens before each level which hint at the unique mechanics introduced in that level. For example, the ability to move the camera is explained at the beginning of level 1. The breakability of terrain blocks is hinted at in the beginning of level 2. 

**Optimized Interactions**
- During crossplay, some players reported having a hard time seeing the ability buttons because they did not look like buttons and blended into the background. We have now updated the ability buttons so that they have a distinct button-like appearance.
- During crossplay, players found the tutorial text too long and had trouble understanding how to follow the instructions. We have how revised the tutorial to be more succinct, and all abilities are disabled except for the one the user is being told to use. 
- During crossplay, players were frustrated that the camera sometimes did not center on the active character, and that sometimes enemy characters would automatically move off screen and be difficult to "chase down" with the camera. We have now made it so that the camera always follows the active enemy character's movements. In addition, we have increased the camera boundaries so that active characters are always within the middle 1/3 of the screen (previously, they might be at the very edge of the screen), which makes them significantly harder to miss, and the game flow is easier to follow as a result. 



## Creative Components

**Story elements**
- Before the game, a slideshow presents the circumstances which led to the player being turned into candy and forced to fight for their life in Willa Wonky's Candy Factory. Each level of the game is a fight which takes place in a different room of the factory. The player faces specific challenges relating to the theme of that room. At the end of each level, the player picks up an ingredient also related to the theme. Upon completing all levels of the game, the player is presented with a slideshow showing how all of these ingredients are combined to turn them back in a human. 

**Numerous sophisticated integrated assets**
- We have added a wide variety of visually coherent sprites. All of our graphics are now in pixel art style, and the vast majoriity of the art added this milestone was created specifically for this project rather than a free asset. We now have menu buttons, unique backgrounds, terrain blocks, and level start/end messages for each level. We have also added title screen and menu graphics, ability buttons, character UI assets and more storytelling graphics. As mentioned, all of these graphics apart from the character turn indicator were created by our team!

**Particle system**
- We have implemented a particle system which is responsible for rendering moving snowflakes in the background of level 3. This was implemented using `glDrawElementsInstanced` and can be found in `render_system::drawSnow`. 

**Cooperative planning**
- We have added a new enemy type which is capable of healing other enemies. On their turns, they will try to first see if there's any enemy can be healed within range and heal them. If not, they perform the same AI as normal enemies. See `level_init::createEnemyHealer` and `EnemyController::make_decision`. 

**Game balance**
- The game is more challenging and interesting to play in this milestone compared to previous milestones. We have decreased the difficulty of the tutorial to reduce frustration, but we have increased the difficult of the actual game levels, including the first level, such that the player needs to develop a strategy to beat each level and cannot simply spam attacks towards the enemies and win. The addition of a 4th level and new enemy mechanics also serve to differentiate the levels to maintain user interest in the game. These updates can be seen in the `data/levels/.../init.json` files for each level. 

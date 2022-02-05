# Sugar Level: High

Created by Team 3
- Karen Lau 
- Lemuel Pranoto 
- John Shin 
- Marcus Thian
- Galila Zhang 
- Henry Zeng 

## Mandatory Requirements

### Rendering

**Loading and rendering of textured geometry**
- Rectangle sprites are used for all textured geometry. The loading is adapted from the assignment template in `render_system_init.hpp::init::17` where all shaders, textures, and geometries are loaded at the start of the game so that we do not reload during the gameplay. Rendering are performed by attaching `RenderRequest` component to an entity and specifying the shaders, textures and geometries used for rendering.  

**- Working basic 2D transformations**
- Basic 2D transformations are used for rendering all entities to the screen. The entry point for the rendering is `RenderSystem::drawTexturedMesh`.

**Key-frame/state interpolation**
- Characters and the camera are able to move smoothly between frames due to our implementation of the `PhysicsSystem::step` and `LevelManager::step` functions, respectively.

### Gameplay

**Keyboard/mouse control of at least one character sprite**
- Players can use 'A' and 'D' to move the character left and right respectively. The logic is implemented in `LevelManager::on_key::282` callback function. Mouse clicks are used for selecting buttons and performing attacks, and it is implemented in `LevelManager::on_mouse_button::375` callback function. We also implemented camera movement using arrow keys to move the camera within a specific range. It is implemented in `LevelManager::on_key::338` callback function.

**Randomimzed or hard-coded action of one or more characters/assets.**
- here

**Well-defined game space boundaries**
- Playable space boundries are created by surrounded terrains such that characters will not fall off the platform. The terrains are created in `LevelManager::load_level::45`. There is also boundries for camera movement, which are limited by `vec2 lower_limit` and `vec2 higher_limit`. The movement of camera are specifically limited under `physic_system::step::50`.

**Correct collision processing**
- Collisions are handled in `LevelManger::handle_collisions` function. It is used to handle attack object and terrain collisions. Attack object collisions are handled such that if an attack object collide with another damagable character, it deals damage to the character and create an Hit Effect object. Terrain collisions are currently handled such that if there is a collision between a Terrain and Enemy/Player, restore the position of the Enemy/Player at the start of this frame. However, this approach does not support our implementation of gravity, so we disabled gravity (gravity is not necessary in M1) and plan to improve the terrain collision handlings in the later milestones.

### Stability 

- Our game runs without any lagging when whether camera movement, character movement or rendering effects are taking place. Continuing execution is supported until the user closes the game window. 

## Creative Components

### Simple rendering effects
- When a character is attacked, its sprite will flash red and shake. This effect is first initialized by a call to `createHitEffect` in `LevelManager::handle_collisions`.

### Camera controls
- The in-level camera pans horizontally when the left and right arrow keys are pressed. UI elements such as buttons are unaffected by the camera perspective and remain fixed. `createCamera` is called in `GameSystem::init` to initialize a camera entity.
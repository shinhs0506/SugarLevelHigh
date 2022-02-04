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
- here

**- Working basic 2D transformations**
- Basic 2D transformations are used for rendering all entities to the screen. The entry point for the rendering is `RenderSystem::drawTexturedMesh`.

**Key-frame/state interpolation**
- Characters and the camera are able to move smoothly between frames due to our implementation of the `PhysicsSystem::step` and `LevelManager::step` functions, respectively.

### Gameplay

**Keyboard/mouse control of at least one character sprite**
- here 

**Randomimzed or hard-coded action of one or more characters/assets**
- here

**Well-defined game space boundaries**
- here

**Correct collision processing**
- here

### Stability 

- Our game runs without any lagging when whether camera movement, character movement or rendering effects are taking place. Continuing execution is supported until the user closes the game window. 

## Creative Components

### Simple rendering effects
- When a character is attacked, its sprite will flash red and shake. This effect is first initialized by a call to `createHitEffect` in `LevelManager::handle_collisions`.

### Camera controls
- The in-level camera pans horizontally when the left and right arrow keys are pressed. UI elements such as buttons are unaffected by the camera perspective and remain fixed. `createCamera` is called in `GameSystem::init` to initialize a camera entity.
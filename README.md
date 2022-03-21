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
- We have fixed a bug related to our physics systems, which was discovered during the last crossplay session. Characters and their respective health bars are now in sync and react the same to gravity, rather than having slightly different positions due to be affected by gravity in different ways as before.  
- Interpolation is now used to determine character movement in level 2. 
- We have fixed a bug which led to unpredictable behaviour when the camera was moved in-level and the player returned to the menu screen. 

## Creative Components

**Parallax scrolling**
- description here

**Audio feedback**
- description here

**Basic integrated assets**
- More assets have been added to `data/textures`, including new textures for all buttons in the game, textures for ladders which tile according to the height of the ladder, visuals for character attacks, on-screen prompts for tutorials and level-endings, and new backgrounds. 
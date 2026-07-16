
# GameLab Project Repository

|  General Info  |                                                                                                |
| ---|------------------------------------------------------------------------------------------------|
| Working Title | `Pipes`                                                                                        |
| Final Title | `Pipes`                                                                                        |
| Student | `Maritza Alfonso Rodriguez`, `maritza.alfonso-rodriguez`@stud-mail.uni-wuerzburg.de, `s452739` |
| Target Platform(s) | `Windows`                                                                                      |
| Start Date | 21.10.2025                                                                                     |
| Study Program | Games Engineering B.Sc.                                                                        |
| Engine Version | Unity 2021.3.11f1 or Unreal 5.0.3                                                              |

### Abstract

Life after humanity is hard, as the only sentient being left on earth you are forced to solve puzzles to try to escape the factory that was supposed to take your consciousness, while mindless, fellow robots try to stop you. Will you succeed? 


## Repository Usage Guides

```
RepositoryRoot/
    ├── README.md           // This should reflect your project 
    │                       //  accurately, so always merge infor- 
    │                       //  mation from your concept paper 
    │                       //  with the readme
    ├── builds/             // Archives (.zip) of built executables of your projects
    │                       //  including (non-standard) dependencies
    ├── code/
    │   ├── engine/         // Place your project folder(s) here
    │   ├── my-game-1/      // No un-used folders, no "archived" folders
    │   ├── CMakeLists.txt  // e.g. if using CMake, this can be your project root
    │   └── ...
    ├── documentation/      // GL2/3 - Each project requires FULL documentation  
    │                       //   i.e. API Docs, Handbook, Dev Docs
    ├── poster/             // PDF of your Poster(s)
    ├── report/             // PDF
    └── trailer/            // .mp4 (final trailer, no raw material)
```
## Trailer 

### Used sounds:

- 19 Sniff.wav Link:
https://freesound.org/people/Alshred/sounds/403773/

- Male Gasp 1.wav Link:
https://freesound.org/people/jawbutch/sounds/344407/

- crab walking Link:
https://freesound.org/people/stuniverso/sounds/761559/

## External Tools and Libraries

- Box2D: https://box2d.org/
- glad: https://glad.dav1d.de/
- glfw: https://www.glfw.org/
- glm: https://glm.g-truc.net/0.9.9/index.html
- stb_image: https://github.com/nothings/stb/blob/master/stb_image.h
- Google Gemini: https://gemini.google.com/ (used for C++ troubleshooting and documentation)



## Used Assets in Game

### Sounds effects:

- Hint,Clue,Trace,01.wav Link:

https://freesound.org/people/LilMati/sounds/441266/

- Adding an item to inventory 2 Link:

https://freesound.org/people/randbsoundbites/sounds/829806/

- Crowd.Yay.Applause.25ppl.Long.wav Link:

https://freesound.org/people/jessepash/sounds/139973/

- crab walking Link:

https://freesound.org/people/stuniverso/sounds/761559/

- Dropping Pipe on Hard Floor Link:

https://freesound.org/people/whi1ter1ce/sounds/708061/

- Electricity Game Sound Pack:

- charge.wav

- hit.wav

- crackleelectricityloop.wav

Link: https://opengameart.org/content/electricity-game-sound-pack

- Scrapes Link:

https://opengameart.org/content/scrapes

- old elevator door Link:

https://opengameart.org/content/old-elevator-door

- Metal Interactions Link:

https://opengameart.org/content/metal-interactions

- Light Switch Turn On And Off SFX Link:

https://opengameart.org/content/light-switch-turn-on-and-off-sfx-0

- mission failed Link:

https://freesound.org/people/buffsatan/sounds/760731/

### Music:

- Factory ambiance Link:

https://opengameart.org/content/factory-ambiance

- Ambient/loading screen game music Pack Link:

https://opengameart.org/content/ambientloading-screen-game-music-pack


## Hardware Requirements

PC, Keyboard, Mouse, Audio output device

## How to execute game:

1. Extract files from zip-file
2. Open the "Pipes" executable


## How to compile project:

1. You can clone the code with this command:

git clone --recursive --depth 1 https://gitlab2.informatik.uni-wuerzburg.de/GE/Teaching/gl3/projects/2025/22-gl3-alfonso-rodriguez.git

2. Once cloned, you can open the root folder in CLion. And load the `CMAKEList.txt` file

3. In CLion go to Settings->Build,Execution,Deployment->Toolchanins add the Microsoft Visual Studio Compiler (2019 or 2022) and hit Apply

4. You should now have a running configuration -> compile the code and have fun!



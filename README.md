<a name="readme-top"></a>

<!-- GENERAL GAME INFO -->
<br />
<div align="center">

  <h1 align="center">Shovel Knight: Treasure Trove</h1>

  <p align="center">
    Short description of the game.
    <br /> 2D Platformer where you fight enemies with a shovel.
    <strong> Original game : https://store.steampowered.com/app/250760/Shovel_Knight_Treasure_Trove/ </strong>
    <a href="https://www.google.be/"><strong>General info »</strong></a>
    ·
    <a href="https://www.google.be/"><strong>Youtube video: https://www.youtube.com/watch?v=4XGMh1Uoxs4&t=617s  »<strong></a>
    <br />
    <br />
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
    </li>
    <li>
      <a href="#my-version">My version</a>
    </li> 
    <li>
      <a href="#getting-started">Getting Started</a>
    </li>
    <li><a href="#how-to-play">How To Play</a></li>
    <li><a href="#class-structure">Class structure</a></li>
    
    <li><a href="#checklist">Checklist</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

![image](https://github.com/HowestDAE/dae14-SantiagoHerreraH/assets/129545544/4af91401-7be0-41e1-b956-79dc5662edc1)

Here's why:
TODO: describe why you chose this game 
* Sprites available
* Adheres to features solicited by the subject

<p align="right">(<a href="#readme-top">back to top</a>)</p>


## My version

This section gives a clear and detailed overview of which parts of the original game I planned to make.

### The minimum I will most certainly develop:
First Level without the boss fight:

Content

AI
* Slime enemy
* Big Dragon enemy
* Small Dragon enemy

Level
* First Level

Abilities
* Basic Mobility
* Added Mobility with Shovel
* Destructible Terrain

Collectibles
* Life
* Gold

Systems For the Content 

Already Made: (some just need tweaking but all of them are already functioning))
* Entity Component System
* Dynamic Iterator System
* Scene Graph
* Event Graph
* State Graph/Machine (functioning, but functionality will need to be added in the future)
* Movement System
* Matrix Transform System (the framework that was given has this implemented, but I made my own before receiving the framework)
* Camara System
* Collision System
* Physics System
* Input System -> a modular way of linking input and events
* Curve (Bezier Curves) System -> useful for controlling values over time or visually correlating variables
* Vector Math and Collision Math -> implemented my own before being given one
* Modified Texture System -> slightly modified the given framework texture system to allow for dynamic references to textures
* Creation System (a more organized way of creating entities in world, managing component coupling and providing string identification for each group of components, which will make creatiing an editor easier)

Systems that need testing but are already implemented:
* Animation System
* Parallax System

Systems to be Made
* Projectile System
* Expanding the Event and State System via introducing more events and more actions to execute upon reacting to those events -> basically, just expanding the state machine for the AI
* Destructible terrain System

Details to Iron out if there is time
* There are a bunch of optimizations I can do in my ECS, some of them being:
* Refreshing dynamic iterators only when memory is moved and not with every creation of components. For the time being, this is not a big issue since all of my allocation happens upon initialization. For a bigger game, I understand this would an issue
* Removing unnecesary archetypes so they don't take space in memory
* Giving the option to refresh the dynamic iterators manually. This would require a whole restructuring of my ECS, which is out of scope for this project. At least now I know how to do a better ECS next time. 

### What I will probably make as well:
AI
* Skeleton enemy

Scenes
* Cut Scenes at the beginning of a level

UI
* Basic implementation of the UI, including:
* Narrowed main menu with no character selection and no world map
* Basic features like audio regulation and pause and play 

"Particle System"
* The part of Shovel Knight I am going to reproduce doesn't really have particles, it's just reacting sprites upon certain events

### What I plan to create if I have enough time left:
AI
* Final Boss

Systems
* Editor System
* Save System

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- GETTING STARTED -->
## Getting Started
Detailed instructions on how to run your game project are in this section.

### Prerequisites

This is an example of how to list things you need to use the software and how to install them.
* Visual Studio 2022

### How to run the project

Explain which project (version) must be run.
* any extra steps if required 

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- HOW TO PLAY -->
## How to play

Use this space to show useful examples of how a game can be played. 
Additional screenshots and demos work well in this space. 

### Controls
* WASD to move

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CLASS STRUCTURE -->
## Class structure 

Overall structure of my systems:
- Registry contains things known as components.
- Components can be linked and referenced through and Entity
- Component Iterator allows for iteration over components that meet certain linkage criteria
- Systems iterate over the components, transforming them in the process
- Main systems are classes that are composed of subsystems.
- While main systems group a semantically similar subsystems, sybsystems hold the data transforming code itself
- There are various classes that need to be associated between each other to track and automatically refresh state when there is:
    - memory movement
    - data change

### Entity Component System

Classes in this System:

Component
- Any class
- If it's not a singleton, must be copyable and movable in the constructor and assignment operator

Registry
- Manages component life creation and destruction cycle
- Stores components
- Stores contextual information fo the components such as: which are linked with which
- Provides filtering functionality (retreives set of components are linked with "x" and not with "y")
- Works together with other systems to iterate over them given the filtering parameters and the various memory movements thanks to memory allocation
- Contains the following classes (class composition):
- Archetype Registry
- map of std::type_index to Component Compartments

Archetype and Component Identification
- FEntity: a struct that contains Archetype Id and Entity Id
- Entity Id: identification for a single group of linked components
- Archetype Id: identification for group of entities with x type of components
- Hash Id: a "unique" number for each entity
- Archetype Key: a std::vector<bool> (which is hashable) that registers which archetype has which components 
- DArchetype -> set of std::type_index that knows which archetype has which components (not hashable)
- UArchetype -> wrapper class of DArchetype that registers components in the given Registry upon construction
- Internal Filter -> variation of UArchetype for different type of implementation (internal)
- Component Filter -> variation of UArchetype -> class for ECS user

Archetype Registry
- Manages Archetype and Component Registration upon component creation or registration

Component Compartments
- Matrix that stores the components of one type. One row per archetype.

Component Iteration
- Component Iterator:
    - Iterates over an array of components which pass x filter (be it excluding or including)
    - Automatically refreshes the references (even if there is std::vector movement in the background), thus, it's never invalid
    - The reason I made this design decision is that it is more efficient to only refresh the Iterator when there is memory movement in the std::vector that is containing the component, than to do it "manually" each tick. I could implement the "Dirty" pattern and have a system to gradually refresh the iterators, but I think that is overkill for now.
    - Has an independent life cycle from its manager
- Component Iterator Manager (associates with Component Iterators)
    - Manages the refreshing, adding, activating, deactivating and deleting of the Component Iterators of one type
- Component Iterator Registry (composed of Component Iterator Managers)
    - Manages the component managers of x types

Entity Storage and Component Referencing
- Entity Handle: when you add or remove a component from an entity, its entity id, archetype id and hash id change. The entity handle is automatically updated when the entity is changed
- Entity Handle Registry: manages the updating of the entity handles
- Entity Handle Container: Contains entity handles in a flat set manner (contiguosly but allowing it for query via the hash id). Automatically refreshes the hash id
- Entity Handle Map: inherits from Entity Handle Container. Same function but allows you to store an extra type besides an entity handle 
- Entity Handle Container Registry: manages the updating of the entity handle registry and map

### Scene Graph
- Main Systems that Determines Spatial Transformation Hierarchy
- Executes the actual transformations and converts the data of an object from position, rotation and scale to actual points in the screen

### Event Graph
- Allows linkage of Events and EventListeners
- Events are just concepts identified with a string like "OnCollision" or "OnJump"
- EventListeners are the actual classes that execute the code that triggers when the events happen
- All Event Listeners for now derive from UEventListener for implementation purposes
- It is a graph because Events can also trigger other events
  
### State Graph/Machine 
- Follows the implementation of the Event graph, however, when an event triggers, the State Grph allows for different data to be modified prior to the execution of the event listener
### Movement System
Right now, the movement systems implemented are the following:
- Honing System: a honing system that allows you to regulate the honing speed relative to the distance of the target
- Vector Movement: moves the object in the direction of the vector
- Vector Movement Over Time: same concept as Vector Movement, however, the magnitude of the Vector Changes over time depending on the unpit curve for the rate of change
### Matrix Transform Systems 
- Sub Systems that constitute Scene Graph that allow for different types of transformations
- Local Transforms (component) let you transform the object in local space in a scene hierarchy
- World Transformw (component) let you transform the object in world space (always orthogonal and unit metric)
- Real Transform -> Tells you the actual location, rotation and scale in world space and screen space
### Camara System
- Transforms the whole world around the camera position
- Normalizes the world to the center of the window
### Collision System
- System for Shape Collision
- Optimized to only check for shapes inside the same grid cell. Works via hashing an objects position to a cell index 
### Input System
- Triggers input events on input state change
### Curve (Bezier Curves) System
Allows for bezier curve manipulation

### Physics System
Implements velocity, mass and forces to objects 

### Creation System 
Facilitates component creation and attaches a string to a group of components (easier to implement an editor later)
### Other special container classes
- Activatable Vector -> vector with activation and creation lock functions. Always keeps activated components contiguos
- Unordered Vector -> aka flat set -> contiguos in memory but allows queries for elements

Systems that need testing but are already implemented:
* Animation System -> system that allows to play a group of frames ina certain order at a certain speed.
* Parallax System -> applies principal of 3D projection to give the parallax sensation 

### ..

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- CHECKLIST -->
## Checklist

- [x] Accept / set up github project
- [x] week 01 topics applied
    - [x] const keyword applied proactively (variables, functions,..)
    - [x] static keyword applied proactively (class variables, static functions,..)
    - [x] object composition (optional)
- [x] week 02 topics applied
- [x] week 03 topics applied
- [x] week 04 topics applied
- [x] week 05 topics applied
- [x] week 06 topics applied
- [x] week 07 topics applied
- [x] week 08 topics applied
- [x] week 09 topics applied (optional)
- [x] week 10 topics applied (optional)

<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- CONTACT -->
## Contact

Your Name - email@student.howest.be

Project Link: [https://github.com/your_username/repo_name](https://github.com/your_username/repo_name)

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

Use this space to list resources you find helpful and would like to give credit to. 
* All of cpp reference
* stack overflow from time to time
* https://gameprogrammingpatterns.com/ -> Also buy the book if you can, it's good
* https://refactoring.guru/ <- gives you a good intution for pattern structure. I find it very liberating to think about structure and code execution principles instead of a rigid implementation. 
* [Example 1: cpp reference on std::vector](https://en.cppreference.com/w/cpp/container/vector)
* ..

<p align="right">(<a href="#readme-top">back to top</a>)</p>


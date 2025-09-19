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

Why I chose the game 
* effective gameplay that requires systems that I want to build

<p align="right">(<a href="#readme-top">back to top</a>)</p>


## My version

### Gameplay Features:
First Level without the boss fight:

Content

AI
* Beetle enemy
* Blorb
* Dive Drake

Level
* First Level

Abilities
* Basic Mobility
* Added Mobility with Shovel
* Destructible Terrain

Collectibles
* Life
* Gold

### Game Engine Features:

* Entity Component System
* Dynamic Iterator System
* Scene Graph
* Event Queue
* State Graph
* Movement System
* Camara System
* Collision System
* Physics System
* Input System -> a modular way of linking input and events
* Curve (Bezier Curves) System -> useful for controlling values over time or visually correlating variables
* Vector Math and Collision Math -> implemented my own before being given one
* Modified Texture System -> slightly modified the given framework texture system to allow for dynamic references to textures
* Creation System (a more organized way of creating entities in world, managing component coupling and providing string identification for each group of components, which will make creatiing an editor easier)
* Animation System
* Parallax System
* Destructible terrain System
* Screen Culling so only the objects inside screen view are activated
* Level Reader that reads pixel data in images and creates a level based on tile sheet rules


<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- GETTING STARTED -->
## Getting Started
Detailed instructions on how to run your game project are in this section.

### Prerequisites For Playing

* Windows

### How to play the project

* Download the project from the repo
* Go to Build folder
* click on 1DAE14_GameProject_SantiagoHerreraHuerta.exe
* Wait a bit for the project to load

<p align="right">(<a href="#readme-top">back to top</a>)</p>


<!-- HOW TO PLAY -->

### Controls
* AD to move horizontally 
* SPACE to jump
* S while on air is shovel down
* J to shovel horizontally

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CLASS STRUCTURE -->
## Class structure 

Overall structure of my systems:
- Registry contains components.
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


<!-- CONTACT -->
## Contact

Santiago Herrera Huerta
santi.herrerah.h@gmail.com

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
* Of course Shovel Knight gameplay as reference

<p align="right">(<a href="#readme-top">back to top</a>)</p>


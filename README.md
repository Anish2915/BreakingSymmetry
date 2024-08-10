# InterIITGamejam13.0

# Breaking Symmetry

## 📖 Description

Breaking Symmetry - A Dynamic 2D Puzzle Adventure

Welcome to "Breaking Symmetry", a captivating 2D puzzle game that challenges your intellect and creativity with ever-breaking and making rules. Immerse yourself in a world where each level is a unique puzzle, and the rules governing the gameplay are as dynamic as your imagination.

## 📝 Table of Contents

- [Features](#features)
- [How to Play](#how_to_play)
- [Approach](#challenges)
- [Contributing](#contributors)

## <div id = features>✨ Features</div>
<ul>
  <li><strong>Innovative Gameplay:</strong> <em>Breaking Symmetry</em> introduces a groundbreaking concept where the rules of the game can be dynamically altered. Adapt to ever-changing conditions, think outside the box, and discover new solutions as you progress through a series of mind-bending puzzles.</li>

<li><strong>Dynamic Rule System:</strong> Every level presents a set of rules that dictate how the puzzle behaves. What works in one level may not apply to the next, keeping you on your toes and challenging you to adapt your strategy continuously.</li>

<li><strong>Unlimited Creativity:</strong> Experiment with different rule combinations to unlock creative solutions as you can change forms or pass things from one side to another. The game encourages players to explore unconventional approaches, fostering a sense of discovery and innovation.</li>

<li><strong>Engaging Puzzles:</strong> Encounter a variety of puzzles, each designed to test different aspects of your problem-solving skills. From logic challenges to spatial reasoning, PuzzleVerse offers a diverse range of brain-teasing scenarios.</li>
</ul>

## Gameplay Video 
https://github.com/Anish2915/InterIITGamejam13.0/assets/137883198/4c7f2773-da76-4193-ae9a-705758311f19

## <div id = how_to_play>🎮 How_to_Play</div>
 The game consists of two 12 X 12 matrices and each player will be there according to the controls shown.
 Only the WASD keys movement is allowed with the rules as specified on the board.
 
<img src = "https://github.com/Anish2915/InterIITGamejam13.0/blob/main/Game_img.jpeg">

<p>As in the image above, Red player controls are defined in the left matrix, and the blue player rules are defined in the right matrix. Since S is on the UP arrow on the left, red moves up upon pressing S. A similar fashion is followed for all other controls. Controls are written on the square and the objects move over it. </p>
<p>By default, text ROCK is at the stop so you can't go over rocks or push them. But if you bring text ROCK on the PUSH by pushing it, then you can push every rock. Players can push very "text" written to anywhere. If the text is on nothing then the player can pass over it and nothing will happen. Like if text ROCK is on nothing then one can pass over the rocks.
If you remove BLUE from PLAYER1 or RED from PLAYER2 then those will no longer be players. If you bring some other texts say LASER on a player then a laser predecided in the level will be the current player for the respective player.</p>

<p> TNTs are for creating blasts on the square they are pushed on if they strike something already lying there, but if something pushes TNT then the blast does not occur. Laser lights shown by an 'X' mark in the image above can destroy anything in their paths if its TEXT is on something.</p>

<p>A local leaderboard is maintained that will be according to the number of levels completed and the moves made. Lasser the moves made, the good game it will be and so high performance on the leaderboard.</p>

<ol><b>Note:</b> <li>One can always push the texts and TNTs.</li>
                 <li>Player and texts can't be passed to the other matrix.</li>
</ol>

## <div id = challenges>❔ Challenges</div>

Creating a dynamic and engaging 2D puzzle game posed several challenges, and overcoming them required innovative solutions. Here are the primary challenges faced during the development of PuzzleVerse:

1. **Dynamic Object Storage:**
   - *Challenge:* Efficiently storing and managing dynamic objects with varying properties.
   - *Solution:* Implemented classes and utilized a linked list structure to store objects of the same class. Enumerated dynamic object types within the class for streamlined management.

2. **Rule Modification System:**
   - *Challenge:* Enabling dynamic rule changes for individual puzzle elements.
   - *Solution:* Developed a flexible rule modification system by defining a set of dynamic rules as enums within the class. Implemented a function using a linked list to apply rule changes uniformly across all instances of the dynamic objects.

3. **Code Optimization:**
   - *Challenge:* Ensuring code efficiency while handling dynamic puzzles, rules, and interactions.
   - *Solution:* Leveraged 3D vectors for spatial representation, employed linked lists for dynamic object management, and implemented optimized algorithms to enhance overall code performance.

4. **Class Interface Design:**
   - *Challenge:* Defining a cohesive interface for the parent class and its derived children to ensure a consistent structure.
   - *Solution:* Implemented abstract classes with well-defined interfaces, allowing derived classes to inherit and implement specific functionalities. This approach streamlined the development process and rules updation and maintained a clear structure for future expansions.


The combination of class structures, linked lists, 3D vectors, and optimized algorithms proved instrumental in addressing these challenges and contributed to the successful development of PuzzleVerse. Each solution was tailored to enhance gameplay dynamics while maintaining code efficiency and flexibility.


## <div id = contributors>🧑🏽‍💻 Contributors</div>
<ul>
  <li>Anish Kumar</li>
  <li>Prakhar Moses</li>
  <li>Umakant Sahu</li>
</ul>

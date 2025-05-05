# Whomp 'Em NES (Simple PC Version)

**Authors**:  
Marc Escribano Sierra  
Sergi Sanmartín Soria

## 🕹️ Gameplay Features

### Player Mechanics
The player character has a full set of animated actions:
- Attack while jumping
- Attack while crouching
- Attack while walking
- Attack while standing still
- Attack upwards while jumping
- Attack downwards while falling
- Shielding (block)
- Movement (walk/run)
- Jump
- Switch weapon at the totem — currently, the Ice Totem, which freezes enemies on contact

### Power-Ups
- **Small Heart**: Restores one heart
- **Big Heart**: Restores the entire row of hearts
- **Pumpkin**: Grants a new heart after collecting several (as in the original game)
- **Flint**: Increases attack power; enemies like ogres die in one hit (boss excluded). Lasts for 4 contact attacks
- **Helmet**: Prevents damage for 4 hits
- **Deerskin Shirt**: Grants invincibility and invisibility for 5 seconds
- **Magic Potion**: Adds an extra row of lives
- **Spear**: Increases spear length for the rest of the game

### Environment and World
- Moving platforms
- Enemy freezing mechanic using the Ice Totem
- Skull triggers a big heart power-up, like in the original game
- Level map: **Sacred Woods**

### HUD
- Displays player hearts
- Remaining magic potions
- Current weapon
- Active power-ups (helmet, flint, deerskin shirt) at the bottom

---

## 🔊 Sound

- Background music (level and boss)
- Sound effects for:
  - Jumping
  - Player taking damage
  - Boss death
  - Collecting power-ups
  - Spear and Ice Totem usage
  - Menu navigation and selection
- Victory music
- Game over music

---

## 🖥️ Screens

- Start menu
- Credits
- Instructions
- Victory screen
- Game Over screen

---

## 🎮 Controls

### Player
- `C`: Switch weapon
- `X`: Attack (spear / Ice Totem)
- `Z`: Jump
- `↑`: Shield (block)
- `↓`: Crouch
- `→`: Move right
- `←`: Move left

#### Combos:
- `Z + X`: Attack while jumping
- `Z + ↑`: Upward attack while jumping
- `Z + ↓`: Downward attack while falling
- `↓ + X`: Crouching attack
- `←/→ + X`: Walking attack

### Debug
- `H`: Recover all hearts
- `G`: God mode (invincible)

---

## 👾 Enemies

### Bamboo
- Bamboo pieces fall from the sky at fixed x-positions.
- The only way to stop them is by touching them while shielding.
- Bamboo on the ground also causes damage.

### Ogre
- Has two lives.
- Continuously jumps toward the player.

### Snakes
- Two snakes appear at a time and patrol horizontally.
- They regenerate when offscreen or eliminated.

> Ogres and snakes randomly drop power-ups.  
> Enemies spawn in the same positions as in the original game.

### Final Boss
- Starts in a fixed position, then flies and shoots bamboo from its hands.
- After 8 seconds or after being hit, it enters a flying state for 2 seconds or until hit again.
- Then it descends, disappears, bamboo falls, and leaves animate as it reappears.
- This cycle repeats.
- The boss's health is visible in the HUD.
- Upon death, the boss drops its totem and the victory screen appears.

---

Enjoy the game and may your spear strike true!

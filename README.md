<img src="logo.png" height="150" alt="DELTADASH Mod Header"/><img src="resources/deltadash_header.png" height="150" alt="DELTADASH Mod Header"/>

Bring Deltarune's Battle System to Geometry Dash!

# Overview
***DeltaDash*** Replicates a TON of aspects from Deltarune's battle system to spice up your GD Gameplay! Such as:
- A fully fledged HP System!
- A Party System like Deltarune's with up to 3 customizable Party Members!
- Deltarune's Grazing + TP Gaining mechanics!
- Individual Party Member actions like Healing and Defending!
- A leveling system to make your own player stronger!
- A visuals-only "True Player" mode to avoid cheating!

# Features
- **Customizable Party**: Choose up to 3 party members to use in levels, use Deltarune's characters like Kris, Susie, Ralsei, Noelle, or even use your own player icon if you wanna self-insert!
- **HP System**: No longer will you die immediately when hitting a hazard, as your party will take DAMAGE instead! Each character has their own MAX HP values, with some extra bonuses for Demon Levels to make things easier!
- **Damage System**: The harder the level you're currently playing is, the more damage your party will take when hit! On easier levels, only one member will take DAMAGE, but on Demon Levels all your party will take higher damage all at once!
- **Turn-based Actions System**: While in a level, you can execute certain actions with all your party members! You can HEAL to recover some lost HP by using TP, or you can DEFEND with any party member to make the next 3 hits you take deal half the damage (and gain some TP in the process)! You can only run actions with one member at a time, though, so plan out your strategies!
- **Leveling Up System**: One of the available party members you can choose from is you, the "Player" character! This character starts out weak, but gains +20HP every 100 starts you obtain! Beat levels to **Get Stronger.**
- **TP Gaining System**: Get close to hazards without fully touching them to GRAZE and build up TP, like in Deltarune! TP can then be used to HEAL!
- **Faithful Deltarune UI & Visuals**: The mod faithfully recreates the Battle UI from Deltarune, as well as the Grazing visual effect you see in the game! You will also see Label Indicators on screen when taking damage & healing, showing you how many DAMAGE you took, if any party member was DOWNED, or how much HP you healed!

# Customization
The mod allows you to customize some aspects of it too, via the mod's settings page!
- **UI Scale**: Adjust the size of the party tabs to your preference, so they don't take up much of your screen and don't mess with your gameplay!
- **Damage Indicator Position**: Choose between following your player's screen pos or staying at a fixed, Deltarune-style position, at the left of the screen!
- **Party Characters**: Mix and match any combination of characters, or go solo with just one!
- **True Player Mode**: A fully-solo character with only 1HP, in case you like the mod's visuals but don't wanna cheat, this character/mode respects GD's gameplay!

## Activation Modes
The mod's effects (we'll call them Battle Mode) can be activated in mainly 2 ways, from which you can choose whichever one you prefer in the mod's Settings Page:
- **On Start**: Battle mode activates as soon as the level begins and persists across restarts.
- **Mid-Game Keybind** (Default: Q): Activate battle mode whenever you want during gameplay, and have them be disabled automatically on a level restart! The mod won't do anything until you activate it manually in-game, if you desire to do so.
*Additionally, the mod can be easily toggled via the mod's settings page, so you don't have to Disable/Uninstall the mod to turn it off.*

# Actions & Keybinds
These are the actions you can perform in-game, as well as their default keybinds (can be changed via Custom Keybinds mod):

- C: **Heal Prayer** - Heals the currently active party member. (32% TP cost)
- V: **Defend** - Defends the currently active party member, making the next 3 hits they take do half the damage. (16% TP gain)
- Q: **Activate Battle Mode** (only if Mid-Game Keybind mode)

# Damage System Details
Some more in-depth details about how the Damage System is handled in-game!
How much Damage a member takes is chosen randomly, each level difficulty has a range of Min-Max damage assigned, and every time damage is taken, a random number within that range is generated.

### Non-Demon Levels
Damage is dealt to a randomly chosen party member. Damage range scales with level difficulty (6 stars = 10-20 damage, 9 stars = 20-40 damage).
Party size applies damage multipliers too:

- 1 party member: **1x damage**
- 2 party members: **1.25x damage**
- 3 party members: **1.5x damage**

### Demon Levels
Everyone takes damage at once! Damage range is a LOT higher (Easy = 30-60, Extreme = 50-80).
Party size multipliers also get rougher:

- 1 party member: **1x damage**
- 2 party members: **1.5x damage**
- 3 party members: **1.75x damage**

Defending reduces all damage taken by the Defending party member by half! AKA, you only take 50% damage!

# Credits
- **Toby Fox**: Creator of Undertale & Deltarune.
- [**Erymanthus**](user:1941705): Code from their Jesus mod was used as a base for the Grazing/TP Gaining logic.
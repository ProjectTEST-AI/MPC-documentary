# Documentation for a self-made raycaster using mLog in mindustry logic.

##  Concepts - Tasks
- (TODO) Rendered walls will be in one color in this stage `textures will come later.`

- (TODO) Two output screens,  one for the rendered space (walls)
                       one for top-down view of player with the current ray being casted/rendered

- (TODO) Multiprocessing capabilities,  depending on the ammount of connected processors, a main processor will divide raycasting tasks evenly throughout all of them.

- (TODO) Movement, X and Z axis movement with rotation `Y axis (up-down) will be added later.`

- (TODO) Memory bank, we will use 2 memory banks in total. Data placement in the banks are in the documentation section.



##  Documentation
### - Data placement in Memory bank
> BANK1 :
>> - #0 = a
>> - #1 = b
> 
> BANK2 :
>> - #0 = Distance from the player to the position where ray[0] detected to hit a wall
>> - #1 = Distance from the player to the position where ray[1] detected to hit a wall
>> - #2 = Distance from the player to the position where ray[2] detected to hit a wall
>> - and so on.
>> (NOTE: default value is -1, to indicate rays that are processed)

### - Variable Denotation
  Most if not all variables are or will be cut down and abbreviated to fit mindustry's small variable box whilst still being readable in a glance. any complaints will be **ignored asf** (:D)
> - 
> - 
> - 


# Documentation will be updated with time.

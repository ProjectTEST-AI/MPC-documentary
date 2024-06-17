# Documentation for a self-made raycaster using mLog in mindustry logic.

##  Concepts - Tasks
- (TODO) Rendered walls will be in one color in this stage `textures will come later.`

- (TODO) Two output screens,  one for the rendered space (walls)
                       one for top-down view of player with the current ray being casted/rendered

- (TODO) Multiprocessing capabilities,  depending on the ammount of connected processors, a main processor will divide raycasting tasks evenly throughout all of them.

- (TODO) Movement, X and Z axis movement with rotation `Y axis (up-down) will be added later.`

- (TODO) Processor, the processor will do some math and come up with the distance between the player and the position where the ray is detected to be hit.

  We would have multiple raycast cores/procs and connected in paralel. each proc will have a hardcoded number assigned to them as their core number, and another number that indicates the total ammount of cores available. Theese numbers will be crucial, as each core/processor will self assign which ray they are going to be casting, with the formula:

  ``` lastRay = coreNumber ``` \
  ``` currentRay = lastRay + totalCoreAmmount ```

  After the processor comes up with the distance, it will output to the corresponding cell in the memory bank. 

- (TODO) Memory bank, we will use **atleast** 2 memory banks. Data placement in the banks are in the documentation section.



##  Documentation
### - Data placement in Memory bank
> BANK1 :
>> - #0 = Distance from the player to the position where ray[0] detected to hit a wall
>> - #1 = Distance from the player to the position where ray[1] detected to hit a wall
>> - #2 = Distance from the player to the position where ray[2] detected to hit a wall
>> - and so on.
>> (NOTE: default value is -1, to indicate rays that are processed)
> 
> BANK2 and so on.. :
>> - #0 = position X of the start of wall #0
>> - #1 = position Y of the start of wall #0
>> - #2 = position X of the end of wall #0 
>> - #3 = position Y of the end of wall #0
>> - and so on.
>> (NOTE: default value is -1, to indicate rays that are processed)

### - Variable Denotation
  Most if not all variables are or will be cut down and abbreviated to fit mindustry's small variable box whilst still being readable in a glance. any complaints will be **ignored asf** (:D)
> - Main processor
> - Raycast processor


# Documentation will be updated with time.

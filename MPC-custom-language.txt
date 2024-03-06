## MPC custom language
ID <Command-name> [Readable data structure]
{Separated data structure} ; {Encoded data structure}<digit count>  *REMEMBER, MAX 15 DIGITS!*
(Description)

## Data structure information
- <LocationID> | sourceID | destinationID
  (location identifier of specified data
  10 = self
  11 = self-section1
  20 = CPU (L3)
  30 = GPU
  40 = RAM
  50 = SSD (non-perma storage)
  60 = external storage
  )


# Custom lang
// Important stuff (-00)
-01 <End-of-instructions> [ID]
{-01} ; {-01}

// Input & output (10)
11 <Singular-Data-Displacement> [sourceID sourceBank# sourceCell# : destinationID destinationBank# destinationCell#]
{11 12 123 123 : 11 12 123 123} ; {1112123123 : 1112123123}<10 : 10>
(Moves a singular specified data to destination)

12 <Multiple-Data-Displacement> [sourceID startSourceBank# startSourceCell# endSourceBank# : endSourceCell# destinationID destinationBank# destinationCell# dataType]
{12 12 123 123 123 : 12 123 12 123 123 1} ; {1212123123123 : 12123121231231}<13 : 14>
(Moves multiple data to specified destination)

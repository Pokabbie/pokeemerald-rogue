MysteryGiftScript_AlteringCave::
	setvaddress MysteryGiftScript_AlteringCave
MysteryGiftScript_AlteringCave_:
	lock
	faceplayer
	vmessage sText_MysteryGiftAlteringCave
	waitmessage
	waitbuttonpress
	release
	end

sText_MysteryGiftAlteringCave:
	.string "Thank you for using the MYSTERY\n"
	.string "GIFT System.\p"
	.string "There appears to be a rumor about\n"
	.string "rare POKéMON sightings.\p"
	.string "The sightings reportedly came from\n"
	.string "the ALTERING CAVE on ROUTE 103.\p"
	.string "Perhaps it would be worthwhile for\n"
	.string "you to investigate this rumor.$"

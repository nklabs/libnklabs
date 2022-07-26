# Just run Make in each subdirectory

tests :
	find . -mindepth 1 -maxdepth 1 -type d | xargs -n 1 make -C

cleaner :
	find . -mindepth 1 -maxdepth 1 -type d | xargs -n 1 make cleaner -C

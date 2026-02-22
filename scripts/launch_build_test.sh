echo Working Directory: $PWD

testToRunPrefix="Battle Armor and Shell Armor block critical"

echo Running Test Build.. [make -j$(nproc) -O pokeemerald-test.elf TEST=1 RELEASE=0 TESTS="$testToRunPrefix"]
make -j$(nproc) -O pokeemerald-test.elf TEST=1 RELEASE=0 TESTS="$testToRunPrefix"

exitCode=$?

# Can comment this out once bug with log disappearing is fixed
if [ $exitCode != 0 ]
then
    echo Error! [Exit code $exitCode]
    read -n 1 -s -r -p "Press any key to continue..."
else
    echo Success!
fi

exit $exitCode
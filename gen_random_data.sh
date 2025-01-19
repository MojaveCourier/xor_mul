mkdir temp
cd temp
dd if=/dev/urandom of=random_data.bin bs=64M count=2
cd ..
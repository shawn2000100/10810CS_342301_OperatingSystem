../build.linux/nachos -f
echo "================== 1 ======================="
../build.linux/nachos -cp num_1000000.txt /1000000a
../build.linux/nachos -p /1000000a
echo "================== 2 ======================="
../build.linux/nachos -cp num_1000000.txt /1000000b
../build.linux/nachos -p /1000000b
echo "================== 3 ======================="
../build.linux/nachos -cp num_1000000.txt /1000000c
../build.linux/nachos -p /1000000c
echo "================== 4 ======================="
../build.linux/nachos -cp num_1000000.txt /1000000d
../build.linux/nachos -p /1000000d
echo "================== 5 ======================="
../build.linux/nachos -cp num_1000000.txt /1000000e
../build.linux/nachos -p /1000000e
echo "================== 6 ======================="
../build.linux/nachos -cp num_1000000.txt /1000000f
../build.linux/nachos -p /1000000f

echo "================ Last ========================="
../build.linux/nachos -l /


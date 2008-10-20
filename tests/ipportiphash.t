# Range: Try to create from an invalid range
2 ipset -N test ipportiphash --from 2.0.0.1 --to 2.1.0.1
# Range: Create a set from a valid range
0 ipset -N test ipportiphash --from 2.0.0.1 --to 2.1.0.0
# Range: Add lower boundary
0 ipset -A test 2.0.0.1,5,1.1.1.1
# Range: Add upper boundary
0 ipset -A test 2.1.0.0,128,2.2.2.2
# Range: Test lower boundary
0 ipset -T test 2.0.0.1,5,1.1.1.1
# Range: Test upper boundary
0 ipset -T test 2.1.0.0,128,2.2.2.2
# Range: Test value not added to the set
1 ipset -T test 2.0.0.1,5,1.1.1.2
# Range: Test value not added to the set
1 ipset -T test 2.0.0.1,6,1.1.1.1
# Range: Test value not added to the set
1 ipset -T test 2.0.0.2,6,1.1.1.1
# Range: Test value before lower boundary
1 ipset -T test 2.0.0.0,5,1.1.1.1
# Range: Test value after upper boundary
1 ipset -T test 2.1.0.1,128,2.2.2.2
# Range: Try to add value before lower boundary
1 ipset -A test 2.0.0.0,5,1.1.1.1
# Range: Try to add value after upper boundary
1 ipset -A test 2.1.0.1,128,2.2.2.2
# Range: Delete test test
0 ipset -X test
# Network: Try to create a set from an invalid network
2 ipset -N test ipportiphash --network 2.0.0.0/15
# Network: Create a set from a valid network
0 ipset -N test ipportiphash --network 2.0.0.0/16
# Network: Add lower boundary
0 ipset -A test 2.0.0.0,5,1.1.1.1
# Network: Add upper boundary
0 ipset -A test 2.0.255.255,128,2.2.2.2
# Network: Test lower boundary
0 ipset -T test 2.0.0.0,5,1.1.1.1
# Network: Test upper boundary
0 ipset -T test 2.0.255.255,128,2.2.2.2
# Network: Test value not added to the set
1 ipset -T test 2.0.0.0,5,1.1.1.2
# Network: Test value not added to the set
1 ipset -T test 2.0.0.0,6,1.1.1.1
# Network: Test value before lower boundary
1 ipset -T test 1.255.255.255,5,1.1.1.1
# Network: Test value after upper boundary
1 ipset -T test 2.1.0.0,128,2.2.2.2
# Network: Try to add value before lower boundary
1 ipset -A test 1.255.255.255,5,1.1.1.1
# Network: Try to add value after upper boundary
1 ipset -A test 2.1.0.0,128,2.2.2.2
# Network: Delete test set
0 ipset -X test
# eof

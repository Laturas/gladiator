import sys
import filecmp
test_number = sys.argv[1]

if filecmp.cmp("./testcases/testcase_{}_answer.s".format(test_number), "./output.s"):
    print("test case {} passed!".format(test_number))
else:
    print("test case {} FAILED".format(test_number))
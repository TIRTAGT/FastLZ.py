CFLAGS?=-Wall -std=c90
TEST_ROUNDTRIP?=./test_roundtrip

all: roundtrip

roundtrip: test_roundtrip
	$(TEST_ROUNDTRIP)

test_roundtrip: test_roundtrip.c ../vendor/fastlz.c refimpl.c
	$(CC) -o $(TEST_ROUNDTRIP)  $(CFLAGS) -I../vendor test_roundtrip.c ../vendor/fastlz.c refimpl.c

clean :
	$(RM) $(TEST_ROUNDTRIP) *.o

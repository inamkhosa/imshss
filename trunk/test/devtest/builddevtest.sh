rm -f *sh*.o *sh*.so
gcc -I. -I.. -I/usr/include/libxml2 -c -g oampReqSenderTest.c interfaceAPI.c 
gcc -g -o oampReqSenderTest oampReqSenderTest.o interfaceAPI.o  -lxml2 
gcc -I. -c -g dummyUDPServer.c
gcc -g -o dummyUDPServer dummyUDPServer.o








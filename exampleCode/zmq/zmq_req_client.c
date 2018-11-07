//  Hello World client
#include <zmq.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

int main (int argc, char** argv)
{

    char* id = (argc > 1)? argv [1]: "0010";
    size_t length = strlen(id);
    if(length != 4)
    {
    	printf("size of ID must be 4 digits\n");
    	return -1;
    }
    printf("length is: %u\n", length);

    printf ("Connecting to hello world server…\n");
    void *context = zmq_ctx_new ();
    void *requester = zmq_socket (context, ZMQ_REQ);	/* Create socket */
    zmq_connect (requester, "tcp://localhost:5555");	/* Connect to server on same host */

    int request_nbr;
    for (request_nbr = 0; request_nbr != 10; request_nbr++) {
        char buffer [10];
        printf ("%s : Sending Hello %d…\n", id, request_nbr);

        char message[] = "     : hello\0";
        strncpy(message, id, 4);
        zmq_send (requester, message, 13, 0);

//        int rc = zmq_send(requester, id, 4, ZMQ_SNDMORE);
//        assert(rc == 4);
//		  rc = zmq_send(requester, " : hello\0", 9, 0);
//		  assert(rc == 8);
        zmq_recv (requester, buffer, 10, 0);			/* Receive message of 10 bytes */
        printf ("Received: %s\n", buffer);
        sleep(1);
    }
    zmq_close (requester);								/* Close socket */
    zmq_ctx_destroy (context);
    return 0;
}

//  Hello World server

#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

int main (void)
{
	//char *strncpy(char *dest, const char *src, size_t n);

    //  Socket to talk to clients
    void *context = zmq_ctx_new ();
    void *responder = zmq_socket (context, ZMQ_REP);	/* Create socket */

    int rc = zmq_bind (responder, "tcp://*:5555");		/* Bind socket on all interfaces to port 5555 */
    assert (rc == 0);
    int timeout = 4000;
    zmq_setsockopt(responder, ZMQ_RCVTIMEO, &timeout, sizeof(int) );
    printf("starting main...\n");
    char buffer [13];
    char id [5];
    while (1) {											/* Loop forever */
        int rc = zmq_recv (responder, buffer, 13, 0);
        if(rc < 0)
        	break;			/* Receive 10 byte message */

        strncpy(id, buffer, 4);
        id[4] = '\0';
        printf ("Received: %s\n", buffer );
        sleep (1);          //  Do some 'work'
        zmq_send (responder, id, 5, 0);			/* Send 5 byte response */
    }
    zmq_close(responder);
    printf("finished.\n");
    return 0;
}

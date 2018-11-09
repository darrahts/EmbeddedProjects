

//  Weather update server
//  Binds PUB socket to tcp://*:5556
//  Publishes random weather updates

#include "zhelpers.h"

int main (void)
{
    //  Prepare our context and publisher
    void *context = zmq_ctx_new ();
    void *publisher = zmq_socket (context, ZMQ_PUB);
    int rc = zmq_bind (publisher, "tcp://*:5556");
    assert (rc == 0);

    //  Initialize random number generator
    srandom ((unsigned) time (NULL));
    long seq_num = 0;
    while (1) {
        //  Get values that will fool the boss
        int zipcode, temperature, relhumidity;
        zipcode     = (rand() % (50000 - 10000 + 1)) + 10000;
        temperature = randof (215) - 80;
        relhumidity = randof (50) + 10;

        //  Send message to all subscribers
        char update [30];
        sprintf (update, "%05d %d %d %ld", zipcode, temperature, relhumidity, seq_num);
        s_send (publisher, update);
        if(zipcode == 10001)
        {
        	printf("%s\n", update);
        }
        seq_num++;
    }
    zmq_close (publisher);
    zmq_ctx_destroy (context);
    return 0;
}

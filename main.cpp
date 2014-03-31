#include <string>
#include <iostream>
#include <pthread.h>
#include <zhelpers.hpp>
#include <boost/format.hpp>

using namespace std;

void * client_task(void *args) {
    zmq::context_t context(1);
    zmq::socket_t subscriber(context, ZMQ_PULL);
    subscriber.connect("tcp://127.0.0.1:1212");
    int msg_number = 0;
    while (true) {
        std::string message = s_recv(subscriber);
        msg_number++;
        if ( message.compare("Done") == 0 ) {
            break;
        }
    }
    std::cout << boost::format("We've received %d messages\n") % msg_number;
    return (NULL);
}

int main()
{
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_PUSH);
    socket.bind("tcp://127.0.0.1:1212");
    s_catch_signals();
    pthread_t client;
    pthread_create(&client, NULL, client_task, NULL);
    int msg_number = 0;
    while (!s_interrupted) {
        int rc = s_send(socket, str(boost::format("Sending message %d to some random client") % msg_number));
        msg_number++;
        if ( rc == EINTR ) break;
    }
    s_send(socket, "Done");
    std::cout << boost::format("We've sent %d messages\n") % ++msg_number;
    pthread_join(client, NULL);
    return 0;
}

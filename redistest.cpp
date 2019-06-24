#include <iostream>
#include <string>
#include <hiredis/hiredis.h>
using namespace std;

int main(int argc, char **argv)
{
    redisContext *c;
    redisReply *reply;
    string ip((argc > 1) ? argv[1] : "127.0.0.1");
    int port = (argc > 2) ? atoi(argv[2]) : 6379;

    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 500000;
    c = redisConnectWithTimeout(ip.c_str(), port, timeout);
    if (c == NULL || c->err)
    {
        if (c)
        {
            cout << "Connection error:" << c->errstr << endl;
            redisFree(c);
        }
        else
        {
            cout << "Connection error: can't allocate redis context" << endl;
        }
        exit(1);
    }

    reply = static_cast<redisReply *>(redisCommand(c, "PING"));
    cout << "PING: " << reply->str << endl;
    freeReplyObject(reply);

    reply = static_cast<redisReply *>(redisCommand(c, "SET %s %s", "foo", "hello world"));
    cout << "SET: " << reply->str << endl;
    freeReplyObject(reply);

    reply = static_cast<redisReply *>(redisCommand(c, "SET %b %b", "bar", 3, "hello", 5));
    cout << "SET (binary API): " << reply->str << endl;
    freeReplyObject(reply);

    reply = static_cast<redisReply *>(redisCommand(c, "INCR counter"));
    cout << "INCR counter: " << reply->integer << endl;
    freeReplyObject(reply);

    reply = static_cast<redisReply *>(redisCommand(c, "INCR counter"));
    cout << "INCR counter: " << reply->integer << endl;
    freeReplyObject(reply);

    reply = static_cast<redisReply *>(redisCommand(c, "DEL mylist"));
    freeReplyObject(reply);
    for (int j = 0; j < 10; ++j)
    {
        reply = static_cast<redisReply *>(redisCommand(c, "LPUSH mylist element-%s", to_string(j).c_str()));
        freeReplyObject(reply);
    }

    reply = static_cast<redisReply *>(redisCommand(c, "LRANGE mylist 0 -1"));
    if (reply->type == REDIS_REPLY_ARRAY)
    {
        for (int j = 0; j < reply->elements; ++j)
        {
            cout << j << ") " << reply->element[j]->str << endl;
        }
    }
    freeReplyObject(reply);

    redisFree(c);
    return 0;
}
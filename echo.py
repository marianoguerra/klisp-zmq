import zmq, json

def main():
    context = zmq.Context()
    subscriber = context.socket(zmq.REP)
    subscriber.bind("tcp://127.0.0.1:5555")

    while True:
        print("waiting for data")
        data = subscriber.recv()
        print("got data")
        print(data)
        print("sending it back")
        subscriber.send(data)

if __name__ == "__main__":
    main()


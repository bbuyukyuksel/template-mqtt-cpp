#ifndef MY_MQTT_LIBRARY
#define MY_MQTT_LIBRARY

// mosquitto
// Additional Inc Dir: C:\Program Files\mosquitto\devel;
// Additional Lib Dir: C:\Program Files\mosquitto\devel;

#include <iostream>
#include <mosquitto.h>
#include <string>
#include <thread>

class MosquittoPublisher {
	mosquitto* m_mosq;
	std::string m_id;
	std::string m_server;
	size_t m_port;
	size_t m_keep_alive_secs;
public:
	MosquittoPublisher() = delete;
	MosquittoPublisher(const char * id, const char * server = "localhost", size_t port = 1883, size_t keep_alive_secs = 60) :
		m_mosq(mosquitto_new(id, true, NULL)),
		m_id(id), m_server(server), m_port(port), m_keep_alive_secs(keep_alive_secs)
	{
		mosquitto_lib_init();
		int rc = mosquitto_connect(m_mosq, m_server.c_str(), m_port, m_keep_alive_secs);
		if (rc != 0) {
			printf("Client could not connect to broker! Error Code: %d\n", rc);
			mosquitto_destroy(m_mosq);
			exit(EXIT_FAILURE);
		}
		printf("Connected to the broker!\n");
	}

	void Publish(std::string topic, std::string payload)
	{
		mosquitto_publish(m_mosq, NULL, topic.c_str(), payload.length(), payload.c_str(), 0, false);
	}

	void Close() {
		mosquitto_disconnect(m_mosq);
		free(m_mosq);
		m_mosq = nullptr;
		mosquitto_lib_cleanup();
	}

	void Info()
	{
		std::cout << "ID:" << m_id << std::endl;
		std::cout << "Server:" << m_server << std::endl;
		std::cout << "Port:" << m_port << std::endl;
		std::cout << "Keep Alive (secs):" << m_keep_alive_secs << std::endl;
	}

	static void TestCode() {
		auto mypublisher = MosquittoPublisher("test-user");
		mypublisher.Publish("test/t1", "Hello World!");
		mypublisher.Info();
	}
};

class MosquittoSubscriber {
	using cb_on_connect = void(*)(mosquitto*, void*, int);
	using cb_on_message = void(*)(mosquitto *, void*, const mosquitto_message*);

	mosquitto* m_mosq;
	std::string m_id;
	int m_id_int;
	std::string m_server;
	size_t m_port;
	size_t m_keep_alive_secs;

	cb_on_connect m_callback_on_connect;
	cb_on_message m_callback_on_message;

public:
	MosquittoSubscriber() = delete;
	MosquittoSubscriber(const char * id, int id_int, cb_on_connect callback_on_connect, cb_on_message callback_on_message, const char * server = "localhost", size_t port = 1883, size_t keep_alive_secs = 0) :
		m_id_int(id_int),
		m_mosq(mosquitto_new(id, true, &m_id_int)),
		m_id(id), m_server(server), m_port(port), m_keep_alive_secs(keep_alive_secs),
		m_callback_on_connect(callback_on_connect),
		m_callback_on_message(callback_on_message)
	{
		mosquitto_lib_init();

		mosquitto_connect_callback_set(m_mosq, m_callback_on_connect);
		mosquitto_message_callback_set(m_mosq, m_callback_on_message);

		int rc = mosquitto_connect(m_mosq, m_server.c_str(), m_port, m_keep_alive_secs);
		if (rc) {
			printf("Could not connect to Broker with return code %d\n", rc);
			exit( EXIT_FAILURE );
		}
		/*mosquitto_loop_forever(m_mosq, 0, 100);
		mosquitto_disconnect(m_mosq);
		mosquitto_destroy(m_mosq);
		mosquitto_lib_cleanup();*/
	}

	void Start(){
		auto th = std::thread(mosquitto_loop_forever, m_mosq, 0, 100);
		th.detach();
	}
};
#endif
#include <iostream>
#include "mqtt.hpp"
#include <thread>
#include <chrono>

void on_connect(mosquitto *mosq, void *obj, int rc) {
	printf("ID: %d\n", *(int *)obj);
	if (rc) {
		printf("Error with result code: %d\n", rc);
		exit(-1);
	}
	mosquitto_subscribe(mosq, NULL, "sensor/#", 0);

	// wildcards
	// sensor/+/temperature -> sensor/1/temperature, sensor/2/temperature
	// sensor/#				-> sensor/1/temperature, sensor/2/temperature
}

void on_message(mosquitto *mosq, void *obj, const mosquitto_message *msg) {
	printf("New message with topic %s: %s\n", msg->topic, (char *)msg->payload);
}

int main() {
	MosquittoSubscriber sub("subscriber", 12, on_connect, on_message);
	sub.Start();
	
	std::this_thread::sleep_for(std::chrono::seconds::duration(3));

	MosquittoPublisher publisher("publisher");

	for (int j = 0; j < 2; ++j)
	{
		std::string topic = "sensor/";
		topic += std::to_string(j);

		for (int i = 0; i < 10; ++i) {
			publisher.Publish(topic, "hello world");
			std::this_thread::sleep_for(std::chrono::milliseconds::duration(500));
		}

	}

	system("pause");
	return 0;
}
#include <dds/dds.h>

int main() {
  // Create a DomainParticipant
  DDS::DomainParticipant participant(0);

  // Create a Topic
  DDS::Topic topic(participant, "my_topic");

  // Create a Publisher
  DDS::Publisher publisher(participant);

  // Create a DataWriter
  DDS::DataWriter datawriter(publisher, topic);

  // Create a Sample
  MyType sample;
  sample.x = 10;
  sample.y = 20;

  // Write the Sample
  datawriter.write(sample, DDS::HANDLE_NIL);

  // Create a Subscriber
  DDS::Subscriber subscriber(participant);

  // Create a DataReader
  DDS::DataReader datareader(subscriber, topic);

  // Wait for Data
  while (true) {
    DDS::SampleInfoSeq infoseq;
    DDS::DataSeq dataseq;

    datareader.read(dataseq, infoseq, DDS::LENGTH_UNLIMITED, DDS::ANY_SAMPLE_STATE, DDS::ANY_VIEW_STATE, DDS::ANY_INSTANCE_STATE);

    for (int i = 0; i < dataseq.length(); i++) {
      MyType sample = dataseq[i];

      // Do something with the sample
      printf("x = %d, y = %d\n", sample.x, sample.y);
    }
  }

  return 0;
}
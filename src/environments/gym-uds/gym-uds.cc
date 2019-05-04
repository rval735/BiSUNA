#include <algorithm>
#include <cassert>
#include <string>

#include "gym-uds.h"


namespace gym_uds
{
EnvironmentClient::EnvironmentClient(const std::string& sock_filepath) :
    stub{Environment::NewStub(grpc::CreateChannel(sock_filepath, grpc::InsecureChannelCredentials()))}
{}


observation_t EnvironmentClient::reset()
{
    grpc::ClientContext context;
    Empty empty_request;
    State state_reply;

    grpc::Status status = stub->Reset(&context, empty_request, &state_reply);
    if (status.ok() == false) {
        printf("Error: %s\n", status.error_message().c_str());
    }
    assert(status.ok());

    observation_t observation;
    std::copy_n(state_reply.observation().data().cbegin(), state_reply.observation().data().size(), std::back_inserter(observation));
    envID = state_reply.envid();
    return observation;
}

state_t EnvironmentClient::step(const action_t &action_value)
{
    grpc::ClientContext context;
    Action action_request;
    
    int size = static_cast<int>(action_value.size());
    for(int i = 0; i < size; i++ ) {
        action_request.add_data(action_value[i]);
    }
    
    State state_reply;

    grpc::Status status = stub->Step(&context, action_request, &state_reply);
    if (status.ok() == false) {
        printf("Error: %s\n", status.error_message().c_str());
    }
    assert(status.ok());

    observation_t observation;
    std::copy_n(state_reply.observation().data().cbegin(), state_reply.observation().data().size(), std::back_inserter(observation));
    return {observation, state_reply.reward(), state_reply.done()};
}


action_t EnvironmentClient::sample()
{
    grpc::ClientContext context;
    Empty empty_request;
    Action action_reply;

    grpc::Status status = stub->Sample(&context, empty_request, &action_reply);
    if (status.ok() == false) {
        printf("Error: %s\n", status.error_message().c_str());
    }
    assert(status.ok());
    
    int size = action_reply.data_size();
    action_t resp(size);
    for (int i = 0; i < size; i++) {
        resp[i] = action_reply.data(i);
    }
    
    return resp;
}

std::string EnvironmentClient::getEnvID() {
    return envID;
}
}

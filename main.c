#include <Windows.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#pragma pack(push, 8)
#include "discord_game_sdk.h"
#pragma pack(pop)
#include <time.h>

#define DISCORD_REQUIRE(x) assert(x == DiscordResult_Ok)

struct Application {
    struct IDiscordCore* core;
    struct IDiscordUserManager* users;
    struct IDiscordAchievementManager* achievements;
    struct IDiscordActivityManager* activities;
    struct IDiscordRelationshipManager* relationships;
    struct IDiscordApplicationManager* application;
    struct IDiscordLobbyManager* lobbies;
    DiscordUserId user_id;
};

void OnOAuth2Token(void* data, enum EDiscordResult result, struct DiscordOAuth2Token* token)
{
    if (result == DiscordResult_Ok) {
        printf("OAuth2 token: %s\n", token->access_token);
    }
    else {
        printf("GetOAuth2Token failed with %d\n", (int)result);
    }
}

int main(int argc, char** argv) {
    struct Application app;
    memset(&app, 0, sizeof(app));
    
    struct IDiscordUserEvents users_events;
    memset(&users_events, 0, sizeof(users_events));
    
    struct IDiscordActivityEvents activities_events;
    memset(&activities_events, 0, sizeof(activities_events));
    
    struct IDiscordRelationshipEvents relationships_events;
    memset(&relationships_events, 0, sizeof(relationships_events));
    
    struct DiscordCreateParams params;
    DiscordCreateParamsSetDefault(&params);
    params.client_id = CLIENT_ID;
    params.flags = DiscordCreateFlags_Default;
    params.event_data = &app;
    params.activity_events = &activities_events;
    params.user_events = &users_events;
    DISCORD_REQUIRE(DiscordCreate(DISCORD_VERSION, &params, &app.core));
    
    app.users = app.core->get_user_manager(app.core);
    app.activities = app.core->get_activity_manager(app.core);
    app.application = app.core->get_application_manager(app.core);
    
    app.application->get_oauth2_token(app.application, &app, OnOAuth2Token);
    
    DiscordBranch branch;
    app.application->get_current_branch(app.application, &branch);
    printf("Current branch %s\n", branch);
    
    struct DiscordActivity actData;
    memset(&actData, 0, sizeof(actData));
    
    if (strcmp(argv[1], "C") == 0) {
        memcpy(actData.assets.large_text , "Editing a C file", sizeof("Editing a C file"));
        memcpy(actData.assets.large_image, "clogo", sizeof("clogo"));
    };
    if (strcmp(argv[1], "Pascal") == 0) {
        memcpy(actData.assets.large_text, "Editing a Pascal file", sizeof("Editing a Pascal file"));
        memcpy(actData.assets.large_image, "pascal", sizeof("pascal"));
    };
    
    memcpy(actData.details, argv[2], strlen(argv[2]));
    memcpy(actData.state, argv[3], strlen(argv[3]));
    memcpy(actData.assets.small_text, "4coder", sizeof("4coder"));
    memcpy(actData.assets.small_image, "4cdr2", sizeof("4cdr2"));
    
    actData.timestamps.start = (unsigned) time(NULL);
    
    actData.instance = 1;
    
    app.activities->update_activity(app.activities, &actData, 0, 0);
    
    
    for (;;) {
        DISCORD_REQUIRE(app.core->run_callbacks(app.core));
        Sleep(16);
    };
    return 1;
};
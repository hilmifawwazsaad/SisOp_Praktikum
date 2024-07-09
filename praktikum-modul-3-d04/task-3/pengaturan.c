#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

#define MAX_LENGTH 100
#define MAX_CONTAINERS 100

void createContainer(const char *containerName) {
    char command[256];
    snprintf(command, sizeof(command), "docker run -d --name %s dockermessage:v2", containerName);
    int status = system(command);
    if (status == -1) {
        perror("Error creating container");
    }
}

void deleteContainer(const char *containerName) {
    char command[256];
    snprintf(command, sizeof(command), "docker rm -f %s", containerName);
    int status = system(command);
    if (status == -1) {
        perror("container doesn't exist\n");
    }
}

void listContainers() { 
    system("docker ps -a");
}

void connectContainers(const char *container1, const char *container2, const char *sender1, const char *sender2) {
    deleteContainer(container1);
    deleteContainer(container2);

    char receiver1[MAX_LENGTH];
    char receiver2[MAX_LENGTH];

    strcpy(receiver1, container1);
    strcpy(receiver2, container2); 

    char command[512];
    snprintf(command, sizeof(command), "docker run -dit --name=%s --env SENDER=%s --env RECEIVER=%s -v sharedvolume:/APP dockermessage:v2", container1, sender2, receiver2);
    int status = system(command);
    if (status == -1) {
        perror("Error creating container 1");
    }

    snprintf(command, sizeof(command), "docker run -dit --name=%s --env SENDER=%s --env RECEIVER=%s -v sharedvolume:/APP dockermessage:v2", container2, sender1, receiver1);
    status = system(command);
    if (status == -1) {
        perror("Error creating container 2");
    }
}

void unconnectContainers(const char *container1, const char *container2) {
    int status;

    // Stop container 1
    char command[512];
    snprintf(command, sizeof(command), "docker stop %s", container1);
    status = system(command);
    if (status == -1) {
        perror("Error stopping container 1");
    } else if (status != 0) {
        fprintf(stderr, "Failed to stop container 1: %s\n", container1);
    }

    // Stop container 2
    snprintf(command, sizeof(command), "docker stop %s", container2);
    status = system(command);
    if (status == -1) {
        perror("Error stopping container 2");
    } else if (status != 0) {
        fprintf(stderr, "Failed to stop container 2: %s\n", container2);
    }

}

void loginContainer(const char *containerName) {

    char command[512];

    snprintf(command, sizeof(command), "docker exec -it %s /bin/bash", containerName);
    int loginStatus = system(command);
    if (loginStatus == -1) {
        perror("Error logging into container");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <create/delete/list/login/connect/unconnect> [container_name1] [container_name2]\n", argv[0]);
        return 1;
    }

    char sender[MAX_LENGTH];
    char twopersononchat[MAX_CONTAINERS][MAX_LENGTH];
    char thetwoperseon[MAX_CONTAINERS][MAX_LENGTH];
    int container_count = 0;

    FILE *docker_inspect = popen("docker inspect --format='{{.Name}}' $(sudo docker ps -aq --no-trunc)", "r");
    if (docker_inspect == NULL) {
        printf("Error running docker inspect.\n");
        return 1;
    }

    while (fgets(sender, MAX_LENGTH, docker_inspect) != NULL && container_count < MAX_CONTAINERS) {
        sender[strcspn(sender, "\n")] = 0; // remove trailing newline
        if (sender[0] == '/') {
            memmove(sender, sender + 1, strlen(sender));
        }
        strcpy(twopersononchat[container_count], sender);
        container_count++;
    }

    pclose(docker_inspect);

    if (strcmp(argv[1], "create") == 0) {
        if (argc != 3) {
            printf("Usage: %s create <container_name>\n", argv[0]);
            return 1;
        }
        createContainer(argv[2]);
    } 
    else if (strcmp(argv[1], "delete") == 0) {
        if (argc != 3) {
            printf("Usage: %s delete <container_name>\n", argv[0]);
            return 1;
        }
        deleteContainer(argv[2]);
    } 
    else if (strcmp(argv[1], "list") == 0) {
        listContainers();
    }
    else if (strcmp(argv[1], "login") == 0) {
        if (argc != 3) {
            printf("Usage: %s login <container_name>\n", argv[0]);
            return 1;
        }

        loginContainer(argv[2]);
    }
    else if (strcmp(argv[1], "connect") == 0) {
        if (argc != 4) {
            printf("Usage: %s connect <container_name1> <container_name2>\n", argv[0]);
            return 1;
        }

        int count = 0;
        for (int i = 0; i < container_count; i++) {
        if(strcmp(twopersononchat[i], argv[2]) == 0 || strcmp(twopersononchat[i], argv[3]) == 0){
            strcpy(thetwoperseon[count], twopersononchat[i]);
            count++;
        }
    }

    for (int i = 0; i < 2; i++) {
        if(strcmp(twopersononchat[i], argv[2]) != 0){

            strcpy(thetwoperseon[0], twopersononchat[i]);
        }
        }

        for (int i = 0; i < 2; i++) {
        if(strcmp(twopersononchat[i], argv[3]) != 0){

            strcpy(thetwoperseon[1], twopersononchat[i]);
        }
        }

        connectContainers(argv[2], argv[3], thetwoperseon[0], thetwoperseon[1]);
    }
    else if (strcmp(argv[1], "unconnect") == 0) {
        if (argc != 4) {
            printf("Usage: %s unconnect <container_name1> <container_name2>\n", argv[0]);
            return 1;
        }

        memset(twopersononchat, 0, sizeof(twopersononchat));
        unconnectContainers(argv[2], argv[3]);
    }
    else {
        printf("Invalid command\n");
        return 1;
    }

    return 0;
}

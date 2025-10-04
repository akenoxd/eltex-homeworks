#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAX_DRIVERS 100
#define MAX_BUFFER 256
#define PIPE_READ 0
#define PIPE_WRITE 1

typedef enum { AVAILABLE, BUSY } DriverStatus;

typedef struct {
  pid_t pid;
  DriverStatus status;
  int task_timer;
  time_t task_end_time;
  int to_driver[2];
  int from_driver[2];
} DriverInfo;

DriverInfo drivers[MAX_DRIVERS];
int driver_count = 0;

DriverInfo *find_driver_by_pid(pid_t pid) {
  for (int i = 0; i < driver_count; i++)
    if (drivers[i].pid == pid)
      return &drivers[i];

  return NULL;
}

int send_command_to_driver(DriverInfo *driver, const char *command,
                           char *response, size_t response_size) {
  if (!driver || !command) {
    printf("Error: Invalid parameters\n");
    return -1;
  }

  ssize_t bytes_written =
      write(driver->to_driver[PIPE_WRITE], command, strlen(command));
  if (bytes_written == -1) {
    perror("Error writing to driver");
    return -1;
  }

  struct pollfd fds[1];
  fds[0].fd = driver->from_driver[PIPE_READ];
  fds[0].events = POLLIN;

  int poll_result = poll(fds, 1, 1000);
  if (poll_result == -1) {
    perror("Error polling driver response");
    return -1;
  } else if (poll_result == 0) {
    printf("Error: Timeout waiting for response from driver %d\n", driver->pid);
    return -1;
  }

  ssize_t bytes_read =
      read(driver->from_driver[PIPE_READ], response, response_size - 1);
  if (bytes_read == -1) {
    perror("Error reading driver response");
    return -1;
  } else if (bytes_read == 0) {
    printf("Error: No response from driver %d\n", driver->pid);
    return -1;
  }

  response[bytes_read] = '\0';
  return 0;
}

void driver_process(int to_driver_read, int from_driver_write) {
  DriverStatus status = AVAILABLE;
  int task_timer = 0;
  time_t task_end_time = 0;
  char buffer[MAX_BUFFER];

  struct pollfd fds[1];
  fds[0].fd = to_driver_read;
  fds[0].events = POLLIN;

  while (1) {
    int timeout = -1;

    if (status == BUSY) {
      time_t current_time = time(NULL);
      if (current_time >= task_end_time) {
        status = AVAILABLE;
        task_timer = 0;
      } else {
        timeout = (task_end_time - current_time) * 1000;
      }
    }

    int poll_result = poll(fds, 1, timeout);

    if (poll_result > 0) {
      if (fds[0].revents & POLLIN) {
        ssize_t bytes_read = read(to_driver_read, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
          buffer[bytes_read] = '\0';

          if (strncmp(buffer, "SEND_TASK", 9) == 0) {
            int timer;
            if (sscanf(buffer, "SEND_TASK %d", &timer) == 1) {
              if (status == AVAILABLE) {
                status = BUSY;
                task_timer = timer;
                task_end_time = time(NULL) + timer;

                char response[MAX_BUFFER];
                snprintf(response, sizeof(response), "OK\n");
                write(from_driver_write, response, strlen(response));
              } else {
                char response[MAX_BUFFER];
                snprintf(response, sizeof(response), "BUSY %d\n", task_timer);
                write(from_driver_write, response, strlen(response));
              }
            }
          } else if (strncmp(buffer, "GET_STATUS", 10) == 0) {
            char response[MAX_BUFFER];
            if (status == AVAILABLE) {
              snprintf(response, sizeof(response), "AVAILABLE\n");
            } else {
              snprintf(response, sizeof(response), "BUSY %d\n", task_timer);
            }
            write(from_driver_write, response, strlen(response));
          } else if (strncmp(buffer, "EXIT", 4) == 0) {
            break;
          }
        }
      }
    }

    if (status == BUSY && time(NULL) >= task_end_time) {
      status = AVAILABLE;
      task_timer = 0;
    }
  }

  close(to_driver_read);
  close(from_driver_write);
  exit(0);
}

int create_driver() {
  if (driver_count >= MAX_DRIVERS) {
    printf("Error: Maximum number of drivers (%d) reached\n", MAX_DRIVERS);
    return -1;
  }
  DriverInfo *driver = &drivers[driver_count];

  if (pipe(driver->to_driver) == -1) {
    perror("Error creating pipe");
    return -1;
  }

  if (pipe(driver->from_driver) == -1) {
    perror("Error creating pipe");
    close(driver->to_driver[PIPE_READ]);
    close(driver->to_driver[PIPE_WRITE]);
    return -1;
  }

  pid_t pid = fork();
  if (pid == -1) {
    perror("Error forking process");
    close(driver->to_driver[PIPE_READ]);
    close(driver->to_driver[PIPE_WRITE]);
    close(driver->from_driver[PIPE_READ]);
    close(driver->from_driver[PIPE_WRITE]);
    return -1;
  }

  if (pid == 0) {
    close(driver->to_driver[PIPE_WRITE]);
    close(driver->from_driver[PIPE_READ]);

    driver_process(driver->to_driver[PIPE_READ],
                   driver->from_driver[PIPE_WRITE]);
  } else {
    close(driver->to_driver[PIPE_READ]);
    close(driver->from_driver[PIPE_WRITE]);

    driver->pid = pid;
    driver->status = AVAILABLE;
    driver->task_timer = 0;
    driver->task_end_time = 0;

    driver_count++;
    printf("Created driver with PID: %d\n", pid);
  }

  return 0;
}

int send_task(pid_t pid, int task_timer) {
  if (task_timer <= 0) {
    printf("Error: Task timer must be positive\n");
    return -1;
  }

  DriverInfo *driver = find_driver_by_pid(pid);
  if (!driver) {
    printf("Error: Driver with PID %d not found\n", pid);
    return -1;
  }

  char command[MAX_BUFFER];
  char response[MAX_BUFFER];

  snprintf(command, sizeof(command), "SEND_TASK %d\n", task_timer);

  if (send_command_to_driver(driver, command, response, sizeof(response)) !=
      0) {
    return -1;
  }

  printf("Driver %d: %s", pid, response);
  return 0;
}

int get_status(pid_t pid) {
  DriverInfo *driver = find_driver_by_pid(pid);
  if (!driver) {
    printf("Error: Driver with PID %d not found\n", pid);
    return -1;
  }

  char response[MAX_BUFFER];
  if (send_command_to_driver(driver, "GET_STATUS\n", response,
                             sizeof(response)) != 0) {
    return -1;
  }

  printf("Driver %d: %s", pid, response);
  return 0;
}

int get_drivers() {
  if (driver_count == 0) {
    printf("No drivers available\n");
    return 0;
  }

  printf("Total drivers: %d\n", driver_count);
  for (int i = 0; i < driver_count; i++) {
    printf("Driver %d: PID %d - ", i + 1, drivers[i].pid);
    if (get_status(drivers[i].pid) != 0) {
      printf("Error: getting status for driver %d\n", drivers[i].pid);
    }
  }
  return 0;
}

int cleanup_driver(DriverInfo *driver) {
  if (!driver)
    return -1;

  int result = 0;
  char command[] = "EXIT\n";
  ssize_t bytes_written =
      write(driver->to_driver[PIPE_WRITE], command, strlen(command));
  if (bytes_written == -1) {
    perror("Error: sending EXIT command to driver");
    result = -1;
  }

  close(driver->to_driver[PIPE_WRITE]);
  close(driver->from_driver[PIPE_READ]);

  if (waitpid(driver->pid, NULL, 0) == -1) {
    perror("Error: waiting for driver process");
    result = -1;
  }

  return result;
}

int cleanup() {
  int result = 0;
  for (int i = 0; i < driver_count; i++) {
    if (cleanup_driver(&drivers[i]) != 0) {
      result = -1;
    }
  }
  return result;
}

int main() {
  char input[MAX_BUFFER];

  printf("Available commands:\n");
  printf("create_driver\n");
  printf("send_task <pid> <timer>\n");
  printf("get_status <pid>\n");
  printf("get_drivers\n");
  printf("exit\n\n");

  while (1) {
    printf("taxi> ");
    fflush(stdout);

    if (fgets(input, sizeof(input), stdin) == NULL)
      break;

    input[strcspn(input, "\n")] = '\0';

    if (strncmp(input, "create_driver", 13) == 0) {
      if (create_driver() != 0)
        printf("Error creating driver\n");

    } else if (strncmp(input, "send_task", 9) == 0) {
      pid_t pid;
      int timer;
      if (sscanf(input, "send_task %d %d", &pid, &timer) == 2) {
        if (send_task(pid, timer) != 0)
          printf("Error sending task to driver\n");

      } else {
        printf("Usage: send_task <pid> <timer>\n");
      }
    } else if (strncmp(input, "get_status", 10) == 0) {
      pid_t pid;
      if (sscanf(input, "get_status %d", &pid) == 1) {
        if (get_status(pid) != 0) {
          printf("Error getting driver status\n");
        }
      } else {
        printf("Usage: get_status <pid>\n");
      }
    } else if (strncmp(input, "get_drivers", 11) == 0) {
      if (get_drivers() != 0)
        printf("Error getting drivers list\n");

    } else if (strncmp(input, "exit", 4) == 0) {
      break;
    } else if (strlen(input) > 0) {
      printf("Unknown command: %s\n", input);
    }
  }

  if (cleanup() != 0) {
    printf("Error during cleanup\n");
    return 1;
  }
  return 0;
}
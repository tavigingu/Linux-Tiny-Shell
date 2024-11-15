# **Linux Tiny Shell**

## **Description**
**Linux Tiny Shell** is a minimalist implementation of a shell for Linux. This program allows you to execute basic Linux commands, use environment variables, redirect output to files, and chain commands using pipelines.

The main goal of the project is to demonstrate key concepts of a shell, such as command interpretation, process execution, and inter-process communication via pipes.

---

## **Features**

### 1. **Executing Simple Commands**
The shell can execute any valid Linux command, such as:
```bash
ls
pwd
echo "Hello, world!"
```

### 2. **Environment Variable Support**
You can set environment variables using the syntax:
```bash
VAR_NAME=value
```
Example:
```bash
PATH=/usr/bin
```
The shell automatically expands environment variables when used in commands:
```bash
echo $VAR_NAME
```

### 3. **Output Redirection**
Redirect the output of a command to a file using the > symbol:
```bash
echo "Hello" > output.txt
```

### 4. **Pipelines**
You can chain multiple commands together using the | symbol to pass the output of one command as input to the next
```bash
ls | grep ".c" | wc -l
```

### 5. **Exiting the Shell**
Type exit to close the shell

---

## **System Calls and Process Management**

This shell makes use of several key system calls for process management and inter-process communication:

## **fork()**
The fork() system call is used to create child processes. This is fundamental for executing commands in separate processes.

## **execvp()**
The execvp() system call replaces the current process image with a new one. It is used to execute the commands typed in the shell.

## **wait()**
The wait() system call ensures that the parent process waits for its child processes to finish before continuing execution.

## **pipe() and dup2()**
Pipes are used to connect the output of one command to the input of another. The pipe() system call creates a pipe, while dup2() is used to duplicate file descriptors to achieve redirection.

These system calls provide the necessary mechanisms for creating processes, handling input/output redirection, and implementing pipeline functionality.

---

This is an educational project that combines the knowledge of processes, pipes, and input/output manipulation in Linux. 🎉




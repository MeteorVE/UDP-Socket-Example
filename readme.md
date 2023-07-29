# Intro

這是一個使用 C 語言搭配 POSIX Library 實作的 UDP Socket 範例
內容包刮了

- 一個可以 echo 的 server (return same input)
- 一個實作了 exponential backoff 的 client

# Env

- 開發環境 : WSL + Debian 
- 如果是一個全新的 Debian 環境，需要執行以下操作確認 : 

```bash
sudo apt-get update
sudo apt-get install build-essential gdb
whereis g++
whereis gdb
```

- 如果想在 VSCode 開發可以參考這篇
  - https://code.visualstudio.com/docs/cpp/config-wsl

# Compile

- 如果執行檔並不存在，可以直接 run ``make``

```bash
make all
```

- 如果已經存在想要重新編譯，可以使用

```bash
make clean
make all
```

# Run

```bash
./server
./client 127.0.0.1 7000 any_text_args_here
```



# Reference

- 介紹 backoff 以及 jitter
  https://www.baeldung.com/resilience4j-backoff-jitter
- Socket Programming
  http://www.codingbison.com/c/c-sockets-introduction.html
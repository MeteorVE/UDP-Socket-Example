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
./server 7000
./client 127.0.0.1 7000 any_text_args_here
```

- 如果沒有指定 server 的附帶參數，則預設會 run 在 7000 port。
- 如果沒有指定 client 的附帶參數，會直接跳錯誤訊息並且跳出提示
  ``Usage: <server IP> <port> <message>``
  - 因此我們需要指定 server_ip, port, 以及想要傳送的訊息。

# Detail

### 關於 Client 端

- 在 ``client.c`` 之中，我們設定了一些參數

```c
#define MAX_RETRY 10
#define MULTIPLIER 2
#define BASE_INTERVAL_MS 500
#define MAX_WAIT_INTERVAL_MS 8000
#define DATA_BUFFER 5000
```

其中，``MAX_RETRY``, ``MULTIPLIER``, ``BASE_INTERVAL_MS``, ``MAX_WAIT_INTERVAL_MS`` 是針對 exponential backoff 的實作所使用到的參數

而 ``DATA_BUFFER`` 則是用來指定從 server 端回傳訊息時所使用的 Buffer 的大小，所以如果在 client 端指定了的傳送訊息，要注意不要太長、或是去更改此 ``DATA_BUFFER`` 的大小。

- 我們使用了 UDP 來進行連線

```c
sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
```

- 流程上，我們使用 ``sendto()`` 來進行傳送訊息、並且使用 ``timeval`` 這個 struct 來搭配 ``select()`` 使用，來達成在有限時間內監聽事件是否有無觸發。
  - 如果在時間內觸發，則以 ``recvfrom()`` 接收數據。
- 我們使用 ``retry`` 變數來紀錄重試的次數，當計數到達指定的數字之時，會跳出 while 迴圈並回傳 1 (in main function)。

### 關於 Server 端

- 我們使用 ``PORT`` 來指定預設 port，並且同 client 端，也指定了 ``DATA_BUFFER`` 為 5000。可依照需求進行更改。
- 使用了 UDP 來進行連線
- 使用了 while 迴圈來重複地進行接收以及回傳的工作。
  - 如果觸發了接收/回傳失敗，會印出對印訊息，但並不會終止程式的執行。如果想讓其終止，可於 ``ret_val`` 為 -1 的情況下加入 ``break`` 之類的語句。

# Other Note

- ``<unistd.h>`` : for ``close``, ``usleep``
- ``<math.h>`` : for ``pow``
- ``<string.h>`` : for ``strerror``
- 如果不使用 args 去指定 server ip，得在 code 內額外指定
  例如附加以下程式碼

```c
struct hostent *host; /* need netdb.h */
host = gethostbyname("127.0.0.1");
server_addr.sin_addr = *((struct in_addr *)host->h_addr);
```



# Reference

- 介紹 backoff 以及 jitter
  https://www.baeldung.com/resilience4j-backoff-jitter
- Socket Programming
  http://www.codingbison.com/c/c-sockets-introduction.html
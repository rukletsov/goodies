package main

import (
  "io"
  "log"
  "math/rand"
  "net/http"
  "time"
)

func main() {
  server := http.Server{
    Addr: ":8000",
    Handler: &requestHandler{},
    WriteTimeout: time.Duration(1) * time.Second,
//    IdleTimeout: time.Duration(1) * time.Second,
  }

  server.ListenAndServe()
}

type requestHandler struct{}

func (*requestHandler) ServeHTTP(w http.ResponseWriter, r *http.Request) {
  log.Printf("Got %s request from %s\n", r.Method, r.RemoteAddr)

  if r.URL.String() == "/subscribe" {
    subscribe(w, r)
    return
  }

  io.WriteString(w, "Unknown request: " + r.URL.String())
}

func subscribe(w http.ResponseWriter, r *http.Request) {
  w.Header().Set("Connection", "Keep-Alive")
  w.Header().Set("Transfer-Encoding", "chunked")

  cn, ok := w.(http.CloseNotifier)
  if !ok {
    log.Fatal("Expected http.ResponseWriter to be http.CloseNotifier")
  }

  flusher, ok := w.(http.Flusher)
  if !ok {
    log.Fatal("Expected http.ResponseWriter to be http.Flusher")
  }

  for {
    select {
      case _ = <-cn.CloseNotify():
        log.Printf("Subscriber %p has exited\n", &w)
        return
      default:
      }

    chunk := createChunk()
    log.Printf("Sending chunk [%v] to %p\n", len(chunk), &w)
    w.Write(chunk)
    flusher.Flush()
    log.Printf("Sent chunk [%v] to %p\n", len(chunk), &w)

    time.Sleep(100 * time.Millisecond)
  }
}

func createChunk() (chunk []byte) {
  var maxChunkSize uint64 = 1 << 20 // 1 MB
  chunkSize := rand.Uint64() % maxChunkSize
  chunk = make([]byte, chunkSize)
  rand.Read(chunk)
  return chunk
}

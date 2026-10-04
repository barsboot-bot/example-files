#!/usr/bin/env python3
# ============================================================
#  UE Stream Proxy — серверный прокси радиопотоков мода «унесённые»
#
#  Зачем:
#   1. Игроки подключаются к ОДНОму локальному адресу (прокси),
#      а не к десятку внешних станций -> стабильнее и быстрее.
#   2. Прокси держит одно восходящее соединение на станцию для
#      ВСЕХ игроков (multiplexing): внешний сервер видит 1 клиента,
#      а не N. Экономит трафик и снимает лимиты слушателей.
#   3. Кольцевой буфер сглаживает джиттер; обрыв внешнего потока
#      переподключается автоматически, игроки этого не слышат.
#   4. Единая точка白 списка станций + ACL по IP сервера DayZ.
#
#  Запуск (на том же VPS, что и сервер DayZ):
#     python3 ue_stream_proxy.py --port 8090 \
#         --stations ../Music/Radio.txt --allow 127.0.0.0/8
#
#  После запуска в Music/Radio.txt (или UE_Config libraryBaseURL)
#  станции указываются как http://<IP_сервера>:8090/<КлючСтанции>
# ============================================================
import argparse
import ipaddress
import re
import socket
import sys
import threading
import time
import urllib.parse
import urllib.request

VERSION = "1.0"
POLL = 0.5            # интервал проверки живости апстрима
RECONNECT_DELAY = 3.0 # пауза перед переподключением к апстриму
RING_MAX = 2 * 1024 * 1024  # 2 МБ кольцевого буфера на станцию


def log(msg):
    print("[UE-Proxy %s] %s" % (time.strftime("%H:%M:%S"), msg), flush=True)


class Station:
    """Одна станция: одно восходящее соединение, много нисходящих."""

    def __init__(self, key, url, name=""):
        self.key = key
        self.url = url
        self.name = name or key
        self.ring = bytearray()          # кольцевой буфер последних байт
        self.lock = threading.Lock()
        self.clients = []                # список ClientHandler
        self.alive = False
        self.bytes_served = 0
        self.errors = 0
        self._stop = threading.Event()
        self._thread = None

    def start(self):
        self._thread = threading.Thread(target=self._fetch_loop, daemon=True)
        self._thread.start()

    def _fetch_loop(self):
        """Держим ОДНО соединение с апстримом; рвётся — переподключаемся."""
        while not self._stop.is_set():
            try:
                req = urllib.request.Request(self.url, headers={
                    "User-Agent": "UnesennyeDayZProxy/" + VERSION})
                resp = urllib.request.urlopen(req, timeout=10)
                ctype = resp.headers.get("Content-Type", "")
                log("станция '%s': подключение к апстриму OK (%s)" % (self.key, ctype))
                self.alive = True
                first = True
                while not self._stop.is_set():
                    chunk = resp.read(8192)
                    if not chunk:
                        break
                    if first:
                        first = False
                        # icy metadata-блоки вырезаны быть не могут потоково —
                        # проксируем как есть: BASS на клиенте их понимает
                    with self.lock:
                        self.ring.extend(chunk)
                        tail = len(self.ring) - RING_MAX
                        if tail > 0:
                            del self.ring[:tail]
                        for c in self.clients:
                            c.feed(chunk)
                    self.bytes_served += len(chunk)
                resp.close()
            except Exception as e:
                self.errors += 1
                log("станция '%s': ошибка апстрима (%s), переподключение через %.0fс"
                    % (self.key, e, RECONNECT_DELAY))
            self.alive = False
            self._stop.wait(RECONNECT_DELAY)
        # graceful: закрываем клиентов
        for c in list(self.clients):
            c.close()

    def add_client(self, handler):
        with self.lock:
            # сразу отдаём хвост буфера, чтобы звук начался мгновенно
            tail = bytes(self.ring[-65536:])
        handler.send_header(tail)
        with self.lock:
            self.clients.append(handler)
        log("станция '%s': игрок подключился (%d активных)" % (self.key, len(self.clients)))

    def remove_client(self, handler):
        with self.lock:
            if handler in self.clients:
                self.clients.remove(handler)

    def stop(self):
        self._stop.set()


class ClientHandler(threading.Thread):
    """HTTP-ответ одному игроку (chunked, без Content-Length)."""

    def __init__(self, conn, addr, station, allow_nets):
        super().__init__(daemon=True)
        self.conn = conn
        self.addr = addr
        self.station = station
        self.allow_nets = allow_nets
        self._buf = bytearray()
        self._closed = threading.Event()

    def allowed(self):
        ip = ipaddress.ip_address(self.addr[0])
        return any(ip in n for n in self.allow_nets)

    def run(self):
        try:
            if not self.allowed():
                self.conn.sendall(b"HTTP/1.1 403 Forbidden\r\n\r\n")
                return
            data = self.conn.recv(4096)  # читаем запрос (валидируем путь)
            line = data.split(b"\r\n")[0].decode(errors="replace") if data else ""
            parts = line.split()
            if len(parts) < 2 or not re.match(r"^GET /[\w\-]+ HTTP", parts[0] + " " + parts[1] + " HTTP"):
                self.conn.sendall(b"HTTP/1.1 400 Bad Request\r\n\r\n")
                return
            key = urllib.parse.urlparse(parts[1]).path.strip("/")
            if key != self.station.key:
                self.conn.sendall(b"HTTP/1.1 404 Not Found\r\n\r\n")
                return
            self.station.add_client(self)
            while not self._closed.is_set():
                time.sleep(POLL)
                if not self.station.alive and not self.station.clients:
                    break
        except (BrokenPipeError, ConnectionResetError, OSError):
            pass
        finally:
            self.close()

    def send_header(self, prefill):
        hdr = ("HTTP/1.1 200 OK\r\n"
               "Content-Type: audio/mpeg\r\n"
               "Transfer-Encoding: chunked\r\n"
               "Ice-MetaInt: 16000\r\n"
               "X-Stream-Engine: unesennye-proxy/" + VERSION + "\r\n"
               "Connection: close\r\n\r\n").encode()
        self.conn.sendall(hdr)
        if prefill:
            self.feed(prefill)

    def feed(self, chunk):
        """Пишем chunked-фрейм; медленный клиент не должен блокировать других."""
        framed = b"%x\r\n" % len(chunk) + chunk + b"\r\n"
        self.conn.settimeout(5.0)
        try:
            self.conn.sendall(framed)
        except (socket.timeout, OSError):
            self.close()

    def close(self):
        if self._closed.is_set():
            return
        self._closed.set()
        try:
            self.conn.sendall(b"0\r\n\r\n")
        except OSError:
            pass
        try:
            self.conn.close()
        except OSError:
            pass
        self.station.remove_client(self)
        log("станция '%s': игрок отключился" % self.station.key)


def load_stations(path):
    """Формат Radio.txt:  Название = URL  (строки без '=' игнорируются)."""
    stations = {}
    try:
        with open(path, encoding="utf-8-sig") as f:
            for ln in f:
                ln = ln.strip()
                if not ln or ln.startswith("#") or "=" not in ln:
                    continue
                name, url = [p.strip() for p in ln.split("=", 1)]
                if not url.startswith(("http://", "https://")):
                    continue
                key = re.sub(r"[^A-Za-z0-9_]", "", name.replace(" ", "_").replace("+", "p"))
                if not key:
                    continue
                stations[key.lower()] = Station(key, url, name)
    except FileNotFoundError:
        log("НЕ НАЙДЕН файл станций: %s" % path)
        sys.exit(1)
    return stations


def main():
    ap = argparse.ArgumentParser(description="Серверный прокси радиопотоков «унесённые»")
    ap.add_argument("--port", type=int, default=8090)
    ap.add_argument("--host", default="0.0.0.0")
    ap.add_argument("--stations", default="../Music/Radio.txt")
    ap.add_argument("--allow", nargs="+", default=["127.0.0.0/8"],
                    help="CIDR сетей игроков (обычно интернет = открыто, но можно сузить)")
    args = ap.parse_args()

    allow_nets = [ipaddress.ip_network(a) for a in args.allow]
    stations = load_stations(args.stations)
    for st in stations.values():
        st.start()
    log("запущено %d станций: %s" % (len(stations), ", ".join(s.key for s in stations.values())))

    srv = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv.bind((args.host, args.port))
    srv.listen(64)
    log("слушаю http://%s:%d/<КлючСтанции>" % (args.host, args.port))
    try:
        while True:
            conn, addr = srv.accept()
            req_head = conn.recv(4096).split(b"\r\n")[0].decode(errors="replace")
            m = re.match(r"GET /([\w\-]+)", req_head)
            key = m.group(1).lower() if m else ""
            st = stations.get(key)
            if not st:
                conn.sendall(b"HTTP/1.1 404 Not Found\r\n\r\n")
                conn.close()
                continue
            h = ClientHandler(conn, addr, st, allow_nets)
            h.start()
    except KeyboardInterrupt:
        log("остановка...")
        for st in stations.values():
            st.stop()


if __name__ == "__main__":
    main()

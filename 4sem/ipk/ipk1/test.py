from subprocess import Popen, PIPE, STDOUT
from time import sleep
import random

IPK_SERVER_PATH = "../ipk2/ipk24chat-server"
IPK_CLIENT_PATH = "./ipk24chat-client"

TIMEOUT = 0.1


class Tester:
    def __init__(self, clients: int):
        sleep(TIMEOUT)

        print("next")
        self.port = random.randint(1000, 3000)
        self.server = Popen(
            f"{IPK_SERVER_PATH} -p {self.port}",
            shell=True, stdin=PIPE, text=True)

        sleep(TIMEOUT)

        self.clients = []
        for i in range(clients):
            self.clients.append(Popen(
                f"{IPK_CLIENT_PATH} -t tcp -s localhost -p {self.port}",
                shell=True, stdin=PIPE, stdout=PIPE, stderr=PIPE, text=True))

            sleep(TIMEOUT)
            self.write(i, f"/auth c{i} c{i} c{i}")
            sleep(TIMEOUT)

    def write(self, client: int, content: str):
        try:
            self.clients[client].stdin.write(content)
            self.clients[client].stdin.write("\n")
            self.clients[client].stdin.flush()
        except:
            print(self.clients[client].communicate())
        sleep(TIMEOUT)

    def read_all(self, client: int):
        sleep(TIMEOUT)
        output = self.clients[client].communicate(timeout=TIMEOUT)
        stdout = list(filter(lambda line: not line.startswith(
            "Server:"), output[0].splitlines()))
        stderr = list(filter(lambda line: not line.startswith(
            "Server:"), output[1].splitlines()))
        return (stdout, stderr)
        sleep(TIMEOUT)

    # we aint runnin in parallel


# test various invalid inputs
t = Tester(2)
t.write(0, "a")
t.write(1, "b")
t.write(0, "")
t.write(1, "ÁÁÁÁÁÁÁÁÁÁÁÁÁÁÁÁÁÁÁÁÁÁÁÁ")
t.write(1, "ajfslžgsjj")
t.write(1, "🖖")
t.write(0, "\x1f")
t.write(0, "\r")
t.write(1, "\t")


t.write(1, "/join 456_457")
t.write(0, "/join %546457")
t.write(0, "/join aaaaaaa\"")
t.write(1, f"/join {"a"*21}")
t.write(0, "/join ^")
t.write(1, "/join a b")
t.write(1, "/join ")
t.write(1, "/join")
t.write(0, "/join /join abc")
# t.write(0, "/join abc ") # see CHANGELOG.md

t.write(0, "/rename a b")
# t.write(0, "/rename a ") # see CHANGELOG.md
t.write(0, "/rename a\x00b")
t.write(0, "/rename a\rb")
t.write(0, "/rename a\r")

t.write(1, "a"*1401)
t.write(0, "c")
t.write(1, "d")


c0_out, c0_err = t.read_all(0)
c1_out, c1_err = t.read_all(1)
assert c0_out == ["c1: b", "c1: d"]
assert c1_out == ["c0: a", "c0: c"]

# basic functionality
t = Tester(2)
t.write(0, "aaa")
t.write(0, "bbb")
c0_out, c0_err = t.read_all(0)
c1_out, c1_err = t.read_all(1)
assert c1_out == ["c0: aaa", "c0: bbb"]
assert c1_err == ["Success: Login successful"]

# can we send messages back and forth?
t = Tester(2)
t.write(0, "a")
t.write(1, "b")
t.write(0, "c")
t.write(1, "d")
t.write(0, "e")
t.write(1, "f")
c0_out, c0_err = t.read_all(0)
c1_out, c1_err = t.read_all(1)
assert c0_out == ["c1: b", "c1: d", "c1: f"]
assert c1_out == ["c0: a", "c0: c", "c0: e"]

# join new channel and send messages there
t = Tester(2)
t.write(0, "a")
t.write(1, "b")
t.write(0, "/join new")
t.write(0, "silent")
t.write(1, "silent")
t.write(1, "/join new")
t.write(0, "c")
t.write(1, "d")
c0_out, c0_err = t.read_all(0)
c1_out, c1_err = t.read_all(1)
assert c0_out == ["c1: b", "c1: d"]
assert c1_out == ["c0: a", "c0: c"]

# join new channel and return back
t = Tester(2)
t.write(0, "a")
t.write(1, "b")
t.write(0, "/join new")
t.write(0, "silent")
t.write(0, "/join general")  # default channel in my server
t.write(0, "c")
t.write(1, "d")
c0_out, c0_err = t.read_all(0)
c1_out, c1_err = t.read_all(1)
assert c0_out == ["c1: b", "c1: d"]
assert c1_out == ["c0: a", "c0: c"]

# rename command
t = Tester(2)
t.write(0, "a")
t.write(1, "b")
t.write(0, "/rename left")
t.write(1, "/rename right")
t.write(0, "c")
t.write(1, "d")
c0_out, c0_err = t.read_all(0)
c1_out, c1_err = t.read_all(1)
assert c0_out == ["c1: b", "right: d"]
assert c1_out == ["c0: a", "left: c"]

sleep(TIMEOUT)
Popen("pkill -f ipk24chat-server", shell=True)

print("All tests passed")

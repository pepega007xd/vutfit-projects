from subprocess import run

true = True
false = False

def test(name, number, query, result, s=false):
    stdin = f"{name}\n{number}".encode()
    if s:
        stdout = run(["./t9search", "-s", query], input=stdin, capture_output=true).stdout.decode()
    else:
        stdout = run(["./t9search", query], input=stdin, capture_output=true).stdout.decode()

    expected = f"{name}, {number}\n"

    if result and (stdout == expected):
        print("ok")
    elif (not result) and (stdout == "Not found\n"):
        print("ok")
    else:
        print("error")
        print(f"inputs: {name}\n{number}\nquery: -s={s} {query}\nexpected: {expected}\ngot: {stdout}")
        exit()


test("z", "68418424418", "", true)
test("z", "68418424418", "2441", true)
test("Z", "123456789", "0", false)
test("z", "123456789123456789", "123456789123456789", true)
test("!@#$%^&*(){-/}:\"|;'\\,./", "!@#$%^&*(){}:\"|;'\\,./", "0", false)
test("!@#$%^&*(){+}:\"|;'\\,./", "!@#$%^&*(){}:\"|;'\\,./", "0", true)
test("adGkOqtwz", "0", "234567899", true)
test("adgkOQtZ", "0", "234567899", false)
test("abC"*33+"d", "789", "2223", true)
test("abc"*33+"d", "789", "2233", false)
test("+bc"*33+"d", "789", "0223", true)
test("Petr Dvorak", "603123456", "12", true)
test("Petr Dvorak", "603123456", "686", false)
test("Petr Dvorak", "603123456", "38", true)
test("Petr Dvorak", "603123456", "111", false)
test("Jana Novotna", "777987654", "12", false)
test("Jana Novotna", "777987654", "686", true)
test("Jana Novotna", "777987654", "38", false)
test("Jana Novotna", "777987654", "111", false)
test("Bedrich Smetana ml.", "541141120", "12", true)
test("Bedrich Smetana ml.", "541141120", "686", false)
test("Bedrich Smetana ml.", "541141120", "38", true)
test("Bedrich Smetana ml.", "541141120", "111", false)
#test("", "", "", )
#test("", "", "", )
#test("", "", "", )
#test("", "", "", )

test("z", "646431681168", "631118", true, s=true)
test("z", "646431681168", "630118", false, s=true)
test("z", "4066142008644304400", "4061420864430400", true, s=true)
test("xyz"+"abc"*32+"d", "456", "9992222223", true, s=true)
test("xyz"+"abc"*32+"d", "456", "9992223223", false, s=true)
test("JAUZVAFTNEVIMSEDIMTUUTOHOCELYODPOLEDNEAUZMIZTOHOJEBE", "45278269126824729", "22222", true, s=true)
test("SUS"+"AbC"*32+"J", "123"*33+"g", "7872222222225", true, s=true)
test("SUS"+"AbC"*32+"J", "0"*100, "7872222422225", false, s=true)
#test("", "", "", , s=true)
#test("", "", "", , s=true)
#test("", "", "", , s=true)
#test("", "", "", , s=true)
#test("", "", "", , s=true)


print("all tests done")

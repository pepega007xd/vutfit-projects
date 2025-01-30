TARGET := app.exe

app: main.c
	x86_64-w64-mingw32-gcc $< -o $(TARGET) -mwindows

run: app
	wine $(TARGET)

clean:
	rm $(TARGET)

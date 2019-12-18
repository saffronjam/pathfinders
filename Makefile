b: MainWin.cpp
	g++ MainWin.cpp include/Application.cpp include/AStar.cpp include/Camera.cpp include/CameraController.cpp include/Graphics.cpp include/Grid.cpp include/Funclib.cpp include/InputUtility.cpp include/Node.cpp include/UI.cpp include/Voronoi.cpp -o main.out -L/usr/lib/x86_64-linux-gnu -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network -Wall -g -pthread

r: MainWin.cpp
	./main.out

br: MainWin.cpp
	make b && make r
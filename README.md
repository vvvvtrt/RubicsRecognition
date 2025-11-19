# RubicsRecognition

В данном проекте реализовано распознование кубика рубика с помощью классических методов `cv` и кластеризации `kmeans`



# Запуск
Нужно установить библиотеки `opencv` и `opengl` через пакетный менеджер `apt` или в моем случае `brew`
```bash
  brew install glfw
  brew install opencv
```
Билд + запуск программы
```bash
  git clone https://github.com/vvvvtrt/RubicsRecognition.git
  cd RubicsRecognition
  ./build.sh
```

# Pipeline выполнения программы
После запуска программы, Вам нужно поднести кубик к камере. Когда Вы поднесете кубик, программа выделит цветами и текстом сам кубик, если программа задаст верно грань, то в консоле сохраните этот снимок через `y` или перснимите через `n`

![распознование](https://github.com/vvvvtrt/RubicsRecognition/blob/main/photo_works/cube.jpg?raw=true)

![распознование](https://github.com/vvvvtrt/RubicsRecognition/blob/main/photo_works/cube2.jpg?raw=true)

После сканирования всех граней, будет построенна 3д модель кубика

# TreasureHunter3D

Bu proje, Bilgisayar Grafikleri dersi için hazırlanmış OpenGL tabanlı 3B hazine toplama oyunu çalışmasıdır.

## Proje Konusu

Oyuncu, 3B bir labirent içerisinde hareket ederek sahneye yerleştirilmiş hazineleri toplamaya çalışır. Oyuncu duvarlara çarptığında hareketi engellenir. Hazineye temas ettiğinde hazine toplanır ve skor artar.

## Kullanılan Teknolojiler

- C++
- OpenGL
- GLFW
- GLAD
- GLM
- GLSL Shader
- CMake

## Temel Özellikler

- 3B sahne yapısı
- Labirent mantığı
- Oyuncu hareketi
- Hazine toplama sistemi
- AABB tabanlı çarpışma kontrolü
- Shader altyapısı
- Texture ve ışıklandırma altyapısı

## Kontroller

- W: İleri hareket
- S: Geri hareket
- A: Sola hareket
- D: Sağa hareket
- Mouse: Kamera yönünü değiştirme
- R: Oyunu sıfırlama için planlandı
- ESC: Çıkış

## Proje Yapısı

```text
TreasureHunter3D/
├── CMakeLists.txt
├── README.md
├── src/
│   └── main.cpp
├── shaders/
│   ├── vertex.glsl
│   └── fragment.glsl
├── assets/
│   ├── textures/
│   └── models/
└── docs/

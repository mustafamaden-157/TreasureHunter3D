# Proje Rapor Özeti

## Proje Adı

TreasureHunter3D - 3B Hazine Toplama Oyunu

## Proje Amacı

Bu projede Bilgisayar Grafikleri dersi kapsamında OpenGL tabanlı basit bir 3B hazine toplama oyunu tasarlanmıştır. Projede amaç, 3B sahne oluşturma, nesne dönüşümleri, kamera kontrolü, shader kullanımı, texture altyapısı, ışıklandırma ve çarpışma kontrolü gibi temel bilgisayar grafikleri konularını uygulamaktır.

## Oyun Senaryosu

Oyuncu, labirent şeklinde oluşturulan 3B bir sahne içerisinde hareket ederek farklı konumlara yerleştirilen hazineleri toplamaya çalışır. Oyuncu duvarlara çarptığında hareketi engellenir. Oyuncu hazineye temas ettiğinde hazine toplanır ve skor artırılır.

## Kullanılan Yapılar

Projede sahnedeki nesneleri temsil etmek için `GameObject` yapısı kullanılmıştır. Duvar ve hazine nesneleri bu yapı ile oluşturulmuştur. Oyuncunun konumu, boyutu ve hızı ise `Player` yapısı içerisinde tutulmuştur.

Labirent tasarımı 0 ve 1 değerlerinden oluşan iki boyutlu bir matris ile yapılmıştır. Matristeki `1` değerleri duvarları, `0` değerleri boş alanları temsil eder.

## Çarpışma Kontrolü

Oyuncunun duvarların içinden geçmesini engellemek ve hazine toplama sistemini oluşturmak için AABB tabanlı çarpışma kontrolü kullanılmıştır. Her nesnenin etrafında görünmeyen bir sınır kutusu oluşturulur. Oyuncunun sınır kutusu duvarla kesişirse hareket engellenir. Hazineyle kesişirse hazine toplanır.

## Shader Kullanımı

Projede iki shader dosyası bulunmaktadır:

- `vertex.glsl`
- `fragment.glsl`

Vertex shader, nesnelerin köşe noktalarını model, view ve projection matrisleriyle ekrana aktarılacak hale getirir. Fragment shader ise texture rengi ve ışıklandırma bileşenleriyle piksel rengini hesaplar.

## Işıklandırma

Fragment shader içerisinde ambient, diffuse ve specular ışık bileşenleri kullanılmıştır. Ambient ışık sahnenin tamamen karanlık olmasını engeller. Diffuse ışık yüzeye gelen ışık açısına göre aydınlatma sağlar. Specular ışık ise parlak yansıma etkisi oluşturur.

## Geliştirilecek Alanlar

- Kamera sisteminin gerçek OpenGL döngüsüne bağlanması
- WASD ve mouse kontrollerinin aktif hale getirilmesi
- Texture dosyalarının eklenmesi
- Hazine nesnelerine dönme animasyonu eklenmesi
- Skorun ekranda gösterilmesi
- Oyun bitiş ekranı eklenmesi

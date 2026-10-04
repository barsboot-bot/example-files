КУДА КЛАСТЬ МОДЕЛЬ И ТЕКСТУРЫ КАССЕТЫ
======================================

1. Модель кассеты (готовая .p3d):
     Unesennye_Data\Data\Gear\Cassette\cassette.p3d

2. Текстуры — ВМЕСТЕ с моделью, в папке tex\:
     Unesennye_Data\Data\Gear\Cassette\tex\cassette_co.png   (color/diffuse, sRGB)
     Unesennye_Data\Data\Gear\Cassette\tex\cassette_no.png   (normal, Linear, BC5)
     Unesennye_Data\Data\Gear\Cassette\tex\cassette_orm.png  (AO/Rough/Metal, Linear)

   ВАЖНО: в самом .p3d (в Materials в DayZ Tools / ArmaReplacer) пути к
   текстурам должны быть ОТНОСИТЕЛЬНЫМИ: "tex\cassette_co.png".
   Тогда при упаковке PBO всё соберётся по адресу:
     \Unesennye_Data\Data\Gear\Cassette\tex\cassette_co.png

3. Как сделать .p3d из своей модели (Blender → DayZ Tools):
   - Blender: смоделируйте кассету (~4х6х1 см), развёртка UV, запеките карты.
   - Экспорт в .fbx (Y-up -> Z-up конвертирует плагин "Arma Tools" для Blender).
   - DayZ Tools (Steam -> Tools -> DayZ Modding Tools):
       "Material Builder"  : png -> .rvmat + параметры (профиль DZ_Default)
       "FBX-to-P3D" (Object Builder): fbx + rvmat -> cassette.p3d (LOD 0 и 1, geo)
   - Конвертация текстур: "Image To P3D"? нет — используйте ImageConverter/DXT:
       co.png -> BC7 (sRGB), no.png -> BC5 (Linear), orm.png -> BC7 (Linear)
     (файлы .edds в DayZ не используются, движок читает .png/.jpg прямо из PBO,
      но лучше сжатые DDS через ImageBuilder, если требует ваша версия).

4. Прописать модель в основном конфиге мода (@Unesennye\config.cpp):
     class UE_Item_Cassette_Rock: ItemBase
     {
         model = "\Unesennye_Data\Data\Gear\Cassette\cassette.p3d";
     };
   (сейчас там стоит заглушка \dz\items\magazine_rifle_556.p3d)

5. Упаковка: скрипт build_pbo.py соберёт Unesennye_Data.pbo автоматически.

Аналогично для других предметов:
     Data\Gear\CassettePlayer\player.p3d + tex\
     Data\Gear\DiskPlayer\disk_player.p3d + tex\
     Data\Gear\RadioReceiver\radio.p3d + tex\
     Data\Gear\CarRadio\car_radio.p3d + tex\
     Data\Gear\Headphones\headphones.p3d + tex\
     Data\Gear\Speaker\speaker.p3d + tex\
     Data\Gear\Disk\disk.p3d + tex\

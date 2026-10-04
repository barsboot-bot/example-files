# Unesennye_Data — аддон ассетов мода «унесённые»

Сюда кладутся **модели (.p3d) и текстуры** всех предметов мода.
Аддон упаковывается в `Unesennye_Data.pbo` скриптом `build_pbo.py`
(запускается из корня репозитория) и раздаётся игрокам как отдельный модпак
(или вместе с @Unesennye).

## Структура (готовые папки, модели = .p3d, текстуры = подпапка tex/)

```
Unesennye_Data/
├─ config.cpp                       # CfgPatches::UE_Data
└─ Data/Gear/
   ├─ Cassette/        cassette.p3d       + tex/   # кассета (+ магазин-кассета)
   ├─ Disk/            disk.p3d           + tex/   # компакт-диск
   ├─ CassettePlayer/  player.p3d         + tex/   # кассетный плеер «Квант»
   ├─ DiskPlayer/      disk_player.p3d    + tex/   # дисковый проигрыватель
   ├─ RadioReceiver/   radio.p3d          + tex/   # коротковолновый приёмник
   ├─ CarRadio/        car_radio.p3d      + tex/   # автомобильная магнитола
   ├─ Headphones/      headphones.p3d     + tex/   # наушники
   └─ Speaker/         speaker.p3d        + tex/   # портативная колонка «Гроза-3»
```

Имена файлов фиксированы — на них ссылается `@Unesennye/config.cpp`
(поле `model = "\Unesennye_Data\Data\Gear\<Папка>\<имя>.p3d"`).
Если хотите другое имя — поправьте config.cpp так же.

## Как положить свои ассеты
1. Смоделируйте предмет в Blender, экспортируйте через DayZ Tools
   (Material Builder → Object Builder) — подробности в
   `Data/Gear/Cassette/README_ASSETS.txt`.
2. Готовый `<имя>.p3d` положите в соответствующую папку `Data/Gear/<Предмет>/`.
3. Текстуры — в `Data/Gear/<Предмет>/tex/`. Пути внутри материала
   указывайте ОТНОСИТЕЛЬНО папки модели: `tex\cassette_co.png`.
4. Запустите `python build_pbo.py` → получите `build_out/@Unesennye/Unesennye_Data.pbo`.
5. Распределите PBO игрокам (Steam Workshop / FTP), при verifySignatures=2 — подпишите ключом сервера.

Пустые папки содержат `.gitkeep`, чтобы структура сохранялась в Git.

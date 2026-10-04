// ============================================================
//  УНЕСЁННЫЕ — ассеты (отдельный аддон, укладывается в PBO как
//  Unesennye_Data.pbo). Модели (.p3d) кладите в Data\, текстуры
//  (.png/.jpg) рядом с моделью в подпапке tex\.
//  Путь в config.cpp: model = "\Unesennye_Data\Data\Gear\Cassette\cassette.p3d";
// ============================================================
class CfgPatches
{
    class UE_Data
    {
        name = "Unesennye Data";
        author = "Unesennye";
        url = "";
        version = 1;
        requiredAddons[] = { "DZ_Data", "DZ_Gear_Electronics" };
        requiredVersion = 0.1;
    };
};

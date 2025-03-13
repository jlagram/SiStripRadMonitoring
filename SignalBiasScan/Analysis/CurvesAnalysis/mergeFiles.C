#include <TSystem.h>
#include <TFile.h>
#include <TChain.h>
#include <iostream>
#include <vector>

void mergeFiles() {
    // Nom du fichier de sortie
    const char* outputFileName = "simulation2025.root";

    // Création d'un TChain pour fusionner les fichiers ROOT
    TChain chain("tree");  // Remplace "tree" par le nom correct de ton TTree

    // Liste des fichiers dans le répertoire courant
    void* dir = gSystem->OpenDirectory("/afs/cern.ch/user/p/pvaucell/public/VdepEvolution/Predictions/");
    const char* file;
    
    // Parcourir tous les fichiers du répertoire
    while ((file = gSystem->GetDirEntry(dir))) {
        TString fileName(file);

        // Sélectionner uniquement les fichiers qui commencent par "lumigr_"
        if (fileName.BeginsWith("lumigr_") && fileName.EndsWith(".root")) {
            std::cout << "Ajout du fichier : " << fileName << std::endl;
            chain.Add(fileName);  // Ajoute chaque fichier ROOT à la chaîne
        }
    }
    gSystem->FreeDirectory(dir);

    // Sauvegarde des données dans le fichier de sortie
    TFile outputFile(outputFileName, "RECREATE");
    if (!outputFile.IsOpen()) {
        std::cerr << "Erreur à l'ouverture du fichier de sortie : " << outputFileName << std::endl;
        return;
    }

    // Fusionner et écrire dans le fichier de sortie
    chain.Merge(&outputFile, 0, "keep");

    std::cout << "Fichier fusionné créé : " << outputFileName << std::endl;
    outputFile.Close();
}

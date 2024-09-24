<?php

	// Fichier php utilisé pour traiter l'envoi de formulaire depuis le site HTTP (modifier les valeurs dans la base de données)
	
	
	// Important : ne pas oublier de Start MySQL dans dans XAMPP pour que ça fonctionne
    $connexionSQL = mysqli_connect("localhost", "root", "", "mesureth"); // Connexion à la base de données

    // Vérification connexion
    if($connexionSQL === false) // Si la connexion a échouée
	{
        die("ERREUR: Connexion impossible. " . mysqli_connect_error());
    }

	// On stocke chaque valeur du formulaire dans une variable
    // Utilisation de "mysqli_real_escape_string" recommandée pour gérer les éventuels caractères spéciaux (par précautions)
	// Pour plus de détails : https://www.lephpfacile.com/manuel-php/mysqli.real-escape-string.php
    $seuil_temperature_min = mysqli_real_escape_string($connexionSQL, $_POST['SeuilTemperatureMin']);
    $seuil_temperature_max = mysqli_real_escape_string($connexionSQL, $_POST['SeuilTemperatureMax']);
    $ecart_temperature = mysqli_real_escape_string($connexionSQL, $_POST['EcartTemperature']);
	
    $seuil_humidite_min = mysqli_real_escape_string($connexionSQL, $_POST['SeuilHumiditeMin']);
    $seuil_humidite_max = mysqli_real_escape_string($connexionSQL, $_POST['SeuilHumiditeMax']);
    $ecart_humidite = mysqli_real_escape_string($connexionSQL, $_POST['EcartHumidite']);

	// Requête SQL UPDATE pour modifier les valeurs dans la base de données
	$requeteSQL = "UPDATE valeurs SET seuilTemperatureMin='$seuil_temperature_min', seuilTemperatureMax='$seuil_temperature_max', ecartTemperature='$ecart_temperature', seuilHumiditeMin='$seuil_humidite_min', seuilHumiditeMax='$seuil_humidite_max', ecartHumidite='$ecart_humidite'";

	// Exécution de la requête SQL
    if(mysqli_query($connexionSQL, $requeteSQL)) // Si requête exécutée avec succès
	{
        echo "Valeurs modifiées !"; // Affichage d'un message d'information sur la page
    } 
	else
	{
        echo "ERREUR: Impossible d'exécuter la requête SQL. $sql. " . mysqli_error($connexionSQL); // Affichage d'un message d'erreur sur la page
    }
	
	echo "<br><br><form><input type='button' value='Retour' onclick='history.back()'></form>"; // Bouton pour revenir à la page précédante
	    
    mysqli_close($connexionSQL); // Fermer la connexion 
?>
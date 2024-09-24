<?php

	// Fichier php utilisé pour traiter les requêtes HTTP depuis l'ESP (recevoir/modifier les données de la base de données)


	header("Content-Type: application/json"); // Indique que le traitement s'effectue en JSON
		
	// Important : ne pas oublier de Start MySQL dans dans XAMPP pour que ça fonctionne
	$connexionSQL = mysqli_connect("localhost", "root", "", "mesureth"); // Connexion à la base de données

	// Vérification connexion
	if($connexionSQL === false) // Si la connexion a échouée
	{
		die("ERREUR: Connexion impossible. " . mysqli_connect_error());
	}

	$typeDeRequete = $_SERVER['REQUEST_METHOD'];

	switch ($typeDeRequete) {
		case 'GET':        		
			TraiterRequeteGet($connexionSQL);	
			break;
		case 'PUT':
			TraiterRequetePut($connexionSQL);
			break;
		default:
			echo json_encode(['message' => 'Type de requete invalide']);
			break;
	}

	mysqli_close($connexionSQL); // Fermer la connexion 


	//// Fonction TraiterRequeteGet (traitement des requêtes HTTP de type GET)
	//// Description: retourne les valeurs stockées dans la base de données du serveur
	//// Entrées: connexionSQL
	//// Sorties: -
	function TraiterRequeteGet($connexionSQL) 
	{
		// Requête SQL pour récupérer les données de la base de données
		$requeteSQL = "SELECT seuilTemperatureMin,seuilTemperatureMax,ecartTemperature,seuilHumiditeMin,seuilHumiditeMax,ecartHumidite FROM valeurs";
		
		$resultatRequeteSQL = mysqli_query($connexionSQL, $requeteSQL); // Exécution de la requête
		if($resultatRequeteSQL) // Si requête exécutée avec succès
		{					
			$colonnes = $resultatRequeteSQL->fetch_assoc(); // Récupèrer les résultats de la requête
			echo json_encode($colonnes); // Encoder les résultats en JSON avec json_encode et transmettre la réponse avec echo
		} 
		else // Echec lors de l'exécution de la requête
		{
			echo "ERREUR: Impossible d'exécuter la requête SQL: $requeteSQL. " . mysqli_error($connexionSQL);
		}
	}

	//// Fonction TraiterRequetePut (traitement des requêtes HTTP de type PUT)
	//// Description: Transmet les valeurs de la requête à la base de données
	//// Entrées: connexionSQL 
	//// Sorties: -
	function TraiterRequetePut($connexionSQL) 
	{			
		parse_str(file_get_contents("php://input"), $putData); // Récupérer les données passées dans la requête HTTP et les stocker dans la variable $putData
		
		if(count($putData) == 0) // Si la requête HTTP ne contient aucune données
		{
			// Pas de paramètres à modifier
			echo "Pas de param";
			return;
		}
			
		// Construction de la requête SQL
		$requeteSQL = "UPDATE valeurs SET";
		
		// Ajout des paramètres (nom et nouvelle valeur) indiqués dans la requête PUT reçue
		foreach ($putData as $nom => $valeur)
		{ 
			// On ajoute chaque donnée reçue (nom/valeur) à la requête SQL
			$requeteSQL .= " $nom='$valeur',"; // .= pour ajouter à la suite du texte existant (+= ne marche pas)
		}
		
		$requeteSQL = trim($requeteSQL, ','); // Pour enlever la virgule en trop à la fin


		//echo $requeteSQL; // Pour debugguer la requête SQL créée
		//return;
		
		// Exécution de la requête
		if(mysqli_query($connexionSQL, $requeteSQL)) // Si requête exécutée avec sucès
		{
			echo "OK!";
		} 
		else // Echec lors de l'exécution de la requête
		{
			echo "ERREUR: Impossible d'exécuter la requête SQL: $requeteSQL. " . mysqli_error($connexionSQL);
		}
	}
?>
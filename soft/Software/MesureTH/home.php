<!DOCTYPE html> <!-- Indispensable, indique qu'il s'agit d'une page HTML -->
<html lang=”fr”>  <!-- Pas obligatoire, mais indique la langue du site -->
    <head>
        <meta charset="utf-8"> <!-- Encodage des caractères : accents, idéogrammes chinois et japonais, etc... -->
        <title>Projet 2409 Mesure TH</title> <!-- Titre du site -->
	</head>
	<body>
		<?php 	
		
		// Important : ne pas oublier de Start MySQL dans dans XAMPP pour que ça fonctionne
		$connexionSQL = mysqli_connect("localhost", "root", "", "mesureth"); // Connexion base de données

		// Vérification connexion
		if($connexionSQL === false) // Si la connexion a échouée
		{
			die("ERREUR: Connexion impossible. " . mysqli_connect_error());
		}

		$requeteSQL = "SELECT * FROM valeurs";
		$resultatRequeteSQL = mysqli_query($connexionSQL, $requeteSQL);
		
		if($resultatRequeteSQL) // Si requête effectuée avec succès
		{
			$colonnes = $resultatRequeteSQL->fetch_assoc(); // Récupérer les résultats
		} 
		else
		{
			echo "ERREUR: Impossible d'executer la requete SQL suivante: $sql. " . mysqli_error($connexionSQL);
		}
		
		mysqli_close($connexionSQL); // Fermer la connexion
	
		?>
		
		<!-- Formulaire -->
		<!-- valeurs initiales provenant de la base de données en utilisant le php -->
		<form method="post" action="form.php"> <!-- Execution de form.php lorsque le formulaire est envoyé avec le bouton Envoyer -->
			<h1>Page pour le réglages des seuils et écarts pour le projet 2409 </h1>  <!-- H1 pour titre -->
			
			<!-- step pour définir le la précision des valeurs -->			
			<label>Seuil température min :</label>
			<input type="number" step="0.01" name="SeuilTemperatureMin" id="SeuilTemperatureMin" value="<?php echo $colonnes["seuilTemperatureMin"]; ?>"> [°C]<br>
			
			<label>Seuil température max :</label> 
			<input type="number" step="0.01" name="SeuilTemperatureMax" id="SeuilTemperatureMax" value="<?php echo $colonnes["seuilTemperatureMax"]; ?>"> [°C]<br>			
			<br>
			
			<label for="EcartTempMax">Ecart température :</label>
			<input type="number" step="0.01" min="0" name="EcartTemperature" id="EcartTemperature" value="<?php echo $colonnes["ecartTemperature"]; ?>"> [°C]<br>
			<br>
					
			<label>Seuil humidité min :</label>
			<input type="number" min="0" max="100" name="SeuilHumiditeMin" id="SeuilHumiditeMin" value="<?php echo $colonnes["seuilHumiditeMin"]; ?>"> [%]<br>
			
			<label>Seuil humidité max :</label>
			<input type="number" min="0" max="100" name="SeuilHumiditeMax" id="SeuilHumiditeMax" value="<?php echo $colonnes["seuilHumiditeMax"]; ?>"> [%]<br>		
			<br>
			
			<label>Ecart humidité :</label>
			<input type="number" min="0" max="100" name="EcartHumidite" id="EcartHumidite" value="<?php echo $colonnes["ecartHumidite"]; ?>"> [%]<br> 
			<br>
			
			<input type="submit" value="Envoyer"> <!-- Bouton pour envoyer le formulaire -->
		 </form>
		 
		 
		<br><br>
		
		<?php
		// Si l'alarme vaut vrai dans la base de données (valeur transmise par le STM à l'ESP, puis par l'ESP à la base de données)
		if($colonnes["alarme"] == true)
		{
			echo "<div style='color:red; text-align: center; font-size:200px'>Alarme !</div>"; // Afficher un message d'alerte sur la page
		}
		
		// Si pilesFaibles vaut vrai dans la base de données (valeur transmise par le STM à l'ESP, puis par l'ESP à la base de données)
		if($colonnes["pilesFaibles"] == true)
		{
			echo "<div style='color:red; text-align: center; font-size:200px'>Piles faibles !</div>"; // Afficher un message d'alerte sur la page
		}

		?>
    </body>
  
</html>